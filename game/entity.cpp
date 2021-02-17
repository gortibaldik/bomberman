#include "entity.hpp"

void go(EntityCoords& coords, EntityDirection direction, float move_factor) {
    switch (direction) {
    case EntityDirection::UP:
        coords.first -= move_factor;
        break;
    case EntityDirection::DOWN:
        coords.first += move_factor;
        break;
    case EntityDirection::RIGHT:
        coords.second += move_factor;
        break;
    case EntityDirection::LEFT:
        coords.second -= move_factor;
        break;
    }
}