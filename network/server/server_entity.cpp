#include "server_entity.hpp"

sf::Packet& operator <<(sf::Packet& packet, const ServerPlayerEntity& spe) {
    packet << spe.name;
    packet << sf::Int8(spe.direction);
    packet << spe.type;
    packet << spe.actual_pos.first;
    return packet << spe.actual_pos.second;
}

void ServerPlayerEntity::update_pos_dir(EntityCoords&& coords, EntityDirection::EntityDirection dir) {
    actual_pos = std::move(coords);
    direction = dir;
}

sf::Packet& operator <<(sf::Packet& packet, const ServerBombEntity& sbe) {
    packet << sf::Int32(sbe.ID);
    packet << sbe.actual_pos.first;
    return packet << sbe.actual_pos.second;
}

void ServerBombEntity::update(float dt) {
    if (dt >= time_to_explosion) {
        time_to_explosion = 0.f;
    } else {
        time_to_explosion = time_to_explosion - dt;
    }
}

bool ServerBombEntity::is_new() {
    if (n) {
        n = false;
        return true;
    }
    return false;
}