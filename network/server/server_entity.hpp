#ifndef GAME_SERVER_ENTITY_HPP
#define GAME_SERVER_ENTITY_HPP

#include "game/entity.hpp"
#include <SFML/Network.hpp>

class ServerPlayerEntity : public PlayerEntity {
public:
    sf::Int8 type;
    bool updated;
    int c_deployed;
    EntityCoords spawn_pos;
    ServerPlayerEntity(const std::string& name
                       , EntityCoords spawn_pos
                       , EntityCoords actual_pos
                       , EntityDirection::EntityDirection direction
                       , sf::Int8 type)
                       : PlayerEntity(name, actual_pos, direction)
                       , type(type)
                       , spawn_pos(spawn_pos)
                       , c_deployed(0) {}
    void update_pos_dir(EntityCoords&&, EntityDirection::EntityDirection);
    bool can_deploy() { return c_deployed < 3; }
    void deploy() { c_deployed++; }
    void remove_deployed() { c_deployed = (c_deployed > 0) ? c_deployed - 1 : c_deployed; }
};

class ServerBombEntity: public Entity {
public:
    int ID;
    float time_to_explosion;
    bool n; /* is the bomb new ? */
    ServerPlayerEntity& spe;
    ServerBombEntity( EntityCoords center_pos
                    , float time_to_explosion
                    , int ID
                    , ServerPlayerEntity& spe)
                    : Entity(center_pos)
                    , time_to_explosion(time_to_explosion)
                    , n(true)
                    , ID(ID)
                    , spe(spe) {}
    void update(float dt);
    bool is_exploded() { return time_to_explosion <= 0.f; }
    bool is_new();
};

sf::Packet& operator <<(sf::Packet&, const ServerBombEntity&);
sf::Packet& operator <<(sf::Packet&, const ServerPlayerEntity&);

#endif