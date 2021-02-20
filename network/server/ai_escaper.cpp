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

void AIEscaper::update_loop() {
    while (is_running) {}
}

void AIEscaper::BFS() {
    
}

void AIEscaper::notify_new_bomb(const IDPos& idp) {
    map.place_bomb(idp.second);
}

void AIEscaper::notify_sb_destroyed(int i) {
    map.erase_soft_block(i);
}

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
}