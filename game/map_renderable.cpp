#include "map_renderable.hpp"
#include "texture_handling/anim_object.hpp"
#include <stdexcept>

GameMapRenderable::GameMapRenderable(const TextureManager& tm)
                                    : tm(tm), tile_scale_x(1), tile_scale_y(1) {}

void GameMapRenderable::process_loaded(const std::string& token, const std::string& animation,
                                       const std::string& type, int row, int column) {
    tiles.push_back(tm.get_anim_object(animation));
    int i = row*columns + column;
    if (i == 0) {
        auto&& fr = tiles[i].get_global_bounds();
        tile_width = fr.width;
        tile_height = fr.height;
    }
    tiles[i].set_position(column*tile_width, row*tile_height);
}

void GameMapRenderable::initialize() {
    tiles.clear();
}

void GameMapRenderable::render(sf::RenderTarget* rt) {
    for (auto&& tile : tiles) {
        rt->draw(tile.get_sprite());
    }
}

void GameMapRenderable::transform(AnimObject& anim_object, EntityCoords pos, bool scale /*=true*/) {
    float tsx = scale ? tile_scale_x : 1.f;
    float tsy = scale ? tile_scale_y : 1.f;
    anim_object.set_position(pos.second*tile_width*tile_scale_x, pos.first*tile_height*tile_scale_y);
    anim_object.scale(tsx, tsy);
}

void GameMapRenderable::fit_to_window(float x, float y) {
    float height = rows * tile_height;
    float width = columns * tile_width;
    tile_scale_x = x / width;
    tile_scale_y = y / height;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            get(r, c).set_position(c*tile_width*tile_scale_x, r*tile_height*tile_scale_y);
            get(r, c).scale(tile_scale_x, tile_scale_y);
        }
    }
}

AnimObject& GameMapRenderable::get(int row, int column) {
    if ((row >= rows) || (column >= columns)) {
        throw std::runtime_error("Invalid map coordintates!");
    }
    return unsafe_get(row, column);
}

AnimObject& GameMapRenderable::unsafe_get(int row, int column) {
    return tiles[row*columns + column];
}