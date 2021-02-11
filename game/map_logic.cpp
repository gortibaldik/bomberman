#include "map_logic.hpp"
#include <stdexcept>
#include <cmath>

std::tuple<int, int, int> GameMapLogic::get_spawn_pos() {
    auto tpl = std::move(spawn_positions.back());
    spawn_positions.pop_back();
    return tpl;
}

void GameMapLogic::process_loaded(const std::string& token, const std::string& animation,
                                       const std::string& type, int row, int column) {
    TilesTypes::TilesTypes t;
    if (type.compare("NON_WALKABLE") == 0) {
        t = TilesTypes::NON_WALKABLE;
    } else if (type.compare("WALKABLE") == 0) {
        t = TilesTypes::WALKABLE;
    } else if (type.compare("SPAWN") == 0) {
        t = TilesTypes::WALKABLE;
        spawn_positions.emplace_back(row, column, spawn_positions.size());
    } else {
        throw std::runtime_error("Couldn't load tile type!");
    }
    tiles.push_back(t);
}

TilesTypes::TilesTypes GameMapLogic::unsafe_get(int row, int column) {
    return tiles[row*columns + column];
}

static void go_back(float move_factor, EntityCoords& coords, EntityDirection::EntityDirection dir) {
    switch(dir) {
    case EntityDirection::UP:
        coords.first += move_factor;
        break;
    case EntityDirection::DOWN:
        coords.first -= move_factor;
        break;
    case EntityDirection::LEFT:
        coords.second += move_factor;
        break;
    case EntityDirection::RIGHT:
        coords.second -= move_factor;
        break;
    }
}

#define TOLERANCE 0.35f

static bool try_correct(int free, float& me) {
    auto val = abs(me - free);
    if (val <= TOLERANCE) {
        me = free;
        return true;
    }
    return false;
}

bool GameMapLogic::collision_checking(float move_factor, EntityCoords& coords, EntityDirection::EntityDirection dir) {
    int ceil_row = static_cast<int>(ceilf(coords.first));
    int ceil_col = static_cast<int>(ceilf(coords.second));
    int flr_row = static_cast<int>(floorf(coords.first));
    int flr_col = static_cast<int>(floorf(coords.second));
    bool check_f = false, check_c = false;
    float mid_row = (ceil_row + flr_row) / 2.f;
    float mid_col = (ceil_col + flr_col) / 2.f;
    switch(dir) {
    case EntityDirection::UP:
        check_c = unsafe_get(flr_row, ceil_col) == TilesTypes::NON_WALKABLE;
        check_f = unsafe_get(flr_row, flr_col) == TilesTypes::NON_WALKABLE;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::DOWN:
        check_c = unsafe_get(ceil_row, ceil_col) == TilesTypes::NON_WALKABLE;
        check_f = unsafe_get(ceil_row, flr_col) == TilesTypes::NON_WALKABLE;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::LEFT:
        check_c = unsafe_get(ceil_row, flr_col) == TilesTypes::NON_WALKABLE;
        check_f = unsafe_get(flr_row, flr_col) == TilesTypes::NON_WALKABLE;
        if (!check_c && check_f && try_correct(ceil_row, coords.first)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_row, coords.first)) {
            return true;
        }
        break;
    case EntityDirection::RIGHT:
        check_c = unsafe_get(ceil_row, ceil_col) == TilesTypes::NON_WALKABLE;
        check_f = unsafe_get(flr_row, ceil_col) == TilesTypes::NON_WALKABLE;
        if (!check_c && check_f && try_correct(ceil_row, coords.first)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_row, coords.first)) {
            return true;
        }
        break;
    }
    if (check_c || check_f) {
        go_back(move_factor, coords, dir);
        return false;
    }
    return true;
}

void GameMapLogic::initialize() {
    tiles.clear();
}
