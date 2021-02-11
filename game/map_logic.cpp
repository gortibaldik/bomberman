#include "map_logic.hpp"
#include <stdexcept>
#include <iostream>

std::tuple<int, int, int> GameMapLogic::get_spawn_pos() {
    auto tpl = std::move(spawn_positions.back());
    spawn_positions.pop_back();
    return tpl;
}

// probability of soft box is PROB_SOFT / 10.f
#define PROB_SOFT 6

void GameMapLogic::process_loaded(const std::string& token, const std::string& animation,
                                       const std::string& type, int row, int column) {
    TilesTypes::TilesTypes t;
    bool is_soft_block = false;
    if (type.compare("NON_WALKABLE") == 0) {
        t = TilesTypes::NON_WALKABLE;
    } else if (type.compare("FREE") == 0) {
        t = TilesTypes::WALKABLE;
    } else if (type.compare("POSSIBLY_WALKABLE") == 0) {
        auto x = rnb();
        if (x <= 5) {
            is_soft_block = true;
        }
        t = TilesTypes::WALKABLE;
    } else if (type.compare("SPAWN") == 0) {
        t = TilesTypes::WALKABLE;
        spawn_positions.emplace_back(row, column, spawn_positions.size());
    } else {
        throw std::runtime_error("Couldn't load tile type! : " + type);
    }
    tiles.push_back(t);
    soft_blocks.push_back(is_soft_block);
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

#define TOLERANCE 0.5f

static bool try_correct(int free, float& me) {
    float val = free - me;
    if (val < 0.f) {
        val = me - free;
    }
    if (val <= TOLERANCE) {
        me = free;
        return true;
    }
    return false;
}


/* 
 * @param direction direction where the entity is going
 * @param coords coordinates of the checked entity, the coords can change a little bit if 
 *                  the entity can move in that direction within tolerance
 * @param move_factor how much do the entity need to go back after unsuccessful check
 * @return true if collision can be avoided
 */
template <typename T>
static bool col_checking(const GameMapLogic& map, std::vector<T> to_check, T value, float move_factor, EntityCoords& coords, EntityDirection::EntityDirection dir) {
    int ceil_row = static_cast<int>(ceilf(coords.first));
    int ceil_col = static_cast<int>(ceilf(coords.second));
    int flr_row = static_cast<int>(floorf(coords.first));
    int flr_col = static_cast<int>(floorf(coords.second));
    bool check_f = false, check_c = false;
    float mid_row = (ceil_row + flr_row) / 2.f;
    float mid_col = (ceil_col + flr_col) / 2.f;
    switch(dir) {
    case EntityDirection::UP:
        check_c = map.unsafe_get<T>(to_check, flr_row, ceil_col) == value;
        check_f = map.unsafe_get<T>(to_check, flr_row, flr_col) == value;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::DOWN:
        check_c = map.unsafe_get<T>(to_check, ceil_row, ceil_col) == value;
        check_f = map.unsafe_get<T>(to_check, ceil_row, flr_col) == value;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::LEFT:
        check_c = map.unsafe_get<T>(to_check, ceil_row, flr_col) == value;
        check_f = map.unsafe_get<T>(to_check, flr_row, flr_col) == value;
        if (!check_c && check_f && try_correct(ceil_row, coords.first)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_row, coords.first)) {
            return true;
        }
        break;
    case EntityDirection::RIGHT:
        check_c = map.unsafe_get<T>(to_check, ceil_row, ceil_col) == value;
        check_f = map.unsafe_get<T>(to_check, flr_row, ceil_col) == value;
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

Collision::Collision GameMapLogic::collision_checking(float move_factor, EntityCoords& coords, EntityDirection::EntityDirection dir) {
    EntityCoords hard_check(coords), soft_check(coords);
    bool hard = !col_checking<TilesTypes::TilesTypes>(*this, tiles, TilesTypes::NON_WALKABLE, move_factor, hard_check, dir);
    bool soft = !col_checking<bool>(*this, soft_blocks, true, move_factor, soft_check, dir);
    coords = hard_check;
    if (hard) {
        return Collision::HARD_BLOCK;
    }
    if (soft) {
        coords = soft_check;
        return Collision::SOFT_BLOCK;
    }
    return Collision::NONE;
}

void GameMapLogic::erase_soft_block(int i) {
    if ((i < 0) || (i >= soft_blocks.size())) {
        throw std::runtime_error("invalid index! - soft_blocks");
    }
    soft_blocks[i] = false;
}

void GameMapLogic::initialize() {
    tiles.clear();
    soft_blocks.clear();
    spawn_positions.clear();
}

GameMapLogic::GameMapLogic(): rnb(1, 10) {}