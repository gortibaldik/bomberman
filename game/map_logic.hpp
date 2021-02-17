#ifndef GAME_MAP_LOGIC_HPP
#define GAME_MAP_LOGIC_HPP

#include "map.hpp"
#include "entity.hpp"
#include <vector>
#include <tuple>
#include <random>

enum class TilesTypes {
    WALKABLE,
    NON_WALKABLE
};

enum class Collision {
    SOFT_BLOCK,
    HARD_BLOCK,
    NONE
};

using LogicMap = std::vector<TilesTypes>;
using SoftBlockMap = std::vector<bool>;

// code from https://www.fluentcpp.com/2019/05/24/how-to-fill-a-cpp-collection-with-random-values/
class RandomNumberBetween {
public:
    RandomNumberBetween(int low, int high)
    : random_engine_{std::random_device{}()}
    , distribution_{low, high}{}
    int operator()() {
        return distribution_(random_engine_);
    }
private:
    std::mt19937 random_engine_;
    std::uniform_int_distribution<int> distribution_;
};

class GameMapLogic: public GameMap {
public:
    GameMapLogic();
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    size_t get_max_players() { return spawn_positions.size(); }
    std::tuple<int, int, int> get_spawn_pos();
    RandomNumberBetween& get_rnd() const { return rnb; }
    
    const SoftBlockMap& get_soft_blocks() { return soft_blocks; }
    void erase_soft_block(int);

    Collision collision_checking(float move_factor, EntityCoords&, EntityDirection) const;
    int view(const EntityCoords&, EntityDirection) const;
private:
    std::vector<std::tuple<int, int, int>> spawn_positions;
    LogicMap tiles;
    SoftBlockMap soft_blocks;
    mutable RandomNumberBetween rnb;
};

#endif
