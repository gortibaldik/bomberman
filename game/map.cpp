#include "map.hpp"
#include "entity.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>


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
    name = config_file;
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
                ofs << "grass FREE ";
            }
            ofs << r << " " << c << std::endl;
        }
    }
}

std::pair<int, int> GameMap::transform_to_coords(int i) const {
    std::pair<int, int> result;
    result.first = i / columns;
    result.second = i % columns;
    return result;
}

int GameMap::transform_to_int(const std::pair<int, int>& coords) const {
    return coords.first * columns + coords.second;
}