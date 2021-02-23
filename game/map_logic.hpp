#ifndef GAME_MAP_LOGIC_HPP
#define GAME_MAP_LOGIC_HPP

#include "map.hpp"
#include "entity.hpp"
#include <vector>
#include <tuple>
#include <random>
#include <unordered_map>

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
using PowerUpMap = std::unordered_map<int, PowerUpType>;
using BombMap = std::unordered_multimap<int, BombEntity>;
using ExplosionMap = std::unordered_multimap<int, ExplosionEntity>;
using IDPos = std::pair<IDType, int>;
using IDPosVector = std::vector<IDPos>;
using IDPosTypeVector = std::vector<std::pair<IDPos, ExplosionType>>;
using IDTypeVector = std::vector<std::pair<IDType, int>>;
using IDVector = std::vector<IDType>;

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
    int erase_soft_block(int);

    Collision collision_checking(float move_factor, EntityCoords&, EntityDirection) const;
    void place_bomb(const EntityCoords&, PlayerEntity&);
    void place_bomb(int coords);
    PowerUpType is_on_power_up(const EntityCoords&, int& power_up_id);

    void update( float dt
               , IDPosVector& erased_bombs
               , IDPosVector& erased_explosions
               , IDPosVector& new_bombs
               , IDPosTypeVector& new_explosions);

    void update(float dt);
    bool check_damage(const EntityCoords&);
    IDTypeVector check_soft_blocks();
    void check_soft_blocks(const std::string&, int& count);
private:
    std::vector<std::tuple<int, int, int>> spawn_positions;
    LogicMap tiles;
    SoftBlockMap soft_blocks;
    PowerUpMap power_ups;
    BombMap bombs;
    ExplosionMap explosions;
    IDType general_ID;
    mutable RandomNumberBetween rnb;
    PlayerEntity dummy_entity;
};

#endif
