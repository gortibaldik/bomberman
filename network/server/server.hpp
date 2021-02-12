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
    Server(): max_clients(0), running(false), can_add(true){}
    virtual ~Server();
    PortNumber start(PortNumber port);
    void listen();

    void send(const std::string&, sf::Packet&);
    void broadcast(sf::Packet&);

    void disable_adding_new_clients() { can_add = false; }
    void enable_adding_new_clients() { can_add = true; }

    void set_max_clients(int);

    void terminate();

    bool is_running() { return running; }
    std::vector<std::string> get_connected_clients();
protected:
    void update(const sf::Time& dt);
    void update_loop();
    void handle_heartbeat(const sf::IpAddress&, PortNumber);
    void handle_connection_attempt(const sf::IpAddress&, PortNumber, sf::Packet&);
    void handle_disconnect(const sf::IpAddress&, PortNumber);
    virtual void handle_others(const std::string& client_name, sf::Packet&, PacketType) = 0;
    virtual void notify_disconnect(const std::string& name) = 0;
    void update_time_overflow();
    Clients::iterator find_by_ip_port(const sf::IpAddress&, PortNumber);

    Clients connected_clients;
    Clients to_be_erased;

    sf::Time c_time; /* default constructor sets the time to zero \*/
    sf::UdpSocket incoming_socket, outcoming_socket;

    std::thread listener;
    std::thread updater;
    std::mutex clients_mutex;
    std::atomic<bool> running;
    std::atomic<bool> can_add;
    int max_clients;
};

#endif