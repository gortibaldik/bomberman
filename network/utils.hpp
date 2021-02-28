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
    void change_first_remove_rest(const std::string&);
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

// https://stackoverflow.com/questions/5056645/sorting-stdmap-using-value
template<typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B> &p) {
    return {p.second, p.first};
}

// flips an associative container of A,B pairs to B,A pairs
template<typename A, typename B, template<class,class,class...> class M, class... Args>
std::multimap<B,A, std::greater_equal<B>> flip_map(const M<A,B,Args...> &src)
{
    std::multimap<B,A, std::greater_equal<B>> dst;
    std::transform(src.begin(), src.end(),
                   std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}

#endif