#include "map.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

GameMapRenderable::GameMapRenderable(const TextureManager& tm)
                                    : tm(tm) {}

static void throw_error() {
    throw std::runtime_error("Invalid map config file");
}

void GameMap::load_from_config(const std::string& config_file) {
    std::ifstream ifs(config_file);
    if (!ifs) {
        throw std::runtime_error("Unable to load config file: " + config_file);
    }
    std::string token, animation, type, line;
    if (! std::getline(ifs, line)) {
        throw_error();
    }
    std::stringstream ss(line);
    if (!(ss >> token) || (token.compare("MAP") != 0)) {
        throw_error();
    }
    int rows, columns;
    if (! std::getline(ifs, line)) {
        throw_error();
    }
    ss = std::stringstream(line);
    if (! (ss >> rows >> columns ) || (rows < 3 ) || (columns < 3)) {
        throw_error();
    }
    this->rows = rows;
    this->columns = columns;
    initialize();
    for (int i = 0; i < rows * columns; i++) {
        if (!std::getline(ifs, line)) {
            throw_error();
        }
        ss = std::stringstream(line);
        int r, c;
        if (! (ss >> token >> animation >> type >> r >> c) ||
            (r < 0) || (r >= rows) || (c < 0) || (c >= columns) ||
            (r*columns + c != i)) {
            throw_error();
        }
        process_loaded(token, animation, type, r, c);
    }
}

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

void GameMapLogic::process_loaded(const std::string& token, const std::string& animation,
                                       const std::string& type, int row, int column) {
    TilesTypes::TilesTypes t;
    if (type.compare("NON_WALKABLE") == 0) {
        t = TilesTypes::NON_WALKABLE;
    } else if (type.compare("WALKABLE") == 0) {
        t = TilesTypes::WALKABLE;
    } else {
        throw std::runtime_error("Couldn't load tile type!");
    }
    tiles.push_back(t);
}

void GameMapLogic::initialize() {
    tiles.clear();
}

void GameMap::generate_config(const std::string& file_name, int rows, int columns) {
    std::ofstream ofs(file_name);
    ofs << "MAP" << std::endl;
    ofs << "15 15" << std::endl;
    int idx = 0;
    for (int r = 0; r < rows; r++) {
        bool must_row = (r == 0) || (r == rows-1);
        bool row_wall = (r % 2 == 0);
        for (int c = 0; c < columns; c++) {
            bool must_column = (c == 0) || (c == columns -1);
            bool column_wall = (c % 2 == 0);
            bool wall_2d = (r == (rows-1)) || ((c != 0) && (c!= columns -1));
            ofs << "TILE ";
            if ((row_wall && column_wall && (c != columns -2) && (r != rows - 2)) || must_row || must_column) {
                if (wall_2d) {
                    ofs << "wall_front NON_WALKABLE ";
                } else {
                    ofs << "wall_side NON_WALKABLE ";
                }
            } else {
                ofs << "grass WALKABLE ";
            }
            ofs << r << " " << c << std::endl;
        }
    }
}

void GameMapRenderable::render(sf::RenderTarget* rt) {
    for (auto&& tile : tiles) {
        rt->draw(tile.get_sprite());
    }
}

void GameMapRenderable::fit_to_window(float x, float y) {
    std::cout << x << "," << y << std::endl;
    float height = rows * tile_height;
    float width = columns * tile_width;
    std::cout << width << "," << height << std::endl;
    float transform_width = x / width;
    float transform_height = y / height;
    std::cout << "scaling factors: " << transform_width << "," << transform_height << std::endl;
    float min = (transform_width < transform_height) ? transform_width : transform_height;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            get(r, c).set_position(c*tile_width*transform_width, r*tile_height*transform_height);
            get(r, c).scale(transform_width, transform_height);
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