#ifndef GAME_ENTITY_HPP
#define GAME_ENTITY_HPP

#include <utility>
#include <string>

namespace EntityDirection {
    enum EntityDirection {
        UP,
        DOWN,
        RIGHT,
        LEFT
    };
};

using EntityCoords = std::pair<int, int>;

class Entity {
public:
    EntityCoords actual_pos;
    Entity() = default;
    Entity(EntityCoords actual_pos): actual_pos(actual_pos) {}
    
};

class PlayerEntity: public Entity {
public:
    PlayerEntity() = default;
    PlayerEntity(const std::string& name
                , EntityCoords actual_pos
                , EntityDirection::EntityDirection direction)
                : Entity(actual_pos)
                , name(name)
                , direction(direction) {}
    std::string name;
    EntityDirection::EntityDirection direction;
};

class BombEntity {

};

class BlockEntity {

};

#endif
