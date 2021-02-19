#include "ai_escaper.hpp"
#include <iostream>
#include <algorithm>

static const std::vector<EntityDirection> directions = { EntityDirection::UP
                                                       , EntityDirection::DOWN
                                                       , EntityDirection::RIGHT
                                                       , EntityDirection::LEFT};

template<typename T>
bool is_in(T value, std::vector<T> vct) {
    for (auto&& t : vct) {
        if (t == value) {
            return  true;
        }
    }
    return false;
}

void AIEscaper::update_loop() {}

void AIEscaper::notify(sf::Packet& packet) {}

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
}