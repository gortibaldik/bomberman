#ifndef SERVER_DEF_HPP
#define SERVER_DEF_HPP

#include <SFML/Network.hpp>
#include "network/network_params.hpp"
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>

struct ClientInfo {
    sf::IpAddress ip;
    PortNumber port;
    bool waiting_for_heartbeat = false;
    sf::Time last_sent_heartbeat;
    sf::Time latency;
    int retries = 0;

    ClientInfo(const sf::IpAddress& ip, PortNumber port, const sf::Time& t)
              : ip(ip), port(port), last_sent_heartbeat(t) {}
    
    ClientInfo& operator=(const ClientInfo& c) {
        ip = c.ip;
        port = c.port;
        return *this;
    }

    enum TimeDifference {
        HeartBeat,
        TimeOut,
        Correct
    };

    TimeDifference check_time(const sf::Time&);
    void uncheck_heartbeat(const sf::Time&);
};

using Clients = std::unordered_map<std::string, ClientInfo>;

class Server {
public:
    Server(): max_clients(2), running(false), can_add(true) {}
    Server(int max_clients): max_clients(max_clients), running(false), can_add(true) {}
    ~Server();
    bool start(PortNumber port);
    void listen();
    void send(const std::string&, sf::Packet&);
    void update(const sf::Time& dt);
    void disable_adding_new_players() { can_add = false; }
    void enable_adding_new_players() { can_add = true; }
    void terminate();
    bool is_running() { return running; }
protected:
    void update_time_overflow();
    ClientInfo* find_by_ip_port(const sf::IpAddress&, PortNumber);
    Clients connected_clients;
    Clients to_be_erased;

    sf::Time c_time; /* default constructor sets the time to zero \*/
    sf::UdpSocket incoming_socket, outcoming_socket;
    std::thread worker;
    std::mutex clients_mutex;
    std::atomic<bool> running;
    std::atomic<bool> can_add;
    const int max_clients;
};

#endif