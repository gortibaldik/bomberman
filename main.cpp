#include <iostream>
#include <boost/asio.hpp>


std::string get_address() {
    using boost::asio::ip::udp;
    boost::asio::io_service netService;
    udp::resolver   resolver(netService);
    udp::resolver::query query(udp::v4(), "google.com", "");
    udp::resolver::iterator endpoints = resolver.resolve(query);
    udp::endpoint ep = *endpoints;
    udp::socket socket(netService);
    socket.connect(ep);
    boost::asio::ip::address addr = socket.local_endpoint().address();
    return addr.to_string();
}

int main() {
    std::cout << get_address() << std::endl;
}