#ifndef GAME_MAP_LOGIC_HPP
#define GAME_MAP_LOGIC_HPP

#include "map.hpp"
#include "entity.hpp"
#include <vector>
#include <tuple>
#include <random>

namespace TilesTypes {
    enum TilesTypes {
        WALKABLE,
        NON_WALKABLE
    };
};

namespace Collision {
    enum Collision {
        SOFT_BLOCK,
        HARD_BLOCK,
        NONE
    };
};

using LogicMap = std::vector<TilesTypes::TilesTypes>;
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
    std::tuple<int, int, int> get_spawn_pos();
    const SoftBlockMap& get_soft_blocks() { return soft_blocks; }
    void erase_soft_block(int);
    Collision::Collision collision_checking(float move_factor, EntityCoords&, EntityDirection::EntityDirection);
    template<typename T>
    T unsafe_get(std::vector<T>& map, int row, int column) const {
        return map.at(row*columns + column);
    }
private:
    std::vector<std::tuple<int, int, int>> spawn_positions;
    LogicMap tiles;
    SoftBlockMap soft_blocks;
    RandomNumberBetween rnb;
};

#endif
