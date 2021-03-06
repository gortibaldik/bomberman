#include "server.hpp"
#include <iostream>
#include "network/packet_types.hpp"

PortNumber Server::start(PortNumber port) {
    if (running) {
        return 0;
    }
    if (incoming_socket.bind(port) != sf::Socket::Done) {
        return 0;
    }
    outcoming_socket.bind(sf::Socket::AnyPort);
    std::cout << "SERVER : Local IP address: " << sf::IpAddress::getLocalAddress() << std::endl;
    std::cout << "SERVER : Incoming port: " << incoming_socket.getLocalPort() << std::endl;
    std::cout << "SERVER : Outcoming port: " << outcoming_socket.getLocalPort() << std::endl;
    running = true;
    heartbeater = std::thread(&Server::update_loop, this);
    listener = std::thread(&Server::listen, this);
    return incoming_socket.getLocalPort();
}

void Server::listen() {
    sf::IpAddress ip;
    PortNumber port;
    while (is_running()) {
        sf::Packet packet;
        auto status = incoming_socket.receive(packet, ip, port);
        if (status != sf::Socket::Done) { continue; }

        sf::Int8 id;
        if (!(packet >> id)) {
            continue;
        }
        std::string client_name;

        switch(static_cast<PacketType>(id)){
        case PacketType::Connect:
            handle_connection_attempt(ip, port, packet);
            break;
        case PacketType::HeartBeat:
            handle_heartbeat(ip, port);
            break;
        case PacketType::Disconnect:
            handle_disconnect(ip, port);
            break;
        default:
            {
                std::unique_lock<std::mutex> l(clients_mutex);
                auto iter = find_by_ip_port(ip, port);
                if (iter == connected_clients.end()) {
                    std::cout << "Unknown client!" << std::endl;
                    break;
                }
                client_name = iter->first;
            }
            listener_handle_others(client_name, packet, static_cast<PacketType>(id));
        }
    }
}

void Server::handle_disconnect(const sf::IpAddress& ip, PortNumber port) {
    std::string name = "";
    {
        std::unique_lock<std::mutex> l(clients_mutex);
        auto ptr = find_by_ip_port(ip, port);
        if (ptr == connected_clients.end()) {
            return; // ignore non existent client error
        }
        std::cout << "SERVER : " << ptr->first << " wants to  disconnect!" << std::endl;
        name = ptr->first;
        connected_clients.erase(name);
    }
    notify_disconnect(name);
}

void Server::handle_heartbeat(const sf::IpAddress& ip, PortNumber port) {
    std::unique_lock<std::mutex> l(clients_mutex);
    auto ptr = find_by_ip_port(ip, port);
    if (ptr == connected_clients.end()) {
        std::cout << "SERVER : HeartBeat received from unknown user!" << std::endl;
        return;
    } else if (!ptr->second.waiting_for_heartbeat) {
        std::cout << "SERVER : Got HeartBeat, haven't waited for it!" << std::endl;
        return;
    }
    //std::cout << "HeartBeat from " << ptr->second.ip << std::endl;
    ptr->second.uncheck_heartbeat(c_time);
}

void Server::handle_connection_attempt(const sf::IpAddress& ip, PortNumber port, sf::Packet& packet) {
    std::cout << "SERVER : " <<  ip << " is trying to connect" << std::endl;
    std::string name;
    if (!(packet >> name)) {
        std::cout << "SERVER : Invalid connect packet!" << std::endl;
        return;
    }
    sf::Packet p;
    p.clear();
    std::unique_lock<std::mutex> l(clients_mutex);
    auto it = connected_clients.find(name);
    if (it != connected_clients.end()){ 
        std::cout << "SERVER : Duplicate name!" << std::endl;
        add_type_to_packet(p, PacketType::Duplicate);
    } else if (can_add && ((connected_clients.size() <= max_clients) || (max_clients == 0))) {

        // send ack packet
        std::cout << "SERVER : Adding " << name << " to the list of connected clients!" << std::endl;
        add_type_to_packet(p, PacketType::Connect);
        connected_clients.insert(std::make_pair(name, ClientInfo(ip, port, c_time)));
    } else {

        // refuse connection
        std::cout << "SERVER : Cannot add any more users" << std::endl;
        add_type_to_packet(p, PacketType::Invalid);
    }
    outcoming_socket.send(p, ip, port);
}

