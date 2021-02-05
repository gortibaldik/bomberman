#include "texture_manager.hpp"
#include <stdexcept>

Animation::Animation(const sf::Texture& texture, int milliseconds_frame_change)
                    : milliseconds_frame_change(milliseconds_frame_change / 1000.f)
                    , c_time(0.f) {
    sprite.setTexture(texture);
}

void Animation::update(float dt) const {
    if (steps.size() == 1) { return; }
    c_time += dt;
    if (c_time < milliseconds_frame_change) {return;}
    c_time = 0.f;
    c_anim_index = (c_anim_index + 1) % steps.size();
    auto [size_x, size_y, x, y] = steps[c_anim_index];
    sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
}

void Animation::add_animation_state(int size_x, int size_y, int x, int y) {
    steps.emplace_back(size_x, size_y, x, y);
    if (steps.size() == 1) {
        sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
    }
}

const sf::Sprite& Animation::get_sprite() const {
    if (steps.size() == 0) {
        throw std::runtime_error("Couldn't load non_existent sprite");
    }
    return sprite;
}

void Animation::set_position(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

void TextureManager::add_animation_state(const std::string& name, int size_x, int size_y, int x, int y) {
    if (animations.find(name) == animations.end()) {
        throw std::runtime_error("adding state to non_existent animation!");
    }
    animations.at(name).add_animation_state(size_x, size_y, x, y);
}

const Animation& TextureManager::get_animation(const std::string& name) const {
    if (animations.find(name) == animations.end()) {
        throw std::runtime_error("accessing non_existent animation!");
    }
    return animations.at(name);
}

void TextureManager::create_animation(const std::string& name, const std::string& texture_name, int milliseconds_frame_change) {
    if ((textures.find(texture_name) == textures.end()) ||
        (animations.find(name) != animations.end())) {
            throw std::runtime_error("Cannot create animation!");
    }
    animations.emplace(name, Animation(textures[texture_name], milliseconds_frame_change));
}

bool TextureManager::load_texture(const std::string& name, const std::string& filename) {
    return textures[name].loadFromFile(filename);
}

sf::Texture& TextureManager::get_ref(const std::string& texture) {
    return textures[texture];
}