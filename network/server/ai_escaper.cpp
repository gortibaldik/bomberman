#include "ai_escaper.hpp"
#include <iostream>

static const std::vector<EntityDirection> directions = { EntityDirection::UP
                                                       , EntityDirection::DOWN
                                                       , EntityDirection::RIGHT
                                                       , EntityDirection::LEFT};

std::vector<EntityDirection> AIEscaper::find_bomb_free_views() { 
    std::vector<int> views(4);
    for (auto&& dir : directions) {
        views[static_cast<int>(dir)] = bomb_manager.view(actual_pos, dir);
    }
    auto first = map.get_rnd()();
    auto last = first + 4;
    std::vector<EntityDirection> possibilities;
    for (;first < last; first++) {
        if (views[first%4] > 0) {
            possibilities.push_back(directions[first%4]);
        }
    }
    return possibilities;
}

std::vector<EntityDirection> AIEscaper::find_player_free_views() {
    std::vector<int> views(4);
}

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
    auto coords = actual_pos;
    go(coords, direction, move_factor);
    review_time -= dt;
    auto bomb_free_views = find_bomb_free_views();
    bool has_to_change = false;
    if (bomb_free_views.size() == 0) {
        std::cout << "AIEscaper : nowhere to run, just going to die probably..." << std::endl;
        bomb_free_views = directions;
    } else {
        has_to_change = true;
        for (auto&& dir : bomb_free_views) {
            if (dir == direction) {
                has_to_change = false;
                break;
            }
        }
    }
    if (has_to_change || (map.collision_checking(move_factor, coords, direction) != Collision::NONE)) {
        std::vector<int> views = {-1,-1,-1,-1};
        for (auto&& dir : bomb_free_views) {
            views[static_cast<int>(dir)] = map.view(actual_pos, dir);
        }
        auto first = map.get_rnd()();
        auto last = first + 4;
        for (;first < last; first++) {
            if (views[first % 4] > 0) {
                direction = static_cast<EntityDirection>(first % 4);
                updated = true;
                break;
            }
        }
    } else {
        actual_pos = coords;
        updated = true;
    }
}