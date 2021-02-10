#include "server_entity.hpp"
#include <iostream>

sf::Packet& operator <<(sf::Packet& packet, const ServerPlayerEntity& spe) {
    packet << spe.name;
    packet << sf::Int8(spe.direction);
    packet << spe.type;
    packet << spe.actual_pos.first;
    return packet << spe.actual_pos.second;
}

void ServerPlayerEntity::update_pos_dir(EntityCoords&& coords, EntityDirection::EntityDirection dir) {
    actual_pos = std::move(coords);
    direction = dir;
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
    using type_coords = std::pair<std::pair<ExplosionType::ExplosionType, EntityDirection::EntityDirection>, EntityCoords>;
    const std::vector<type_coords> to_right = {{{ExplosionType::HORIZONTAL, EntityDirection::RIGHT},    EntityCoords(0,  1)},
                                               {{ExplosionType::RIGHT_END,   EntityDirection::RIGHT},    EntityCoords(0,  1)}};
    const std::vector<type_coords> to_left  = {{{ExplosionType::HORIZONTAL,  EntityDirection::LEFT},     EntityCoords(0, -1)},
                                               {{ExplosionType::LEFT_END,    EntityDirection::LEFT},     EntityCoords(0, -1)}};
    const std::vector<type_coords> up       = {{{ExplosionType::VERTICAL,    EntityDirection::UP},       EntityCoords(-1, 0)},
                                               {{ExplosionType::UP_END,      EntityDirection::UP},       EntityCoords(-1, 0)}};
    const std::vector<type_coords> down     = {{{ExplosionType::VERTICAL,    EntityDirection::DOWN},     EntityCoords(1,  0)},
                                               {{ExplosionType::DOWN_END,    EntityDirection::DOWN},     EntityCoords(1,  0)}};
    const auto all_dirs = { to_left, to_right, up, down };
    for (auto&& d : all_dirs) {
        EntityCoords c;
        bool fst = true;
        for (auto&& tc : d) {
            if (fst == true) {
                c.first = tc.second.first + actual_pos.first;
                c.second = tc.second.second + actual_pos.second;
                fst = false;
            } else {
                c.first += tc.second.first;
                c.second += tc.second.second;
            }
            if (map.collision_checking(0.f, c, tc.first.second)) {
                result.emplace_back(ServerExplosionEntity(c, till_erasement, 0, tc.first.first));
            } else {
                break;
            }
        }
    }
    std::cout << "exploded to " << result.size() << " size" << std::endl;
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
bool naive_bbox_intersect(const EntityCoords& c1, const EntityCoords& c2) {
    bool rows_intersect = ((c1.first >= c2.first) && (c1.first - 1.f <= c2.first)) ||
                          ((c2.first >= c1.first) && (c2.first - 1.f <= c1.first));
    bool cols_intersect = ((c1.second >= c2.second) && (c1.second - 1.f <= c2.second)) ||
                          ((c2.second >= c1.second) && (c2.second - 1.f <= c1.second));
    return rows_intersect && cols_intersect;
}