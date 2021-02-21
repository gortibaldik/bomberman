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

void ServerPlayerEntity::respawn() {
    spawn_protection = SPAWN_PROTECTION;
    actual_pos = spawn_pos;
    direction = EntityDirection::UP;
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