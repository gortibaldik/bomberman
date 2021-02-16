#include "base.hpp"
#include "network/packet_types.hpp"
#include <iostream>

enum class WAIT_RESULT {
    CONTINUE,
    SUCCESS,
    FAILURE
};

static void initialize_init_packet(const std::string& name, sf::Packet& p) {
    add_type_to_packet(p, PacketType::Connect);
    p << name;
}

static bool try_send_to_server(sf::UdpSocket& socket,
                                sf::Packet& p,
                                const sf::IpAddress& server_ip,
                                PortNumber server_port) {
    return (socket.send(p, server_ip, server_port) == sf::Socket::Done);
}

static WAIT_RESULT wait_for_server(  sf::UdpSocket& socket
                                   , const sf::IpAddress& server_ip
                                   , sf::Packet& packet
                                   , sf::IpAddress& receiver_ip
                                   , PortNumber& receiver_port
                                   , PacketType& ptype) {
    auto status = socket.receive(packet, receiver_ip, receiver_port);
    if (status != sf::Socket::Done || receiver_ip != server_ip) {
        return WAIT_RESULT::CONTINUE;
    }
    sf::Int8 raw_ptype = 0;
    if (!(packet >> raw_ptype)) { return WAIT_RESULT::FAILURE; }
    ptype = static_cast<PacketType>(raw_ptype);
    return WAIT_RESULT::SUCCESS;
}

static WAIT_RESULT listen_to_server( sf::UdpSocket& socket
                                   , const sf::IpAddress& server_ip
                                   , sf::Packet& packet
                                   , sf::IpAddress& receiver_ip
                                   , PortNumber& receiver_port
                                   , PacketType& ptype) {
    auto status = socket.receive(packet, receiver_ip, receiver_port);
    if (status != sf::Socket::Done) {
        return WAIT_RESULT::FAILURE; 
    }
    // unknown sender, or undecodable packet
    sf::Int8 raw_ptype = 0;
    if ((receiver_ip != server_ip) || !(packet >> raw_ptype)) { return WAIT_RESULT::CONTINUE; }
    ptype = static_cast<PacketType>(raw_ptype);
    // invalid type of packet
    if ((ptype < PacketType::Disconnect) || (ptype >= PacketType::Invalid)) { return WAIT_RESULT::CONTINUE; }
    return WAIT_RESULT::SUCCESS;
}

bool Client::connect(const sf::IpAddress& server_ip, PortNumber server_port) {
    if (is_connected()) {
        return false;
    }
    status = ClientStatus::TryingToConnect;
    socket.bind(sf::Socket::AnyPort);
    std::cout << "CLIENT : sending and listening on: " << socket.getLocalPort() << std::endl;
    sf::Packet packet;
    initialize_init_packet(player_name, packet);
    if (!try_send_to_server(socket, packet, server_ip, server_port)) {
        socket.unbind();
        status = ClientStatus::Failed;
        return false;
    }
    socket.setBlocking(false);
    sf::Clock timer;
    timer.restart();
    std::cout << "CLIENT : connecting to " << server_ip << ":" << server_port << std::endl;
    while (timer.getElapsedTime().asMilliseconds() < sf::Int32(Network::ClientConnectTimeOut)) {
        packet.clear();
        sf::IpAddress receiver_ip;
        PortNumber receiver_port;
        PacketType ptype = PacketType::Invalid;
        auto result = wait_for_server(socket, server_ip, packet, receiver_ip, receiver_port, ptype);
        if (result == WAIT_RESULT::FAILURE) { break; }
        if (ptype == PacketType::Duplicate) {
            std::cout << "CLIENT : got response from server : DUPLICATE NAME !" << std::endl;
            status = ClientStatus::Duplicate;
            return false;
        }
        if ((result == WAIT_RESULT::CONTINUE) || (ptype != PacketType::Connect)) { continue; }
        this->server_ip = receiver_ip;
        this->server_port_out = receiver_port;
        this->server_port_in = server_port;

        status = ClientStatus::Connected;
        socket.setBlocking(true);
        updater = std::thread(&Client::update_loop, this);
        listener = std::thread(&Client::listen, this);
        std::cout << "CLIENT : STATUS CONNECTED" << std::endl;
        return true;
    }
    status = ClientStatus::Failed;
    std::cout << "CLIENT : STATUS FAILURE" << std::endl;
    return false;
}

