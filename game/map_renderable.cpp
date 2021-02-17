#include "map_renderable.hpp"
#include "texture_handling/anim_object.hpp"
#include <stdexcept>

GameMapRenderable::GameMapRenderable(const TextureManager& tm)
                                    : tm(tm), tile_scale_x(1), tile_scale_y(1) {}

void GameMapRenderable::process_loaded(const std::string& token, const std::string& animation,
                                       const std::string& type, int row, int column) {
    auto square = tm.get_anim_object(animation);
    int i = row*columns + column;
    if (i == 0) {
        auto&& fr = square.get_global_bounds();
        tile_width = fr.width;
        tile_height = fr.height;
    }
    square.set_position(column*tile_width, row*tile_height);
    if (type.compare("NON_WALKABLE") == 0) {
        tiles.emplace(i, std::move(square));
    } else {
        grass.emplace(i, std::move(square));
    }
}

void GameMapRenderable::initialize() {
    tiles.clear();
}

void GameMapRenderable::render_tiles(sf::RenderTarget* rt) {
    for (auto&& tile : tiles) {
        rt->draw(tile.second.get_sprite());
    }
}

void GameMapRenderable::render_grass(sf::RenderTarget* rt) {
    for (auto&& g : grass) {
        rt->draw(g.second.get_sprite());
    }
}

void GameMapRenderable::transform(AnimObject& anim_object, EntityCoords pos, bool scale /*=true*/) const {
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
            unsafe_get(r, c).set_position(c*tile_width*tile_scale_x, r*tile_height*tile_scale_y);
            unsafe_get(r, c).scale(tile_scale_x, tile_scale_y);
        }
    }
}

AnimObject& GameMapRenderable::unsafe_get(int row, int column) {
    auto n = row*columns + column;
    auto it = grass.find(n);
    if (it == grass.end()) {
        return tiles.at(n);
    } else {
        return grass.at(n);
    }
}