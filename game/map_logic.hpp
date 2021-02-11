#ifndef GAME_MAP_LOGIC_HPP
#define GAME_MAP_LOGIC_HPP

#include "map.hpp"
#include "entity.hpp"
#include <vector>
#include <tuple>

namespace TilesTypes {
    enum TilesTypes {
        WALKABLE,
        NON_WALKABLE
    };
};
using LogicMap = std::vector<TilesTypes::TilesTypes>;

class GameMapLogic: public GameMap {
public:
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    std::tuple<int, int, int> get_spawn_pos();
    bool collision_checking(float move_factor, EntityCoords&, EntityDirection::EntityDirection);
private:
    TilesTypes::TilesTypes unsafe_get(int row, int column);
    std::vector<std::tuple<int, int, int>> spawn_positions;
    LogicMap tiles;
};

#endif
