#include "def.hpp"

#include <SFML/System.hpp>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>

void WindowManager::push_state(GSPtr&& state) {
    states.push_back(std::move(state));
}

void WindowManager::pop_states(int n) {
    while (n > 1) {
        if (states.empty()) {
            throw std::runtime_error("Couldn't pop as many states!");
        } else {
            states.erase(states.end()-2);
            n--;
        }
    }
    remove_top = true;
}

void WindowManager::change_state(GSPtr&& state) {
    if (!states.empty()) {
        pop_states(1);
        states.insert(states.end()-1, std::move(state));
    } else {
        push_state(std::move(state));
    }
}

const GSPtr& WindowManager::peek_state() {
    if (states.empty()) {
        return null_placeholder;
    } else {
        return states.back();
    }
}

void WindowManager::loop() {
    sf::Clock clock;
    while(window.isOpen()) {
        if (remove_top) {
            states.pop_back();
            remove_top = false;
        }
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        auto&& state_ptr = peek_state();
        if (!state_ptr) {
            continue;
        }
        
        state_ptr->handle_input();
        state_ptr->update(dt);
        window.clear(sf::Color::Black);
        state_ptr->draw(dt);
        window.display();
    }
}

enum CONFIG_TYPES {
    TEXTURE,
    FONT,
    ANIMATION
};

enum ANIMATION_TYPES {
    STATIC,
    DIRECTIONAL
};

static std::string config_file_error = "Invalid config file";

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
                            , Direction direction ) {
    std::string line;
    while (duration > 0) {
        int size_x, size_y, x, y;
        std::string token;
        std::getline(is, line);
        std::stringstream ss(line);
        if (!(ss >> token)) {
            continue; // skip blank lines
        }
        if (!(ss >> size_y >> x >> y) || !is_digit(token)) {
            throw std::runtime_error(config_file_error);
        }
        size_x = std::stoi(token);
        tm.add_animation_state(animation_name, size_x, size_y, x, y, direction);
        duration--;
    }
}

static void load_static_animation(TextureManager& tm, std::istream& is, const std::string& animation_name) {
    int duration, milliseconds_frame_change;
    std::string line;
    if (!std::getline(is, line)) {
        throw std::runtime_error(config_file_error);
    }
    std::stringstream _ss(line);
    if (!(_ss >> duration >> milliseconds_frame_change)) {
        throw std::runtime_error(config_file_error);
    }
    tm.add_animation_frame_rate(animation_name, milliseconds_frame_change, Direction::STATIC);
    load_anim_states(tm, is, animation_name, duration, Direction::STATIC);
}

static void load_directional_animation(TextureManager& tm, std::istream& is, const std::string& animation_name) {
    std::map<std::string, Direction> all_dirs = { {"UP", Direction::UP },
                                                  {"DOWN", Direction::DOWN},
                                                  {"SIDE", Direction::SIDE} };
    std::string line, token;
    int duration, milliseconds_frame_change;
    while (all_dirs.size() != 0) {
        if (!std::getline(is, line)) {
            throw std::runtime_error(config_file_error);
        }
        std::stringstream _ss(line);
        if (!(_ss >> token)) {
            continue; // skip empty lines
        }
        if (!(_ss >> duration >> milliseconds_frame_change)) {
            throw std::runtime_error(config_file_error);
        }
        if (all_dirs.find(token) == all_dirs.end()) {
            throw std::runtime_error(config_file_error);
        }
        tm.add_animation_frame_rate(animation_name, milliseconds_frame_change, all_dirs.at(token));
        load_anim_states(tm, is, animation_name, duration, all_dirs.at(token));
        all_dirs.erase(token);
    }
}

static void load_animation(TextureManager& tm, std::istream& is) {
    std::string animation_name, texture_name, animation_type;
    std::string line;
    if (!std::getline(is, line)) {
        throw std::runtime_error(config_file_error);
    }
    std::stringstream ss(line);
    while (! (ss >> animation_name)) {
        if (!std::getline(is, line)) {
            throw std::runtime_error(config_file_error);
        }
        ss = std::stringstream(line);
    }
    if (!(ss >> texture_name >> animation_type)) {
        throw std::runtime_error(config_file_error);
    }
    const std::unordered_map<std::string, ANIMATION_TYPES> animation_types = { { "STATIC", STATIC },
                                                                              { "DIRECTIONAL", DIRECTIONAL } };
    if (animation_types.find(animation_type) == animation_types.end()) {
        throw std::runtime_error(config_file_error);
    }
    switch(animation_types.at(animation_type)) {
    case STATIC:
        tm.create_animation(animation_name, texture_name, Direction::STATIC);
        load_static_animation(tm, is, animation_name);
        break;
    case DIRECTIONAL:
        tm.create_animation(animation_name, texture_name, Direction::UP);
        load_directional_animation(tm, is, animation_name);
        break;
    }
}

static void load_texture(TextureManager& tm, std::istream& is) {
    std::string file_name, texture_name;
    if (!(is >> file_name >> texture_name) || !tm.load_texture(texture_name, file_name)) {
        throw std::runtime_error(config_file_error);
    }
}

static void load_font(Fonts& fonts, std::istream& is) {
    std::string file_name, font_name;
    if (!(is >> file_name >> font_name) || !fonts[font_name].loadFromFile(file_name)) {
        throw std::runtime_error(config_file_error);
    }
}

void WindowManager::load_from_config(const std::string& name_of_file) {
    std::fstream ifs(name_of_file);
    if (!ifs) {
        throw std::runtime_error("Couldn't load media config file!");
    }

    std::string line;
    const std::unordered_map<std::string, CONFIG_TYPES> cfg_type = { {"TEXTURE", TEXTURE},
                                                                     {"FONT", FONT},
                                                                     {"ANIMATION", ANIMATION}};
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        std::string token;
        if (!(ss >> token)) {
            continue; // ignore blank lines
        }
        if (cfg_type.find(token) == cfg_type.end()) {
            throw std::runtime_error(config_file_error);
        }
        switch(cfg_type.at(token)) {
            case TEXTURE:
                load_texture(texture_manager, ss);
                break;
            case FONT:
                load_font(fonts, ss);
                break;
            case ANIMATION:
                load_animation(texture_manager, ifs);
                break;
        }
    }
    std::cout << "Loaded config file" << std::endl;
}

void WindowManager::resize_window(unsigned int width, unsigned int height) {
    background.setPosition(
        window.mapPixelToCoords(
            sf::Vector2i(0,0)
        )
    );
    auto texture = background.getTexture()->getSize();
    background.setScale(
        float(width) / texture.x,
        float(height) / texture.y
    );
}

void WindowManager::close_window() {
    window.close();
}

const sf::Font* WindowManager::get_font(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return &it->second;
    }
    throw std::runtime_error("Referencing unknown font!");
}

WindowManager::WindowManager(): null_placeholder() {
    load_from_config("media/config.cfg");
    window.create(sf::VideoMode(800,600), "Bomberman");
    window.setFramerateLimit(60);
    background.setTexture(texture_manager.get_ref("background"));
}