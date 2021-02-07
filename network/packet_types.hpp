#ifndef PACKET_TYPES_HPP__
#define PACKET_TYPES_HPP__

#include <SFML/Network.hpp>

enum class PacketType {
    Disconnect = -1,
    Connect,
    Duplicate,
    Message,
    HeartBeat,
    GetReady,
    ClientReady,
    Invalid
};

void add_type_to_packet(sf::Packet& packet, PacketType type);

#endif
