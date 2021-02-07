#ifndef NETWORK_UTILS_HPP
#define NETWORK_UTILS_HPP

#include <queue>
#include <utility>
#include <atomic>
#include <mutex>
#include <SFML/Network.hpp>
#include "packet_types.hpp"

/* packet and type pair*/
using PTpair = std::pair<sf::Packet, PacketType>;

/* thread safe queue */
class ReceiverQueue {
public:
    ReceiverQueue(): queue_size(0) {}
    void enqueue(sf::Packet&& packet, PacketType ptype);
    PTpair dequeue();
    bool is_empty();
private:
    std::mutex queue_mutex;
    std::queue<PTpair> queue;
    std::atomic<int> queue_size;
};

bool is_valid_ip(const std::string& str);
bool is_valid_port(const std::string& str);

#endif