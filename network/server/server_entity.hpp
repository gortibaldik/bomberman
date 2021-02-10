#ifndef GAME_SERVER_ENTITY_HPP
#define GAME_SERVER_ENTITY_HPP

#include "game/entity.hpp"
#include <SFML/Network.hpp>

class ServerPlayerEntity : public PlayerEntity {
public:
    sf::Int8 type;
    bool updated;
    EntityCoords spawn_pos;
    ServerPlayerEntity(const std::string& name
                       , EntityCoords spawn_pos
                       , EntityCoords actual_pos
                       , EntityDirection::EntityDirection direction
                       , sf::Int8 type)
                       : PlayerEntity(name, actual_pos, direction)
                       , type(type)
                       , spawn_pos(spawn_pos) {}
    void update_pos_dir(EntityCoords&&, EntityDirection::EntityDirection);
};

sf::Packet& operator <<(sf::Packet&, const ServerPlayerEntity& spe);

#endif