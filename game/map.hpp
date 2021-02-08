#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>
#include <SFML/Graphics.hpp>
#include "texture_handling/texture_manager.hpp"
#include "entity.hpp"

namespace TilesTypes {
    enum TilesTypes {
        WALKABLE,
        NON_WALKABLE
    };
};

using TilesMap = std::vector<AnimObject>;
using LogicMap = std::vector<TilesTypes::TilesTypes>;
using Entities = std::vector<Entity>;

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
private:
    LogicMap tiles;
};

class GameMapRenderable: public GameMap{
public:
    GameMapRenderable(const TextureManager& tm);
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    AnimObject& get(int row, int column);
    void render(sf::RenderTarget* rt);
    void fit_to_window(float, float);
private:
    const TextureManager& tm;
    AnimObject& unsafe_get(int row, int column);
    float tile_width, tile_height;
    float tile_scale_x, tile_scale_y;
    TilesMap tiles;
    Entities entities;
};

#endif