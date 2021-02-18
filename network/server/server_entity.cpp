#include "server_entity.hpp"
#include <iostream>

sf::Packet& operator <<(sf::Packet& packet, const ServerPlayerEntity& spe) {
    packet << spe.name;
    packet << sf::Int8(spe.direction);
    packet << sf::Int8(spe.lives);
    packet << spe.type;
    packet << spe.actual_pos.first;
    return packet << spe.actual_pos.second;
}

void ServerPlayerEntity::update_pos_dir(EntityCoords&& coords, EntityDirection dir) {
    actual_pos = std::move(coords);
    direction = dir;
}

void ServerPlayerEntity::update(float dt) {
    if (dt >= spawn_protection) {
        spawn_protection = 0.f;
    } else {
        spawn_protection -= dt;
    }
}

sf::Packet& operator <<(sf::Packet& packet, const ServerBombEntity& sbe) {
    packet << sf::Int32(sbe.ID);
    packet << sbe.actual_pos.first;
    return packet << sbe.actual_pos.second;
}

void ServerBombEntity::update(float dt) {
    if (dt >= time_to_explosion) {
        time_to_explosion = 0.f;
    } else {
        time_to_explosion = time_to_explosion - dt;
    }
}

bool ServerBombEntity::is_new() {
    if (n) {
        n = false;
        return true;
    }
    return false;
}

std::vector<ServerExplosionEntity> ServerBombEntity::explode(GameMapLogic& map, float till_erasement) {
    std::vector<ServerExplosionEntity> result;
    result.emplace_back(actual_pos, till_erasement, 0, ExplosionType::CENTER);
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
        bool fst = true;
        for (auto&& tc : d) {
            if (fst == true) {
                c = EntityCoords(actual_pos);
                go(c, tc.second, 1.f);
                fst = false;
            } else {
                go(c, tc.second, 1.f);
            }
            auto collision = map.collision_checking(0.f, c, tc.second);
            result.emplace_back(ServerExplosionEntity(c, till_erasement, 0, tc.first));
            if (collision != Collision::NONE) {
                break;
            }
        }
    }
    return result;
}

sf::Packet& operator <<(sf::Packet& packet, const ServerExplosionEntity& see) {
    packet << sf::Int32(see.ID);
    packet << sf::Int8(see.type);
    packet << see.actual_pos.first;
    return packet << see.actual_pos.second;
}

void ServerExplosionEntity::update(float dt) {
    if (dt >= till_erasement) {
        till_erasement = 0.f;
    } else {
        till_erasement = till_erasement - dt;
    }
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