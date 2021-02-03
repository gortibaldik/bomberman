#include "def.hpp"
#include "network/packet_types.hpp"
#include <iostream>

enum WAIT_RESULT {
    CONTINUE,
    SUCCESS,
    FAILURE
};

/* Client Accessor */
class CA {
public:
    static void initialize_init_packet(const Client& c, sf::Packet& p) {
        add_type_to_packet(p, PacketType::Connect);
        p << c.player_name;
    }

    static bool try_send_to_server(Client& c,
                                  sf::Packet& p,
                                  const sf::IpAddress& server_ip,
                                  PortNumber server_port) {
        return (c.socket.send(p, server_ip, server_port) == sf::Socket::Done);
    }

    static WAIT_RESULT wait_for_server(Client& c, const sf::IpAddress& server_ip) {
        auto status = c.socket.receive(packet, receiver_ip, receiver_port);
        if (status != sf::Socket::Done || receiver_ip != server_ip) {
            std::cout << receiver_ip << "!=" << server_ip << std::endl;
            return CONTINUE;
        }
        if (!(packet >> ptype)) { return FAILURE; }
        return SUCCESS;
    }
    
    static WAIT_RESULT listen_to_server(Client& c) {
        auto status = c.socket.receive(packet, receiver_ip, receiver_port);
        if (status != sf::Socket::Done) {
            if (c.connected) {
                return CONTINUE;
            }
            // packet receiving failed because the connection was closed
            return FAILURE; 
        }
        // unknown sender, or undecodable packet
        if ((receiver_ip != c.server_ip) || !(packet >> ptype)) { return CONTINUE; }
        auto t = (PacketType)ptype;
        // invalid type of packet
        if ((t < PacketType::Disconnect) || (t >= PacketType::Invalid)) { return CONTINUE; }
        return SUCCESS;

    }

    static sf::Int8 ptype;
    static sf::IpAddress receiver_ip;
    static PortNumber receiver_port;
    static sf::Packet packet;

};
sf::Int8 CA::ptype;
sf::IpAddress CA::receiver_ip;
PortNumber CA::receiver_port;
sf::Packet CA::packet;

bool Client::connect(const sf::IpAddress& server_ip, PortNumber server_port) {
    if (connected) {
        return false;
    }
    socket.bind(sf::Socket::AnyPort);
    std::cout << "Client sending and listening on: " << socket.getLocalPort() << std::endl;
    sf::Packet packet;
    CA::initialize_init_packet(*this, packet);
    if (! CA::try_send_to_server(*this, packet, server_ip, server_port)) {
        socket.unbind();
        return false;
    }
    socket.setBlocking(false);
    sf::Clock timer;
    timer.restart();
    std::cout << "Connecting to " << server_ip << ":" << server_port << std::endl;
    while (timer.getElapsedTime().asMilliseconds() < sf::Int32(Network::ClientConnectTimeOut)) {
        std::cout << "WAITING" << std::endl;
        auto result = CA::wait_for_server(*this, server_ip);
        auto ptype = (PacketType)CA::ptype;
        if (result == FAILURE) { break; }
        if (ptype == PacketType::Duplicate) {
            std::cout << "Duplicate name !" << std::endl;
            break;
        }
        if ((result == CONTINUE) || (ptype != PacketType::Connect)) { continue; }
        if (!handle_first_server_answer(CA::packet, CA::ptype)) { break; };
        this->server_ip = CA::receiver_ip;
        this->server_port_out = CA::receiver_port;
        this->server_port_in = server_port;

        connected = true;
        socket.setBlocking(true);
        worker = std::thread(&Client::listen, this);
        std::cout << "SUCCESS" << std::endl;
        return true;
    }
    std::cout << "FAILURE" << std::endl;
    return false;
}

void Client::listen() {
    sf::Packet packet;
    while (connected) {
        packet.clear();
        auto result = CA::listen_to_server(*this);
        if (result == FAILURE) { break; }
        if (result == CONTINUE) { continue; }
        auto ptype = (PacketType)CA::ptype;
        if (ptype == PacketType::HeartBeat) {
            sf::Int32 t;
            CA::packet >> t;
            server_time = sf::milliseconds(t);
            std::cout << "Received a heartbeat" << std::endl;
            add_type_to_packet(packet, PacketType::HeartBeat);
            if (socket.send(packet, this->server_ip, this->server_port_in) != sf::Socket::Done) {
                std::cout << "Failed to respond to a heartbeat!" << std::endl;
            }
        }
    }
}

void Client::terminate() {
    if(connected) {
        connected = false;
        socket.unbind();
        worker.join();
    }
}

bool Client::handle_first_server_answer(sf::Packet& p, sf::Int8 ptype) {
    return true;
}