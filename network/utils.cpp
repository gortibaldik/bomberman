#include <algorithm>
#include <sstream>
#include <string>
#include <stdexcept>
#include "utils.hpp"

bool is_valid_ip(const std::string& str) {
    size_t cnt = std::count(str.begin(), str.end(), '.');
    if (cnt == 3) {
        std::string quad;
        std::stringstream ss(str);
        cnt = 0;
        while(std::getline(ss,quad, '.')) {
            if ((quad.size() == 0) || (std::stoi(quad) >= 256)) {
                return false;
            }
            cnt++;
        }
        return cnt == 4;
    }
    return false;
}

static int is_valid_port_static(const std::string& str) {
    if (str.size() == 0) { return -1; }
    for (auto&& c : str) {
        if (!std::isdigit(c)) {
            return -1;
        }
    }
    return std::stoi(str);
}

bool is_valid_port_nz(const std::string& str) {
    return is_valid_port_static(str) > 1024;
}

bool is_valid_port(const std::string& str) {
    auto i = is_valid_port_static(str);
    return (i > 1024) || (i == 0);
}

void ReceiverQueue::enqueue(sf::Packet&& packet, PacketType ptype) {
    std::unique_lock<std::mutex> l(queue_mutex);
    queue_size++;
    queue.emplace(std::move(packet), ptype);
}

PTpair ReceiverQueue::dequeue() {
    std::unique_lock<std::mutex> l(queue_mutex);
    if (queue_size == 0) {
        throw std::runtime_error("Invalid op on queue -> not enough elements to dequeue");
    }
    queue_size--;
    auto tmp = std::move(queue.front());
    queue.pop();
    return tmp;
}

bool ReceiverQueue::is_empty() {
    std::unique_lock<std::mutex> l(queue_mutex);
    return queue_size == 0;
}