#include "ai_escaper.hpp"
#include <iostream>
#include <algorithm>

static const std::vector<EntityDirection> directions = { EntityDirection::UP
                                                       , EntityDirection::DOWN
                                                       , EntityDirection::RIGHT
                                                       , EntityDirection::LEFT};

std::vector<EntityDirection> AIEscaper::find_bomb_free_views(const std::vector<int>& map_free_views) { 
    std::vector<int> bomb_free_views(4);
    static const int view_distance = 4;
    for (auto&& dir : directions) {
        bomb_free_views[static_cast<int>(dir)] = bomb_manager.view( actual_pos, dir
                                                                  , std::min(map_free_views[static_cast<int>(dir)], view_distance));
    }
    auto first = map.get_rnd()();
    auto last = first + 4;
    std::vector<EntityDirection> possibilities;
    for (;first < last; first++) {
        if (bomb_free_views[first%4] > 0) {
            possibilities.push_back(directions[first%4]);
        }
    }
    return possibilities;
}

std::vector<EntityDirection> AIEscaper::find_player_free_views(const std::vector<int>& map_free_views) {
    std::vector<EntityDirection> possibilities;
    static const int view_distance = 2;
    static const float intersection_tolerance = 0.2f;
    for (auto&& dir : directions) {
        int result = 1;
        EntityCoords c = actual_pos;
        for (;;result++) {
            go(c, dir, 1.f);
            if (result > std::min(view_distance, map_free_views[static_cast<int>(dir)])) {
                break;
            }
            for (auto&& p : players) {
                if (p.first.compare(name) == 0) { continue; }
                if (naive_bbox_intersect(c, p.second->actual_pos, intersection_tolerance)) {
                    result *= -1;
                    break;
                }
            }
            if (result < 0) {
                break;
            }
        }
        if (result > 0) {
            possibilities.push_back(dir);
        }
    }
    return possibilities;
}

template<typename T>
bool is_in(T value, std::vector<T> vct) {
    for (auto&& t : vct) {
        if (t == value) {
            return  true;
        }
    }
    return false;
}

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
    auto coords = actual_pos;
    go(coords, direction, move_factor);
    review_time -= dt;
    std::vector<int> map_free_views(4);
    for (auto&& dir : directions) {
        map_free_views[static_cast<int>(dir)] = map.view(actual_pos, dir);
    }
    auto bomb_free_views = find_bomb_free_views(map_free_views);
    auto player_free_views = find_player_free_views(map_free_views);
    bool has_to_change = false;
    if (bomb_free_views.size() == 0) {
        std::cout << "AIEscaper : nowhere to run, just going to die probably..." << std::endl;
        bomb_free_views = directions;
    } else {
        has_to_change = !is_in(direction, bomb_free_views) || !is_in(direction, player_free_views);
    }
    if ((review_time <= 0.f) || has_to_change || (map.collision_checking(move_factor, coords, direction) != Collision::NONE)) {
        std::vector<int> views = {-1,-1,-1,-1};
        for (auto&& dir : bomb_free_views) {
            views[static_cast<int>(dir)] = map.view(actual_pos, dir);
        }
        auto first = map.get_rnd()();
        auto last = first + 4;
        EntityDirection possible_direction = EntityDirection::STATIC;
        for (;first < last; first++) {
            if (views[first % 4] > 0) {
                if ((possible_direction == EntityDirection::STATIC) ||
                    is_in(static_cast<EntityDirection>(first % 4), player_free_views)) {
                    possible_direction = static_cast<EntityDirection>(first % 4);
                }
                updated = true;
            }
        }
        direction = possible_direction;
        if (review_time <= 0.f) {
            review_time = 5.f;
        }
    } else {
        actual_pos = coords;
        updated = true;
    }
}