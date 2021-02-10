#include "client_entity.hpp"
#include <unordered_map>
#include <iostream>

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

sf::Packet& operator >>(sf::Packet& packet, ClientBombEntity& cbe) {
    packet >> cbe.ID;
    packet >> cbe.actual_pos.first;
    return packet >> cbe.actual_pos.second;
}

ClientExplosionEntity ClientExplosionEntity::extract_from_packet(const TextureManager& tm, sf::Packet& packet) {
    const std::unordered_map<ExplosionType::ExplosionType, std::string> exp_to_str = {
        {ExplosionType::CENTER, "exp_center"},
        {ExplosionType::HORIZONTAL, "exp_horizontal"},
        {ExplosionType::VERTICAL, "exp_vertical"},
        {ExplosionType::RIGHT_END, "exp_right"},
        {ExplosionType::LEFT_END, "exp_left"},
        {ExplosionType::UP_END, "exp_up"},
        {ExplosionType::DOWN_END, "exp_down"}
    };
    sf::Int32 id = 0;
    sf::Int8 type = 0;
    float f = 0.f, s = 0.f;
    packet >> id >> type >> f >> s;
    auto cee = ClientExplosionEntity(tm, exp_to_str.at(static_cast<ExplosionType::ExplosionType>(type)), id);
    cee.actual_pos = EntityCoords(f, s);
    return cee;
}