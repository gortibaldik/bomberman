#include "client_entity.hpp"

sf::Packet& operator >>(sf::Packet& packet, ClientPlayerEntity& cpe) {
    packet >> cpe.name;
    sf::Int8 token;
    packet >> token;
    cpe.direction = (EntityDirection::EntityDirection)token;
    token = 0;
    packet >> token;
    cpe.anim_object = cpe.tm.get_anim_object("p1");
    packet >> cpe.actual_pos.first;
    packet >> cpe.actual_pos.second;
    return packet;
}