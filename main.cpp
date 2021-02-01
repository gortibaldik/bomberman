#include "network/utils.hpp"
#include "network/client.hpp"
#include "network/server.hpp"
#include <iostream>
#include <array>

int main(int argc, char** argv) {
    if (argc == 2) {
        if (!is_valid_port_number(std::stoi(argv[1]))) {
            std::cerr << "Not valid port number!" << std::endl;
            return 2;
        }
        std::cout << "Starting server on port " << argv[1] << std::endl;
        std::cout << "Your IP address is " << get_local_ip() << std::endl;
        std::cout << "Your public IP address is " << get_public_ip() << std::endl;
        Server server(argv[1]);
        server.loop();
    } else if (argc == 3) {
        if (!is_valid_port_number(std::stoi(argv[2]))) {
            std::cerr << "Not valid port number!" << std::endl;
            return 1;
        }
        if (!is_valid_ip(argv[1])) {
            std::cerr << "Not valid IP address!" << std::endl;
            return 2;
        }
        Client client(argv[1], argv[2]);
        std::array<char, 1> bf;
        while (true) {
            client.send(bf);
        }
        std::array<char, 128> rec;
        size_t len = client.receive(rec);
        std::cout << "Received: " << std::string(std::begin(rec), std::begin(rec)+len) << std::endl;
    }
}