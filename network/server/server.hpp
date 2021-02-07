#ifndef SERVER_DEF_HPP
#define SERVER_DEF_HPP

#include <SFML/Network.hpp>
#include "network/network_params.hpp"
#include "network/utils.hpp"
#include "client_info.hpp"
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>

using Clients = std::unordered_map<std::string, ClientInfo>;

class Server {
public:
    Server(int max_clients): max_clients(max_clients), running(false), can_add(true){}
    Server(): Server(2) {}
    ~Server();
    bool start(PortNumber port);
    void listen();

    void send(const std::string&, sf::Packet&);
    void broadcast(sf::Packet&);
    ReceiverQueue& get_received_messages() { return received_messages; }

    void update(const sf::Time& dt);

    void disable_adding_new_players() { can_add = false; }
    void enable_adding_new_players() { can_add = true; }

    void terminate();

    bool is_running() { return running; }
    std::vector<std::string> get_connected_clients();
protected:
    void handle_heartbeat(const sf::IpAddress&, PortNumber);
    void handle_connection_attempt(const sf::IpAddress&, PortNumber, sf::Packet&);
    void handle_disconnect(const sf::IpAddress&, PortNumber);
    virtual void handle_others(sf::Packet&, PacketType) = 0;
    void update_time_overflow();
    Clients::iterator find_by_ip_port(const sf::IpAddress&, PortNumber);

    Clients connected_clients;
    Clients to_be_erased;

    sf::Time c_time; /* default constructor sets the time to zero \*/
    sf::UdpSocket incoming_socket, outcoming_socket;

    std::thread worker;
    std::mutex clients_mutex;
    std::atomic<bool> running;
    std::atomic<bool> can_add;
    const int max_clients;

    ReceiverQueue received_messages;
};

#endif