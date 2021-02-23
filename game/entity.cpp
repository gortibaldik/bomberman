#include "entity.hpp"
#include "map_logic.hpp"
#include <cmath>

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

EntityDirection opposite(EntityDirection direction) {
    switch (direction) {
    case EntityDirection::UP:
         return EntityDirection::DOWN;
    case EntityDirection::DOWN:
        return EntityDirection::UP;
    case EntityDirection::RIGHT:
        return EntityDirection::LEFT;
    case EntityDirection::LEFT:
        return EntityDirection::RIGHT;
    }
    return direction;
}

std::string to_string(EntityDirection direction) {
    switch (direction) {
    case EntityDirection::UP:
        return "EntityDirection::UP";
    case EntityDirection::DOWN:
         return "EntityDirection::DOWN";
    case EntityDirection::RIGHT:
        return "EntityDirection::RIGHT";
    case EntityDirection::LEFT:
        return "EntityDirection::LEFT";
    }
    return "";
}

/* Naive, because of the assumption, that both c1 and c2 are coordinates
 *  of top left corner of a square with a side of length 1
 * 
 * @return true if the squares intersect
 */ 
bool naive_bbox_intersect(const EntityCoords& c1, const EntityCoords& c2, float intersect_tolerance) {
    bool rows_intersect = ((c1.first >= c2.first) && (c1.first - 1.f + intersect_tolerance <= c2.first)) ||
                          ((c2.first >= c1.first) && (c2.first - 1.f + intersect_tolerance <= c1.first));
    bool cols_intersect = ((c1.second >= c2.second) && (c1.second - 1.f + intersect_tolerance <= c2.second)) ||
                          ((c2.second >= c1.second) && (c2.second - 1.f + intersect_tolerance <= c1.second));
    return rows_intersect && cols_intersect;
}

std::pair<int, int> to_integral(const EntityCoords& coords) {
    std::pair<int, int> result;
    float ceil_row = ceilf(coords.first);
    float ceil_col = ceilf(coords.second);
    if (ceil_row - coords.first <= 0.5f) {
        result.first = ceil_row;
    } else {
        result.first = ceil_row - 1;
    }
    if (ceil_col - coords.second <= 0.5f) {
        result.second = ceil_col;
    } else {
        result.second = ceil_col - 1;
    }
    return result;
}

void TimedEntity::update(float dt) {
    if (dt >= time_to_expire) {
        time_to_expire = 0.f;
    } else {
        time_to_expire -= dt;
    }
}

bool TimedEntity::is_new() {
    if (n) {
        n = false;
        return true;
    }
    return false;
}

std::vector<std::pair<int, ExplosionEntity>> BombEntity::explode(int c_pos
                                                                , IDType& id
                                                                , GameMapLogic& map
                                                                , float till_erasement) {
    EntityCoords actual_pos = map.transform_to_coords(c_pos);
    std::vector<std::pair<int, ExplosionEntity>> result;
    result.emplace_back(c_pos, ExplosionEntity( till_erasement
                                              , id++
                                              , ExplosionType::CENTER
                                              , player_entity));
    using type_dir = std::pair<ExplosionType, EntityDirection>;
    const std::vector<type_dir> to_right = {{ExplosionType::HORIZONTAL_RIGHT,   EntityDirection::RIGHT},
                                            {ExplosionType::RIGHT_END,          EntityDirection::RIGHT}};
    const std::vector<type_dir> to_left  = {{ExplosionType::HORIZONTAL_LEFT,    EntityDirection::LEFT},
                                            {ExplosionType::LEFT_END,           EntityDirection::LEFT}};
    const std::vector<type_dir> up       = {{ExplosionType::VERTICAL_UP,        EntityDirection::UP},
                                            {ExplosionType::UP_END,             EntityDirection::UP}};
    const std::vector<type_dir> down     = {{ExplosionType::VERTICAL_DOWN,      EntityDirection::DOWN},
                                            {ExplosionType::DOWN_END,           EntityDirection::DOWN}};
    const auto all_dirs = { to_left, to_right, up, down };
    for (auto&& d : all_dirs) {
        EntityCoords c;
        auto range = player_entity.bomb_range;
        bool fst = true;
        for (int s = 0; s < d.size();) {
            if (fst) {
                c = EntityCoords(actual_pos);
                go(c, d.at(s).second, 1.f);
                fst = false;
            } else {
                go(c, d.at(s).second, 1.f);
            }
            auto collision = map.collision_checking(0.f, c, d.at(s).second);

            result.emplace_back( map.transform_to_int(c)
                               , ExplosionEntity( till_erasement
                                                , id++
                                                , d.at(s).first
                                                , player_entity));
            if (collision != Collision::NONE) {
                break;
            }
            range--;
            if (range <= 1) {
                s++;
            }
        }
    }
    return result;
}