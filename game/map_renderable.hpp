#ifndef GAME_MAP_RENDERABLE
#define GAME_MAP_RENDERABLE

#include "texture_handling/texture_manager.hpp"
#include "map.hpp"

using TilesMap = std::vector<AnimObject>;

class GameMapRenderable: public GameMap {
public:
    GameMapRenderable(const TextureManager& tm);
    void process_loaded(const std::string& token, const std::string& animation,
                        const std::string& type, int row, int column) override;
    void initialize() override;
    void transform(AnimObject&, EntityCoords, bool scale = true) const;
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
