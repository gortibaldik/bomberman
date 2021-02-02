#include "def.hpp"
#include <iostream>
#include "network/packet_types.hpp"

bool Server::start(PortNumber port) {
    if (is_running) {
        return false;
    }
    if (incoming_socket.bind(port) != sf::Socket::Done) {
        return false;
    }
    outcoming_socket.bind(sf::Socket::AnyPort);
    std::cout << "Local IP address: " << sf::IpAddress::getLocalAddress() << std::endl;
    std::cout << "Incoming port: " << incoming_socket.getLocalPort() << std::endl;
    std::cout << "Outcoming port: " << outcoming_socket.getLocalPort() << std::endl;
    worker = std::thread(&Server::listen, this);
    is_running = true;
    return true;
}

void Server::listen() {
    sf::IpAddress ip;
    PortNumber port;
    sf::Packet packet;
    while (is_running) {
        packet.clear();
        auto status = incoming_socket.receive(packet, ip, port);
        if (status != sf::Socket::Done) { continue; }

        sf::Int8 id;
        if (!(packet >> id)) {
            continue;
        }

        PacketType pt = (PacketType)id;
        if (pt == PacketType::Connect) {
            std::cout << ip << " is trying to connect" << std::endl;
            std::string name;
            if (packet >> name) {
                std::cout << "name: " << name << std::endl;
            }
            sf::Packet p;
            p << sf::Int8(PacketType::Connect);
            outcoming_socket.send(p, ip, port);            
        }
    }
}

Server::~Server() {
    worker.join();
}