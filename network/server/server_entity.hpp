#ifndef GAME_SERVER_ENTITY_HPP
#define GAME_SERVER_ENTITY_HPP

#include "game/entity.hpp"
#include "game/map_logic.hpp"
#include <SFML/Network.hpp>
#include <vector>
#include <memory>

class ServerPlayerEntity : public PlayerEntity {
public:
    sf::Int8 type;
    float spawn_protection = 2.f;
    float move_factor;
    bool updated;
    int c_deployed;
    EntityCoords spawn_pos;
    int lives;
    ServerPlayerEntity(const std::string& name
                       , EntityCoords spawn_pos
                       , EntityCoords actual_pos
                       , EntityDirection direction
                       , sf::Int8 type
                       , int lives
                       , float move_factor)
                       : PlayerEntity(name, actual_pos, direction)
                       , type(type)
                       , spawn_pos(spawn_pos)
                       , c_deployed(0)
                       , lives(lives)
                       , move_factor(move_factor)
                       , updated(false) {}
    void update(float dt);
    bool is_attackable() { return spawn_protection == 0.f; }
    void respawn() { spawn_protection = 1.f; }
    void update_pos_dir(EntityCoords&&, EntityDirection);
    bool can_deploy() { return c_deployed < 3; }
    void deploy() { c_deployed++; }
    void remove_deployed() { c_deployed = (c_deployed > 0) ? c_deployed - 1 : c_deployed; }
};

using PlayerPtr = std::unique_ptr<ServerPlayerEntity>;

class ServerExplosionEntity: public Entity {
public:
    int ID;
    float till_erasement;
    ExplosionType type;
    ServerExplosionEntity( EntityCoords pos
                         , float till_erasement
                         , int ID
                         , ExplosionType type)
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