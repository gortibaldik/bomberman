#ifndef GAME_SERVER_ENTITY_HPP
#define GAME_SERVER_ENTITY_HPP

#include "game/entity.hpp"
#include "game/map_logic.hpp"
#include <SFML/Network.hpp>
#include <vector>
#include <deque>
#include <memory>
#include <atomic>
#include <iostream>

#define SPAWN_PROTECTION 2.f

class ServerPlayerEntity : public PlayerEntity {
public:
    sf::Int8 type;
    float spawn_protection = SPAWN_PROTECTION;
    float move_factor;
    bool updated;
    EntityCoords spawn_pos;
    int lives;
    bool reflect = false;
    std::deque<std::pair<sf::Time, PowerUpType>> power_ups;
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
                       , lives(lives)
                       , move_factor(move_factor)
                       , updated(false) {}
    virtual ~ServerPlayerEntity() = default;
    virtual void update(float dt);
    bool is_attackable() { return spawn_protection == 0.f; }
    virtual void respawn();
    virtual void update_pos_dir(EntityCoords&&, EntityDirection);
    virtual void apply_power_up(PowerUpType, const sf::Time&);
};

sf::Packet& operator <<(sf::Packet&, const ServerPlayerEntity&);

#endif