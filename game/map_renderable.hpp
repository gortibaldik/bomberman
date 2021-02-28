#ifndef GAME_MAP_RENDERABLE
#define GAME_MAP_RENDERABLE

#include "texture_handling/texture_manager.hpp"
#include "texture_handling/anim_object.hpp"
#include "map.hpp"
#include <unordered_map>

using GrassMap = std::unordered_map<int, AnimObject>;
using TilesMap = std::unordered_map<int, AnimObject>;

class GameMapRenderable: public GameMap {
public:
    GameMapRenderable(const TextureManager& tm);
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    void transform(AnimObject&, EntityCoords, bool scale = true) const;
    void render_grass(sf::RenderTarget* rt);
    void render_tiles(sf::RenderTarget* rt);
    void fit_to_window(float, float);
    void clear() {
        tiles.clear();
        grass.clear();
    }
private:
    const TextureManager& tm;
    AnimObject& unsafe_get(int row, int column);
    float tile_width, tile_height;
    float tile_scale_x, tile_scale_y;
    TilesMap tiles;
    GrassMap grass;
};

#endif
