#ifndef NETWORK_UTILS_HPP
#define NETWORK_UTILS_HPP

#include <queue>
#include <utility>
#include <atomic>
#include <mutex>
#include <SFML/Network.hpp>
#include "packet_types.hpp"

/* thread safe queue */
class ReceiverQueue {
public:
    ReceiverQueue(): queue_size(0) {}
    void enqueue(const std::string&);
    std::string dequeue();
    bool is_empty();
private:
    std::mutex queue_mutex;
    std::queue<std::string> queue;
    std::atomic<int> queue_size;
};

bool is_valid_ip(const std::string& str);
bool is_valid_port_nz(const std::string& str);
bool is_valid_port(const std::string& str);

#endif