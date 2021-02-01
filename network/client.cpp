#include "client.hpp"
#include <string>
#include <iostream>

using boost::asio::ip::udp;

Client::Client(const std::string& ip_address_host,
               const std::string& server_port,
               unsigned short local_port)
               : socket(io_context, udp::endpoint(udp::v4(), local_port)) {
    udp::resolver resolver(io_context);
    udp::resolver::query query(udp::v4(), ip_address_host, server_port);
    server_endpoint = *resolver.resolve(query);
}

Client::~Client() {
    io_context.stop();
}

void Client::send(const std::array<char, 1>& data) {
    socket.send_to(boost::asio::buffer(data), server_endpoint);
}

size_t Client::receive(std::array<char, 128>& data) {
    size_t len = socket.receive_from(boost::asio::buffer(data), remote_endpoint);
    std::cout << len << std::endl;
    return len;
}