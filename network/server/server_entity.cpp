#include "server_entity.hpp"

sf::Packet& operator <<(sf::Packet& packet, const ServerPlayerEntity& spe) {
    packet << spe.name;
    packet << sf::Int8(spe.direction);
    packet << spe.type;
    packet << spe.actual_pos.first;
    return packet << spe.actual_pos.second;
}