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

static std::string config_file_error = "Invalid config file";

static void load_animation(TextureManager& tm, std::istream& is) {
    std::string animation_name, texture_name;
    int duration, milliseconds_frame_change;
    std::string line;
    std::getline(is, line);
    std::stringstream _ss(line);
    if (!(_ss >> animation_name >> duration >> milliseconds_frame_change >> texture_name)) {
        throw std::runtime_error(config_file_error);
    }
    std::cout << "Creating animation dur:" << duration << "; mfc: " << milliseconds_frame_change << std::endl;
    tm.create_animation(animation_name, texture_name, milliseconds_frame_change);
    while (duration > 0) {
        int size_x, size_y, x, y;
        std::getline(is, line);
        std::stringstream ss(line);
        if (!(ss >> size_x >> size_y >> x >> y)) {
            throw std::runtime_error(config_file_error);
        }
        std::cout << "Creating animation state " << size_x << "," << size_y << ","  << x << "," << y << std::endl;
        tm.add_animation_state(animation_name, size_x, size_y, x, y);
        duration--;
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