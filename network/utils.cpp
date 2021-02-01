#include "utils.hpp"
#include <boost/asio.hpp>
#include <SFML/Network.hpp>

bool is_valid_ip(const std::string& ip) {
    boost::system::error_code ec;
    boost::asio::ip::address::from_string(ip, ec);
    if (ec) {
        return false;
    }
    return true;
}

std::string get_local_ip() {
    return sf::IpAddress::getLocalAddress().toString();
}

std::string get_public_ip() {
    return sf::IpAddress::getPublicAddress().toString();
}

bool is_valid_port_number(unsigned int port) {
    return (port >= 1024) && (port <= 50000);
}