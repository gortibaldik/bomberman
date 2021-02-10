#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>
#include <unordered_map>
#include "entity.hpp"
#include <SFML/Graphics.hpp>
#include "texture_handling/texture_manager.hpp"

namespace TilesTypes {
    enum TilesTypes {
        WALKABLE,
        NON_WALKABLE
    };
};

using TilesMap = std::vector<AnimObject>;
using LogicMap = std::vector<TilesTypes::TilesTypes>;

class GameMap {
public:
    void load_from_config(const std::string&);
    virtual void process_loaded(const std::string& token, const std::string& animation,
                                const std::string& type, int row, int column) = 0;
    virtual void initialize() = 0;

    static void generate_config(const std::string&, int, int);
protected:
    int rows, columns;
};

class GameMapLogic: public GameMap {
public:
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    std::tuple<int, int, int> get_spawn_pos();
    void collision_checking(float move_factor, EntityCoords&, EntityDirection::EntityDirection);
private:
    TilesTypes::TilesTypes unsafe_get(int row, int column);
    std::vector<std::tuple<int, int, int>> spawn_positions;
    LogicMap tiles;
};

class GameMapRenderable: public GameMap{
public:
    GameMapRenderable(const TextureManager& tm);
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    void transform(AnimObject&, EntityCoords, bool scale = true);
    AnimObject& get(int row, int column);
    void render(sf::RenderTarget* rt);
    void fit_to_window(float, float);
private:
    const TextureManager& tm;
    AnimObject& unsafe_get(int row, int column);
    float tile_width, tile_height;
    float tile_scale_x, tile_scale_y;
    TilesMap tiles;
};

#endif