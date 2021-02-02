#ifndef SERVER_DEF_HPP
#define SERVER_DEF_HPP

#include <SFML/Network.hpp>
#include "network/network_params.hpp"
#include <unordered_map>
#include <thread>

struct ClientInfo {
    sf::IpAddress ip;
    PortNumber port;

    ClientInfo(const sf::IpAddress& ip, PortNumber port)
              : ip(ip), port(port) {}
    
    ClientInfo& operator=(const ClientInfo& c) {
        ip = c.ip;
        port = c.port;
        return *this;
    }
};

using Clients = std::unordered_map<ClientID, ClientInfo>;

class Server {
public:
    ~Server();
    bool start(PortNumber port);
    void listen();
protected:

    sf::UdpSocket incoming_socket, outcoming_socket;
    std::thread worker;
    bool is_running = false;
};

#endif