Clients::iterator Server::find_by_ip_port(const sf::IpAddress& ip, PortNumber port) {
    for (auto&& client : connected_clients) {
        if ((client.second.ip == ip) && (client.second.port == port)) {
            return connected_clients.find(client.first);
        }
    }
    return connected_clients.end();
}

void Server::set_max_clients(int n) {
    if (n < connected_clients.size()) {
        throw std::runtime_error("Set invalid number of max connected clients!");
    }
    max_clients = n;
}

void Server::send(const std::string& receiver, sf::Packet& p) {
    std::unique_lock<std::mutex> l(clients_mutex);
    auto&& c = connected_clients.at(receiver);
    outcoming_socket.send(p, c.ip, c.port);
}

void Server::broadcast(sf::Packet& p) {
    std::unique_lock<std::mutex> l(clients_mutex);
    for (auto&& client : connected_clients) {
        outcoming_socket.send(p, client.second.ip, client.second.port);
    }
}

std::vector<std::string> Server::get_connected_clients() {
    std::unique_lock<std::mutex> l(clients_mutex);
    std::vector<std::string> result;
    for (auto&& client: connected_clients) {
        result.push_back(client.first);
    }
    return result;
}

void Server::update_loop() {
    sf::Clock c;
    using namespace std::chrono_literals;
    while(is_running()) {
        update(c.restart());
        std::this_thread::sleep_for(100ms);
    }
    terminate();
}

/* Server::update handles heartbeating, sends ack packets
 *  to all the clients and looks if the connection is still alive 
 * @param dt (delta time) // how much time has passed since last call to update
 */
void Server::update(const sf::Time& dt) {
    c_time += dt;
    if (c_time.asMilliseconds() < 0) {
        update_time_overflow();
    }
    to_be_erased.clear();
    for (auto&& client : connected_clients) {
        sf::Packet hb;
        switch(client.second.check_time(c_time)) {
        case ClientInfo::TimeDifference::HeartBeat:
            add_type_to_packet(hb, PacketType::HeartBeat);
            hb << c_time.asMilliseconds();
            send(client.first, hb);
            break;
        case ClientInfo::TimeDifference::TimeOut:
            std::cout << "SERVER : Client " << client.first << " timed out!" << std::endl;
            to_be_erased.insert(client);
            break;
        }
    }
    std::unique_lock<std::mutex> l(clients_mutex);
    for (auto&& client : to_be_erased) {
        std::cout << "SERVER : removing " << client.first << std::endl;
        connected_clients.erase(client.first);
    }
}

void Server::update_time_overflow() {
    c_time -= sf::milliseconds(Network::HighestTimeStamp);
    std::unique_lock<std::mutex> l(clients_mutex);
    for (auto&& client : connected_clients) {
        auto hb = client.second.last_sent_heartbeat.asMilliseconds();
        client.second.last_sent_heartbeat = sf::milliseconds(std::abs( hb - Network::HighestTimeStamp));
    }
}

void Server::terminate() {
    if(is_running()) {
        running = false;
        sf::Packet p;
        add_type_to_packet(p, PacketType::Disconnect);
        broadcast(p);
        outcoming_socket.send(p, sf::IpAddress::getLocalAddress(), incoming_socket.getLocalPort());
        std::cout << "SERVER : sent terminate socket" << std::endl;
        incoming_socket.unbind();
        if (listener.joinable()) {
            listener.join();
        }
    }
}

Server::~Server() {
    terminate();
    if (heartbeater.joinable()) {
        heartbeater.join();
        std::cout << "SERVER : joined heartbeater!" << std::endl;
    } else {
        std::cout << "SERVER : heartbeater already joined!" << std::endl;
    }
    if (listener.joinable()) {
        listener.join();
        std::cout << "SERVER : joined listener!" << std::endl;
    } else {
        std::cout << "SERVER : listener already joined!" << std::endl;
    }
}