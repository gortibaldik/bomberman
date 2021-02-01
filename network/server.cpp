#include "server.hpp"
#include <array>
#include <ctime>
#include <iostream>

using boost::asio::ip::udp;

Server::Server(const std::string& local_port)
              : socket(io_context, udp::endpoint(udp::v4(), std::stoi(local_port))) {}

Server::~Server() {
    io_context.stop();
}

static std::string make_daytime_string() {
    std::time_t now = std::time(0);
    return std::ctime(&now);
}

void Server::loop() {
    for (;;) {
        std::array<char, 1> recv_buf;
        socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
        std::string message = make_daytime_string();
        std::cout << "received message from " << remote_endpoint.address().to_string() << std::endl;

        boost::system::error_code ignored;
        socket.send_to(boost::asio::buffer(message),
                       remote_endpoint,
                       0,
                       ignored);
    }
}