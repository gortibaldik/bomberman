#include "texture_manager.hpp"
#include "anim_object.hpp"
#include <stdexcept>
#include <iostream>

void TextureManager::add_animation_state(const std::string& name, int size_x, int size_y, int x, int y, Direction dir) {
    if (animations.find(name) == animations.end()) {
        throw std::runtime_error("adding state to non_existent animation!");
    }
    animations.at(name).add_animation_state(size_x, size_y, x, y, dir);
}

void TextureManager::add_animation_frame_rate(const std::string& name, int mft, Direction dir) {
    if (animations.find(name) == animations.end()) {
        throw std::runtime_error("Adding frame rate to non_existent animation");
    }
    animations.at(name).add_frame_rate(mft, dir);
}

AnimObject TextureManager::get_anim_object(const std::string& name) const {
    if (animations.find(name) == animations.end()) {
        throw std::runtime_error("animation " + name + " doesn't exist!");
    }
    return animations.at(name).get_anim_object();
}

void TextureManager::create_animation(const std::string& name
                                     , const std::string& texture_name
                                     , Direction default_direction) {
    if (animations.find(name) != animations.end()) {
            throw std::runtime_error("Animation with given name already exists!");
    }
    animations.emplace(name, Animation(get_texture(texture_name), default_direction));
}

bool TextureManager::load_texture(const std::string& name, const std::string& filename) {
    if (textures.find(name) == textures.end()) {
        textures.emplace(name, sf::Texture());
        bool result = textures.at(name).loadFromFile(filename);
        if (result) {
            return true;
        } else {
            textures.erase(name);
            return false;
        }
    }
    return textures.at(name).loadFromFile(filename);
}

bool TextureManager::load_font(const std::string& name, const std::string& filename) {
    if (fonts.find(name) == fonts.end()) {
        fonts.emplace(name, sf::Font());
        bool result = fonts.at(name).loadFromFile(filename);
        if (result) {
            return true;
        } else {
            fonts.erase(name);
            return false;
        }
    }
    return fonts[name].loadFromFile(filename);
}

void TextureManager::load_color(const std::string& name, sf::Color&& color) {
    if (colors.find(name) == colors.end()) {
        colors.emplace(name, std::move(color));
    } else {
        colors.at(name) = std::move(color);
    }
}

const sf::Font& TextureManager::get_font(const std::string& name) const {
    if (fonts.find(name) == fonts.end()) {
        throw std::runtime_error("Invalid font name! : " + name);
    }
    return fonts.at(name);
}

sf::Texture& TextureManager::get_texture(const std::string& name) {
    if (textures.find(name) == textures.end()) {
        throw std::runtime_error("Invalid texture name! : " + name);
    }
    return textures.at(name);
}

const sf::Color& TextureManager::get_color(const std::string& name) const {
    if (colors.find(name) == colors.end()) {
        throw std::runtime_error("Invalid color name! : " + name);
    }
    return colors.at(name);
}

TextureManager::TextureManager() {
    colors = {
        { "TRANSPARENT", sf::Color::Transparent },
        { "BLACK", sf::Color::Black },
        { "BLUE", sf::Color::Blue },
        { "WHITE", sf::Color::White }
    };
}