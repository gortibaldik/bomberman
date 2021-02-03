#include "def.hpp"
#include <iostream>
#include "network/packet_types.hpp"

bool Server::start(PortNumber port) {
    if (running) {
        return false;
    }
    if (incoming_socket.bind(port) != sf::Socket::Done) {
        return false;
    }
    outcoming_socket.bind(sf::Socket::AnyPort);
    std::cout << "Local IP address: " << sf::IpAddress::getLocalAddress() << std::endl;
    std::cout << "Incoming port: " << incoming_socket.getLocalPort() << std::endl;
    std::cout << "Outcoming port: " << outcoming_socket.getLocalPort() << std::endl;
    worker = std::thread(&Server::listen, this);
    running = true;
    return true;
}

void Server::listen() {
    sf::IpAddress ip;
    PortNumber port;
    sf::Packet packet;
    while (running) {
        packet.clear();
        auto status = incoming_socket.receive(packet, ip, port);
        if (status != sf::Socket::Done) { continue; }
        std::cout << "Received something!" << std::endl;

        sf::Int8 id;
        if (!(packet >> id)) {
            continue;
        }

        PacketType pt = (PacketType)id;
        if (pt == PacketType::Connect) {
            std::cout << ip << " is trying to connect" << std::endl;
            std::string name;
            if (!(packet >> name)) {
                std::cout << "Invalid connect packet!" << std::endl;
                continue;
            }
            sf::Packet p;
            auto it = connected_clients.find(name);
            if (it != connected_clients.end()){ 
                std::cout << "Duplicate name!" << std::endl;
                add_type_to_packet(p, PacketType::Duplicate);
            } else {
                // send ack packet
                std::cout << "Adding " << name << " to the list of connected clients!" << std::endl;
                add_type_to_packet(p, PacketType::Connect);
                connected_clients.insert(std::make_pair(name, ClientInfo(ip, port, c_time)));
            }
            outcoming_socket.send(p, ip, port);
            continue;
        }

        if (pt == PacketType::HeartBeat) {
            auto ptr = find_by_ip_port(ip, port);
            if (!ptr) {
                std::cout << "HeartBeat received from unknown user!" << std::endl;
                continue;
            } else if (!ptr->waiting_for_heartbeat) {
                std::cout << "Got HeartBeat, haven't waited for it!" << std::endl;
                continue;
            }
            ptr->uncheck_heartbeat(c_time);
            std::cout << "Got heartbeat from " << ptr->ip << "; latency " << ptr->latency.asMilliseconds() << std::endl;
        }
    }
}

ClientInfo* Server::find_by_ip_port(const sf::IpAddress& ip, PortNumber port) {
    for (auto&& client : connected_clients) {
        if ((client.second.ip == ip) && (client.second.port == port)) {
            return &client.second;
        }
    }
    return nullptr;
}

ClientInfo::TimeDifference ClientInfo::check_time(const sf::Time& server_time) {
    auto elapsed = server_time.asMilliseconds() - last_sent_heartbeat.asMilliseconds();
    if (elapsed >= Network::ClientTimeOut) {
        return TimeDifference::TimeOut;
    }
    if (elapsed < (retries + 1) * Network::HeartBeatInterval) {
        return TimeDifference::Correct;
    }
    if (retries == 0) {
        waiting_for_heartbeat = true;
        last_sent_heartbeat = server_time;
    }
    retries++;
    return TimeDifference::HeartBeat;
}

void ClientInfo::uncheck_heartbeat(const sf::Time& c_time) {
    latency = c_time - last_sent_heartbeat;
    retries = 0;
    waiting_for_heartbeat = false;
}

void Server::send(const std::string& receiver, sf::Packet& p) {
    auto&& c = connected_clients.at(receiver);
    outcoming_socket.send(p, c.ip, c.port);
}

void Server::update(const sf::Time& dt) {
    c_time += dt;
    if (c_time.asMilliseconds() > Network::HighestTimeStamp) {
        update_time_overflow();
    }
    to_be_erased.clear();
    for (auto&& client : connected_clients) {
        sf::Packet hb;
        switch(client.second.check_time(c_time)) {
        case ClientInfo::TimeDifference::HeartBeat:
            std::cout << "Sending heartBeat to " <<  client.first << std::endl;
            add_type_to_packet(hb, PacketType::HeartBeat);
            hb << c_time.asMilliseconds();
            send(client.first, hb);
            break;
        case ClientInfo::TimeDifference::TimeOut:
            std::cout << "Client " << client.first << " timed out!" << std::endl;
            to_be_erased.insert(client);
            break;
        }
    }
    std::unique_lock<std::mutex> l(clients_mutex);
    for (auto&& client : to_be_erased) {
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

Server::~Server() {
    worker.join();
}