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
        throw std::runtime_error("accessing non_existent animation!");
    }
    return animations.at(name).get_anim_object();
}

void TextureManager::create_animation(const std::string& name
                                     , const std::string& texture_name
                                     , Direction default_direction) {
    if ((textures.find(texture_name) == textures.end()) ||
        (animations.find(name) != animations.end())) {
            throw std::runtime_error("Cannot create animation!");
    }
    animations.emplace(name, Animation(textures[texture_name], default_direction));
}

bool TextureManager::load_texture(const std::string& name, const std::string& filename) {
    return textures[name].loadFromFile(filename);
}

bool TextureManager::load_font(const std::string& name, const std::string& filename) {
    return fonts[name].loadFromFile(filename);
}

const sf::Font& TextureManager::get_font(const std::string& name) const {
    return fonts.at(name);
}

sf::Texture& TextureManager::get_ref(const std::string& texture) {
    return textures[texture];
}