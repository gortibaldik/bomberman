#ifndef GAME_SERVER_ENTITY_HPP
#define GAME_SERVER_ENTITY_HPP

#include "game/entity.hpp"
#include "game/map.hpp"
#include <SFML/Network.hpp>
#include <vector>

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

class ServerExplosionEntity: public Entity {
public:
    int ID;
    float till_erasement;
    ExplosionType::ExplosionType type;
    ServerExplosionEntity( EntityCoords pos
                         , float till_erasement
                         , int ID
                         , ExplosionType::ExplosionType type)
                         : Entity(pos)
                         , ID(ID)
                         , till_erasement(till_erasement)
                         , type(type) {}
    void update(float dt);
    bool can_be_erased() { return till_erasement <= 0.f; }
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
    bool can_explode() { return time_to_explosion <= 0.f; }
    bool is_new();
    std::vector<ServerExplosionEntity> explode(GameMapLogic&, float till_erasement);
};

bool naive_bbox_intersect(const EntityCoords& c1, const EntityCoords& c2);

sf::Packet& operator <<(sf::Packet&, const ServerExplosionEntity&);
sf::Packet& operator <<(sf::Packet&, const ServerBombEntity&);
sf::Packet& operator <<(sf::Packet&, const ServerPlayerEntity&);

#endif