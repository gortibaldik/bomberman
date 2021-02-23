#include "map_logic.hpp"
#include <stdexcept>
#include <iostream>
#include <limits>

std::tuple<int, int, int> GameMapLogic::get_spawn_pos() {
    auto tpl = std::move(spawn_positions.back());
    spawn_positions.pop_back();
    return tpl;
}

// probability of soft box is PROB_SOFT / 12.f
#define PROB_SOFT 8

void GameMapLogic::process_loaded( const std::string& token
                                 , const std::string& animation
                                 , const std::string& type
                                 , int row
                                 , int column) {
    TilesTypes t;
    bool is_soft_block = false;

    if (type.compare("NON_WALKABLE") == 0) {
        t = TilesTypes::NON_WALKABLE;
    } else if (type.compare("FREE") == 0) {
        t = TilesTypes::WALKABLE;
    } else if (type.compare("POSSIBLY_WALKABLE") == 0) {
        auto x = rnb();
        if (x <= PROB_SOFT) {
            is_soft_block = true;
            switch (x) {
            case 1:
                power_ups.emplace(tiles.size(), PowerUpType::FASTER);
                break;
            case 2:
                power_ups.emplace(tiles.size(), PowerUpType::BIGGER_BOMB);
                break;
            case 3:
                power_ups.emplace(tiles.size(), PowerUpType::REFLECT);
                break;
            }
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

static void go_back(float move_factor, EntityCoords& coords, EntityDirection dir) {
    switch(dir) {
    case EntityDirection::UP:
        go(coords, EntityDirection::DOWN, move_factor);
        break;
    case EntityDirection::DOWN:
        go(coords, EntityDirection::UP, move_factor);
        break;
    case EntityDirection::LEFT:
        go(coords, EntityDirection::RIGHT, move_factor);
        break;
    case EntityDirection::RIGHT:
        go(coords, EntityDirection::LEFT, move_factor);
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
        me = static_cast<float>(free);
        return true;
    }
    return false;
}

template<typename T>
static T unsafe_get(std::vector<T>& map, int row, int column, int columns) {
    return map.at(static_cast<long>(row)*columns + column);
}


/* 
 * @param direction direction where the entity is going
 * @param coords coordinates of the checked entity, the coords can change a little bit if 
 *                  the entity can move in that direction within tolerance
 * @param move_factor how much do the entity need to go back after unsuccessful check
 * @return true if collision can be avoided
 */
template <typename T>
static bool col_checking(const GameMapLogic& map, std::vector<T> to_check, T value, float move_factor, EntityCoords& coords, EntityDirection dir) {
    int ceil_row = static_cast<int>(ceilf(coords.first));
    int ceil_col = static_cast<int>(ceilf(coords.second));
    int flr_row = static_cast<int>(floorf(coords.first));
    int flr_col = static_cast<int>(floorf(coords.second));
    bool check_f = false, check_c = false;
    if ( (flr_row < 0) || (flr_col < 0) || (ceil_row >= map.get_rows()) || (ceil_col >= map.get_columns())) {
        throw std::runtime_error("Invalid index to map: (" + std::to_string(coords.first) + "," + std::to_string(coords.second) + ")");
    }
    switch(dir) {
    case EntityDirection::UP:
        check_c = unsafe_get<T>(to_check, flr_row, ceil_col, map.get_columns()) == value;
        check_f = unsafe_get<T>(to_check, flr_row, flr_col, map.get_columns()) == value;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::DOWN:
        check_c = unsafe_get<T>(to_check, ceil_row, ceil_col, map.get_columns()) == value;
        check_f = unsafe_get<T>(to_check, ceil_row, flr_col, map.get_columns()) == value;
        if (!check_c && check_f && try_correct(ceil_col, coords.second)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_col, coords.second)) {
            return true;
        }
        break;
    case EntityDirection::LEFT:
        check_c = unsafe_get<T>(to_check, ceil_row, flr_col, map.get_columns()) == value;
        check_f = unsafe_get<T>(to_check, flr_row, flr_col, map.get_columns()) == value;
        if (!check_c && check_f && try_correct(ceil_row, coords.first)) {
            return true;
        } else if (check_c && !check_f && try_correct(flr_row, coords.first)) {
            return true;
        }
        break;
    case EntityDirection::RIGHT:
        check_c = unsafe_get<T>(to_check, ceil_row, ceil_col, map.get_columns()) == value;
        check_f = unsafe_get<T>(to_check, flr_row, ceil_col, map.get_columns()) == value;
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

Collision GameMapLogic::collision_checking( float move_factor
                                          , EntityCoords& coords
                                          , EntityDirection dir) const {
    EntityCoords hard_check(coords), soft_check(coords);
    bool hard = !col_checking(*this, tiles, TilesTypes::NON_WALKABLE, move_factor, hard_check, dir);
    bool soft = !col_checking(*this, soft_blocks, true, move_factor, soft_check, dir);
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

#define EXPLOSION_TIME 1.5f
#define VIEW_EXPLOSION_TIME 0.5f

void GameMapLogic::place_bomb(int coords, float time) {
    bombs.emplace(coords, BombEntity( time
                                    , general_ID++
                                    , dummy_entity));
}

void GameMapLogic::place_bomb(const EntityCoords& coords, PlayerEntity& entity) {
    auto new_coords = to_integral(coords);
    bombs.emplace(transform_to_int(new_coords), BombEntity( EXPLOSION_TIME
                                                          , general_ID++
                                                          , entity));
}

void GameMapLogic::update(float dt
                         , IDPosVector& erased_bombs
                         , IDPosVector& erased_explosions
                         , IDPosVector& new_bombs
                         , IDPosTypeVector& new_explosions) {
    std::vector<BombMap::const_iterator> bombs_to_remove;
    std::vector<ExplosionMap::const_iterator> explosions_to_remove;
    for ( auto&& b = bombs.begin(); b != bombs.end(); b++) {
        auto&& bomb = b->second;
        auto coords = b->first;
        bomb.update(dt);
        if (bomb.is_new()) {
            new_bombs.emplace_back(bomb.get_id(), coords);
        }
        if (bomb.is_expired()) {
            bomb.player_entity.remove_deployed();
            erased_bombs.emplace_back(bomb.get_id(), coords);
            bombs_to_remove.emplace_back(b);
            for (auto&& exp : bomb.explode( coords
                                          , general_ID
                                          , *this
                                          , VIEW_EXPLOSION_TIME)) {
                new_explosions.emplace_back(IDPos(exp.second.get_id(), exp.first)
                                           , exp.second.get_type());
                explosions.emplace(std::move(exp));
            }
        }
    }
    for (auto&& e = explosions.begin(); e != explosions.end(); e++) {
        e->second.update(dt);
        if (e->second.is_expired()) {
            erased_explosions.emplace_back(e->second.get_id(), e->first);
            explosions_to_remove.emplace_back(e);
        }
    }
    for (auto&& i : erased_explosions) {
        explosions.erase(i.second);
    }
    for (auto&& i : bombs_to_remove) {
        bombs.erase(i);
    }
}

void GameMapLogic::update(float dt) {
    IDPosVector erased_bombs, erased_explosions;
    for (auto&& b : bombs) {
        b.second.update(dt);
        if (b.second.is_expired()) {
            erased_bombs.emplace_back(b.second.get_id(), b.first);
            for (auto&& exp : b.second.explode( b.first
                                          , general_ID
                                          , *this
                                          , VIEW_EXPLOSION_TIME)) {
                explosions.emplace(std::move(exp));
            }
        }
    }
    for (auto&& e : explosions) {
        e.second.update(dt);
        if (e.second.is_expired()) {
            erased_explosions.emplace_back(e.second.get_id(), e.first);
        }
    }
    for (auto&& i : erased_explosions) {
        explosions.erase(i.second);
    }
    for (auto&& i : erased_bombs) {
        bombs.erase(i.second);
    }
}

bool GameMapLogic::check_damage( const EntityCoords& pos) {
    static const float intersection_tolerance = 0.3f;
    for (auto&& exp : explosions) {
        auto exp_coords = transform_to_coords(exp.first);
        if (naive_bbox_intersect( pos
                                , exp_coords
                                , intersection_tolerance)) {
            return true;
        }
    }
    return false;
}

IDTypeVector GameMapLogic::check_soft_blocks() {
    IDTypeVector erased_soft_blocks;
    for ( auto&& exp : explosions) {
        auto exp_coords = transform_to_coords(exp.first);
        int i = -1;
        for (auto&& exists : soft_blocks) {
            i++;
            if (!exists) { continue; }
            auto soft_block_coords = transform_to_coords(i);
            if (soft_block_coords == exp_coords) {
                auto type = erase_soft_block(i);
                erased_soft_blocks.emplace_back(i, type);
            }
        }
    }
    return erased_soft_blocks;
}

void GameMapLogic::check_soft_blocks(const std::string& pe_name, int& count) {
    count = 0;
    for ( auto&& exp : explosions) {
        auto exp_coords = transform_to_coords(exp.first);
        int i = -1;
        for (auto&& exists : soft_blocks) {
            i++;
            if (!exists) { continue; }
            auto soft_block_coords = transform_to_coords(i);
            if (soft_block_coords == exp_coords) {
                auto type = erase_soft_block(i);
                if (exp.second.player_entity.name.compare(pe_name) == 0) {
                    count++;
                }
            }
        }
    }
}

/* Check if coords are the same as power up coords
 * if yes erase the power up from the map
 * @return PowerUpType which is at the position of PowerUpType::NONE in the respective case
 */
PowerUpType GameMapLogic::is_on_power_up(const EntityCoords& coords, int& power_up_id) {
    static const float tolerance = 0.4f;
    for (auto&& pu : power_ups) {
        auto pu_coords = transform_to_coords(pu.first);
        if (naive_bbox_intersect(coords, pu_coords, tolerance)) {
            auto power_up = pu.second;
            power_up_id = pu.first;
            power_ups.erase(pu.first);
            return power_up;
        }
    }
    return PowerUpType::NONE;
}

int GameMapLogic::erase_soft_block(int i) {
    if ((i < 0) || (i >= soft_blocks.size())) {
        throw std::runtime_error("invalid index! - soft_blocks");
    }
    soft_blocks[i] = false;
    if (power_ups.find(i) != power_ups.end()) {
        auto pu = static_cast<int>(power_ups.at(i));
        return pu;
    }
    return 0;
}

void GameMapLogic::initialize() {
    tiles.clear();
    soft_blocks.clear();
    spawn_positions.clear();
}

GameMapLogic::GameMapLogic(): rnb(1, 12)
                            , general_ID(std::numeric_limits<int>::min())
                            , dummy_entity("", EntityCoords(), EntityDirection::UP) {}