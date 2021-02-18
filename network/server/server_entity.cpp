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

#define FASTER_MOVEMENT 1.4f

void ServerPlayerEntity::update(float dt) {
    if (dt >= spawn_protection) {
        spawn_protection = 0.f;
    } else {
        spawn_protection -= dt;
    }
    for (int s = 0;s < power_ups.size();) {
        power_ups.at(s).first -= sf::seconds(dt);
        if (power_ups.at(s).first.asSeconds() <= 0.f) {
            switch (power_ups.at(s).second) {
            case PowerUpType::FASTER:
                move_factor /= FASTER_MOVEMENT;
                break;
            case PowerUpType::BIGGER_BOMB:
                bomb_range -= 1;
                break;
            case PowerUpType::REFLECT:
                reflect = !reflect;
                break;
            }
            std::cout << "SERVER : power up " << std::to_string(static_cast<int>(power_ups.at(s).second)) << " erased from " << name << std::endl;
            power_ups.erase(power_ups.begin()+s);
        } else {
            s++;
        }
    }
}

void ServerPlayerEntity::apply_power_up(PowerUpType pu, const sf::Time& duration) {
    switch (pu) {
    case PowerUpType::FASTER:
        move_factor *= FASTER_MOVEMENT;
        break;
    case PowerUpType::BIGGER_BOMB:
        bomb_range += 1;
        break;
    case PowerUpType::REFLECT:
        reflect = !reflect;
        break;
    }
    power_ups.emplace_back(duration, pu);
    std::cout << "SERVER : power up " << std::to_string(static_cast<int>(pu)) << " added to " << name << std::endl;
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
        auto range = spe.bomb_range;
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
            result.emplace_back(ServerExplosionEntity(c, till_erasement, 0, d.at(s).first));
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