/* Keep the actual connection with the server until the server
 * responds to the heartbeats
 *  - doesn't interfere with any other interface of the client 
 *    (except answering heartbeat messages to server)
 */
void Client::update_loop() {
    using namespace std::chrono_literals;
    sf::Clock c;
    c.restart();
    while(is_connected()) {
        server_time += c.restart();
        // overflow may happen after really long time
        // but just for the sake of completeness...
        if (server_time.asMilliseconds() < 0) {
            update_time_overflow();
        }
        // if the client hadn't received the heartbeat response
        // from the server for long enough, it terminates the connection
        if (server_time.asMilliseconds() - last_heart_beat.asMilliseconds() >= Network::ClientTimeOut) {
            std::cout << "CLIENT : server timed out!" << std::endl;
            terminate();
        }
        std::this_thread::sleep_for(100ms);
    }
    terminate();
}

void Client::handle_heartbeat(sf::Packet& p) {
    sf::Int32 t;
    sf::Packet packet;
    if (!(p >> t)) {
        std::cout << "CLIENT : invalid heartbeat!" << std::endl;
    }
    server_time = sf::milliseconds(t);
    last_heart_beat = server_time;
    add_type_to_packet(packet, PacketType::HeartBeat);
    if (socket.send(packet, this->server_ip, this->server_port_in) != sf::Socket::Done) {
        std::cout << "CLIENT : failed to respond to a heartbeat!" << std::endl;
    }
}

void Client::send(sf::Packet& packet) {
    socket.send(packet, server_ip, server_port_in);
}

void Client::listen() {
    while (is_connected()) {
        sf::Packet packet;
        sf::IpAddress receiver_ip;
        PortNumber receiver_port;
        PacketType ptype = PacketType::Invalid;
        auto result = listen_to_server(socket, server_ip, packet, receiver_ip, receiver_port, ptype);
        if ((result == WAIT_RESULT::FAILURE) && !is_connected()) { break; }
        if ((result == WAIT_RESULT::CONTINUE) || (result == WAIT_RESULT::FAILURE)) { continue; }
        switch(ptype) {
        case PacketType::HeartBeat:
            handle_heartbeat(packet);
            break;
        case PacketType::Disconnect:
            socket.unbind();
            status = ClientStatus::Terminated;
            notify_disconnect();
            break;
        default:
            handle_others(packet, ptype);
        }
    }
    status = ClientStatus::Terminated;
}

void Client::update_time_overflow() {
    server_time -= sf::milliseconds(Network::HighestTimeStamp);
    last_heart_beat = server_time;
}

void Client::terminate() {
    if(is_connected()) {
        status = ClientStatus::Terminated;
        sf::Packet p;
        add_type_to_packet(p, PacketType::Disconnect);
        socket.send(p, server_ip, server_port_in);
        socket.send(p, sf::IpAddress::getLocalAddress(), socket.getLocalPort());
        std::cout << "CLIENT : sent disconnect packet!" << std::endl;
        socket.unbind();
        if (listener.joinable()) {
            listener.join();
            std::cout << "CLIENT : joined listener!" << std::endl;
        } else {
            std::cout << "CLIENT : listener already joined!" << std::endl;
        }
    }
}

Client::~Client() {
    // end both updater and listener threads
    if (listener.joinable()) {
        listener.join();
        std::cout << "CLIENT : joined listener!" << std::endl;
    } else {
        std::cout << "CLIENT : listener already joined!" << std::endl;
    }

    if (updater.joinable()) {
        updater.join();
        std::cout << "CLIENT : joined updater!" << std::endl;
    }
    else {
        std::cout << "CLIENT : updater already joined!" << std::endl;
    }
}