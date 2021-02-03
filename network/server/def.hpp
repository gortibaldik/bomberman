#ifndef SERVER_DEF_HPP
#define SERVER_DEF_HPP

#include <SFML/Network.hpp>
#include "network/network_params.hpp"
#include <unordered_map>
#include <thread>
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
    ~Server();
    bool start(PortNumber port);
    void listen();
    void send(const std::string&, sf::Packet&);
    void update(const sf::Time& dt);
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
    bool running = false;
};

#endif