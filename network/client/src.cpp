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
        p << sf::Int8(PacketType::Connect);
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
        if ((PacketType)ptype != PacketType::Connect) { return CONTINUE; }
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
    if (is_connected) {
        return false;
    }
    socket.bind(sf::Socket::AnyPort);
    sf::Packet p;
    CA::initialize_init_packet(*this, p);
    if (! CA::try_send_to_server(*this, p, server_ip, server_port)) {
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
        if (result == FAILURE) { break; }
        if (result == CONTINUE) { continue; }
        if (!handle_first_server_answer(CA::packet, CA::ptype)) { break; };
        is_connected = true;
        socket.setBlocking(true);
        worker = std::thread(&Client::listen, this);
        std::cout << "SUCCESS" << std::endl;
        return true;
    }
    std::cout << "FAILURE" << std::endl;
    return false;
}

void Client::listen() {

}