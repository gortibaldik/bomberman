#include "texture_loader.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

enum class CONFIG_TYPES {
    TEXTURE,
    FONT,
    ANIMATION
};

enum class ANIMATION_TYPES {
    STATIC,
    DIRECTIONAL
};

static std::string config_file_error = "Invalid config file";

static std::string append_line_n(const std::string& str, int line) {
    return str + " at line : " + std::to_string(line);
}

static bool is_digit(const std::string& token) {
    for (auto&& c : token) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

static void load_anim_states( TextureManager& tm
                            , std::istream& is
                            , const std::string& animation_name
                            , int duration
                            , Direction direction
                            , int& line_n) {
    std::string line;
    while (duration > 0) {
        int size_x, size_y, x, y;
        std::string token;
        line_n++;
        std::getline(is, line);
        std::stringstream ss(line);
        if (!(ss >> token)) {
            continue; // skip blank lines
        }
        if (!(ss >> size_y >> x >> y) || !is_digit(token)) {
            throw std::runtime_error(append_line_n("Invalid tokens", line_n));
        }
        size_x = std::stoi(token);
        tm.add_animation_state(animation_name, size_x, size_y, x, y, direction);
        duration--;
    }
}

static void load_static_animation(TextureManager& tm
                                 , std::istream& is
                                 , const std::string& animation_name
                                 , int& line_n) {
    int duration, milliseconds_frame_change;
    std::string line;
    line_n++;
    if (!std::getline(is, line)) {
        throw std::runtime_error(append_line_n("Expecting more lines (STATIC ANIMATION)", line_n));
    }
    std::stringstream _ss(line);
    if (!(_ss >> duration >> milliseconds_frame_change)) {
        throw std::runtime_error(append_line_n("Invalid tokens", line_n));
    }
    tm.add_animation_frame_rate(animation_name, milliseconds_frame_change, Direction::STATIC);
    load_anim_states(tm, is, animation_name, duration, Direction::STATIC, line_n);
}

static void load_directional_animation(TextureManager& tm
                                      , std::istream& is
                                      , const std::string& animation_name
                                      , int& line_n) {
    std::map<std::string, Direction> all_dirs = { {"UP", Direction::UP },
                                                  {"DOWN", Direction::DOWN},
                                                  {"SIDE", Direction::SIDE} };
    std::string line, token;
    int duration, milliseconds_frame_change;
    // the configuration file is expected to contain all the
    // directions that the animation could possibly go to
    while (all_dirs.size() != 0) {
        line_n++;
        if (!std::getline(is, line)) {
            throw std::runtime_error(append_line_n("DIRECTIONAL ANIMATION SPEC: Expecting more lines!", line_n));
        }
        std::stringstream _ss(line);
        if (!(_ss >> token)) {
            continue; // skip empty lines
        }
        if (!(_ss >> duration >> milliseconds_frame_change)) {
            throw std::runtime_error(append_line_n("Invalid tokens!", line_n));
        }
        if (all_dirs.find(token) == all_dirs.end()) {
            throw std::runtime_error(append_line_n("Unknown direction token " + token, line_n));
        }
        tm.add_animation_frame_rate(animation_name, milliseconds_frame_change, all_dirs.at(token));
        load_anim_states(tm, is, animation_name, duration, all_dirs.at(token), line_n);
        all_dirs.erase(token);
    }
}

static void load_animation(TextureManager& tm, std::istream& is, int& line_n) {
    std::string animation_name, texture_name, animation_type;
    std::string line;
    line_n++;
    if (!std::getline(is, line)) {
        throw std::runtime_error(append_line_n("During loading animation - expecting more lines!", line_n));
    }
    std::stringstream ss(line);
    while (! (ss >> animation_name)) {
        line_n++;
        if (!std::getline(is, line)) {
            throw std::runtime_error(append_line_n("During loading animation - expecting more lines!", line_n));
        }
        ss = std::stringstream(line);
    }
    if (!(ss >> texture_name >> animation_type)) {
        throw std::runtime_error(append_line_n("Invalid tokens!", line_n));
    }
    const std::unordered_map<std::string, ANIMATION_TYPES> animation_types = { { "STATIC", ANIMATION_TYPES::STATIC },
                                                                              { "DIRECTIONAL", ANIMATION_TYPES::DIRECTIONAL } };
    if (animation_types.find(animation_type) == animation_types.end()) {
        throw std::runtime_error(append_line_n("Invalid animation type: " + animation_type, line_n));
    }
    switch(animation_types.at(animation_type)) {
    case ANIMATION_TYPES::STATIC:
        tm.create_animation(animation_name, texture_name, Direction::STATIC);
        load_static_animation(tm, is, animation_name, line_n);
        break;
    case ANIMATION_TYPES::DIRECTIONAL:
        tm.create_animation(animation_name, texture_name, Direction::UP);
        load_directional_animation(tm, is, animation_name, line_n);
        break;
    }
}

static void load_texture(TextureManager& tm, std::istream& is) {
    std::string file_name, texture_name;
    if (!(is >> file_name >> texture_name) || !tm.load_texture(texture_name, file_name)) {
        throw std::runtime_error("Couldn't load texture: " + texture_name);
    }
}

static void load_font(TextureManager& tm, std::istream& is) {
    std::string file_name, font_name;
    if (!(is >> file_name >> font_name) || !tm.load_font(font_name, file_name)) {
        throw std::runtime_error("Couldn't load font: " + font_name);
    }
}

TextureLoader::TextureLoader(const std::string& name_of_config_file)
                            : ifs(name_of_config_file) {
    if (!ifs) {
        throw std::runtime_error("Couldn't load texture config file! (" + name_of_config_file + ")");
    }
}

void TextureLoader::load(TextureManager& texture_manager) {
    int line_n = 0;
    std::string line;
    const std::unordered_map<std::string, CONFIG_TYPES> cfg_type = { {"TEXTURE", CONFIG_TYPES::TEXTURE},
                                                                     {"FONT", CONFIG_TYPES::FONT},
                                                                     {"ANIMATION", CONFIG_TYPES::ANIMATION}};
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        line_n++;
        std::string token;
        if (!(ss >> token)) {
            continue; // ignore blank lines
        }
        if (cfg_type.find(token) == cfg_type.end()) {
            throw std::runtime_error("Invalid token type: " + token);
        }
        switch(cfg_type.at(token)) {
            case CONFIG_TYPES::TEXTURE:
                load_texture(texture_manager, ss);
                break;
            case CONFIG_TYPES::FONT:
                load_font(texture_manager, ss);
                break;
            case CONFIG_TYPES::ANIMATION:
                load_animation(texture_manager, ifs, line_n);
                break;
            default:
                throw std::runtime_error("Invalid entity specification token!");
                break;
        }
    }
    std::cout << "Loaded config file" << std::endl;
}