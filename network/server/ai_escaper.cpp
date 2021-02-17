#include "ai_escaper.hpp"

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
    auto coords = actual_pos;
    go(coords, direction, move_factor);
    review_time -= dt;
    if ((map.collision_checking(move_factor, coords, direction) != Collision::NONE) || (review_time <= 0.f)) {
        const std::vector<EntityDirection> directions = { EntityDirection::UP
                                                        , EntityDirection::DOWN
                                                        , EntityDirection::RIGHT
                                                        , EntityDirection::LEFT};
        std::vector<int> views;
        for (auto&& dir : directions) {
            views.push_back(map.view(actual_pos, dir));
        }
        auto first = map.get_rnd()();
        auto last = first + 4;
        for (;first < last; first++) {
            if (views[first%4] > 0) {
                direction = directions[first % 4];
                updated = true;
                break;
            }
        }
    } else {
        actual_pos = coords;
        updated = true;
    }
    if (review_time <= 0) {
        review_time = 1.f;
    }
}