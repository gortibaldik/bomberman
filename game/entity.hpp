#ifndef GAME_ENTITY_HPP
#define GAME_ENTITY_HPP

#include <utility>
#include <string>

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

using EntityCoords = std::pair<float, float>;

class Entity {
public:
    EntityCoords actual_pos;
    Entity() = default;
    Entity(EntityCoords actual_pos): actual_pos(actual_pos) {}
    
};

class PlayerEntity: public Entity {
public:
    PlayerEntity() : direction(EntityDirection::UP) {}
    PlayerEntity(const std::string& name
                , EntityCoords actual_pos
                , EntityDirection direction)
                : Entity(actual_pos)
                , name(name)
                , direction(direction) {}
    std::string name;
    EntityDirection direction;
};

#endif
