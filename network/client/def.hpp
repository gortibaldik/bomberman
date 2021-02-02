#ifndef CLIENT_DEF_HPP
#define CLIENT_DEF_HPP

#include <SFML/Network.hpp>
#include <thread>
#include <string>
#include "network/network_params.hpp"
#include "network/packet_types.hpp"

class Client {
public:
    Client(const std::string& player_name): player_name(player_name) {}
    bool connect(const sf::IpAddress&, PortNumber);
    void listen();
protected:
    virtual bool handle_first_server_answer(sf::Packet&, sf::Int8) { return true;}
    friend class CA; /* Client Accessor */
    std::string player_name;

    sf::IpAddress server_ip;
    PortNumber server_port;

    sf::UdpSocket socket;
    bool is_connected = false;
    std::thread worker;
};

#endif