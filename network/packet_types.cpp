#include "packet_types.hpp"

void add_type_to_packet(sf::Packet& packet, PacketType type) {
    packet << (sf::Int8)type;
}