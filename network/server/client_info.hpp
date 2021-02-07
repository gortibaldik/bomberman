#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP

#include <SFML/Network.hpp>
#include "network/network_params.hpp"

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

#endif
