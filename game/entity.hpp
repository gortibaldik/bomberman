#ifndef GAME_ENTITY_HPP
#define GAME_ENTITY_HPP

#include <utility>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

enum class EntityDirection : int {
    UP = 0,
    DOWN = 1,
    RIGHT = 2,
    LEFT = 3,
    STATIC
};

enum class ExplosionType : int {
    CENTER,
    HORIZONTAL_RIGHT,
    HORIZONTAL_LEFT,
    VERTICAL_UP,
    VERTICAL_DOWN,
    UP_END,
    DOWN_END,
    RIGHT_END,
    LEFT_END
};

enum class PowerUpType : int {
    NONE = 0,
    FASTER = 1,
    BIGGER_BOMB = 2,
    REFLECT = 3,
};

using EntityCoords = std::pair<float, float>;
using IDType = int;

class Entity {
public:
    EntityCoords actual_pos;
    Entity() = default;
    virtual ~Entity() = default;
    Entity(EntityCoords actual_pos): actual_pos(actual_pos) {}
    
};

void go(EntityCoords&, EntityDirection, float move_factor);
EntityDirection opposite(EntityDirection dir);
std::string to_string(EntityDirection dir);

class PlayerEntity: public Entity {
public:
    PlayerEntity() : direction(EntityDirection::UP) {}
    PlayerEntity(const std::string& name
                , EntityCoords actual_pos
                , EntityDirection direction)
                : Entity(actual_pos)
                , name(name)
                , direction(direction)
                , bomb_range(2)
                , c_deployed(0) {}
    std::string name;
    EntityDirection direction;
    int bomb_range;
    int c_deployed;
    bool can_deploy() { return c_deployed < 3; }
    void deploy() { c_deployed++; }
    void remove_deployed() { c_deployed = (c_deployed > 0)
                                            ? c_deployed - 1
                                            : c_deployed; }
};
using PlayerPtr = std::unique_ptr<PlayerEntity>;
using Players = std::unordered_map<std::string, PlayerPtr>;

class TimedEntity {
public:
    TimedEntity( float time_to_expire
               , IDType ID)
               : time_to_expire(time_to_expire)
               , ID(ID)
               , n(true) /* is the bomb new? */ {}
    void update(float dt);
    bool is_expired() { return time_to_expire <= 0.f; }
    bool is_new();
    IDType get_id() { return ID; }
private:
    float time_to_expire;
    IDType ID;
    bool n;
};

class ExplosionEntity: public TimedEntity {
public:
    ExplosionEntity( float time_to_expire
                   , IDType ID
                   , ExplosionType etype
                   , PlayerEntity& player_entity)
                   : TimedEntity(time_to_expire, ID)
                   , etype(etype)
                   , player_entity(player_entity) {}
    ExplosionType get_type() { return etype; }
    PlayerEntity& player_entity;
private:
    ExplosionType etype;
};

class GameMapLogic;
class BombEntity: public TimedEntity {
public:
    BombEntity( float time_to_explosion
              , IDType ID
              , PlayerEntity& player_entity)
              : TimedEntity(time_to_explosion, ID)
              , player_entity(player_entity) {}
    PlayerEntity& player_entity;
    std::vector<std::pair<int, ExplosionEntity>> explode(int c_pos
                                                        , IDType& id
                                                        , GameMapLogic&
                                                        , float till_erasement);
};

bool naive_bbox_intersect(const EntityCoords& c1, const EntityCoords& c2, float intersection_tolerance);
std::pair<int, int> to_integral(const EntityCoords&);

#endif
