#ifndef CLIENT_DEF_HPP
#define CLIENT_DEF_HPP

#include <SFML/Network.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "network/network_params.hpp"
#include "network/packet_types.hpp"

enum class ClientStatus {
    Connected,
    TryingToConnect,
    Failed,
    NotStarted,
    Terminated
};

class Client {
public:
    Client(const std::string& player_name): player_name(player_name), status(ClientStatus::NotStarted) {}
    bool connect(const sf::IpAddress&, PortNumber);
    void listen();
    ClientStatus get_status() { return status; }
    bool is_connected() { return status == ClientStatus::Connected; }
    void terminate();
protected:
    bool handle_first_server_answer(sf::Packet&, sf::Int8);
    friend class CA; /* Client Accessor */
    std::string player_name;

    std::atomic<ClientStatus> status;
    sf::IpAddress server_ip;
    PortNumber server_port_out, server_port_in;
    sf::Time server_time;

    sf::UdpSocket socket;
    std::mutex socket_mutex;
    std::thread worker;
};

#endif