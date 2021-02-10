#include "texture_manager.hpp"
#include <stdexcept>
#include <iostream>

Animation::Animation(const sf::Texture& texture
                    , Direction default_direction)
                    : default_direction(default_direction) {
    sprite.setTexture(texture);
}

const std::tuple<int, int, int, int>& Animation::get_idx(Direction dir, std::size_t idx) const {
    if ((directions.find(dir) != directions.end()) && (idx >= 0) && (idx < directions.at(dir).size())) {
        return directions.at(dir).at(idx);
    }
    throw std::runtime_error("ANIMATION: invalid indexing!");
}


void Animation::add_animation_state(int size_x, int size_y, int x, int y, Direction dir) {
    if ((default_direction == Direction::STATIC) && (dir != Direction::STATIC)) {
        throw std::runtime_error("Cannot change directions of non_directional animation!");
    }
    directions[dir].emplace_back(size_x, size_y, x, y);
    if ((dir == default_direction) && (directions[dir].size() == 1)) {
        sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
    }
}

void Animation::add_frame_rate(int mft, Direction dir) {
    if (frame_rates.find(dir) != frame_rates.end()) {
        throw std::runtime_error("Frame rate for direction already set!");
    }
    frame_rates[dir] = mft / 1000.f;
}

AnimObject Animation::get_anim_object() const {
    if ((directions.size() == 0) || (directions.find(default_direction) == directions.end()) || 
        (directions.at(default_direction).size() == 0)) {
        throw std::runtime_error("Couldn't load non_existent anim object");
    }
    return AnimObject(sprite, frame_rates.at(default_direction), this, default_direction);
}

const std::size_t Animation::size(Direction dir) const {
    if (directions.find(dir) == directions.end()) {
        throw std::runtime_error("Invalid direction for this animation!");
    }
    return directions.at(dir).size();
}

AnimObject::AnimObject(const sf::Sprite& sprite
                       , float milliseconds_frame_change
                       , const Animation* animation
                       , Direction default_direction)
                      : sprite(sprite)
                      , milliseconds_frame_change(milliseconds_frame_change)
                      , animation(animation)
                      , anim_direction(default_direction)
                      , c_time(0.f) {
    switch(default_direction) {
    case Direction::STATIC:
        this->actual_direction = EntityDirection::STATIC;
        break;
    case Direction::DOWN:
        this->actual_direction = EntityDirection::DOWN;
        break;
    case Direction::UP:
        this->actual_direction = EntityDirection::UP;
        break;
    case Direction::SIDE:
        this->actual_direction = EntityDirection::RIGHT;
        break;
    }
}

void AnimObject::set_position(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

void AnimObject::set_position(float x, float y) {
    sprite.setPosition(x, y);
}

void AnimObject::set_direction(EntityDirection::EntityDirection dir) {
    if (actual_direction == dir) { return; }
    bool flip = (dir == EntityDirection::LEFT);
    bool reverse_flip = (actual_direction == EntityDirection::LEFT);
    switch(dir) {
    case EntityDirection::UP:
        anim_direction = Direction::UP;
        break;
    case EntityDirection::DOWN:
        anim_direction = Direction::DOWN;
        break;
    case EntityDirection::LEFT:
    case EntityDirection::RIGHT:
        anim_direction = Direction::SIDE;
        break;
    }
    c_anim_index = 0;
    actual_direction = dir;
    auto& [size_x, size_y, x, y] = animation->get_idx(anim_direction, c_anim_index);
    sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
    if (flip) {
        sprite.setOrigin(sprite.getLocalBounds().width, 0.f);
        sprite.scale(-1,1);
    } else if (reverse_flip) {
        sprite.setOrigin(0.f, 0.f);
        sprite.scale(-1,1);
    }
}

void AnimObject::scale(float x, float y) {
    sprite.scale(x, y);
}

void AnimObject::update(float dt) {
    if (animation->size(anim_direction) == 1) { return; }
    c_time += dt;
    if (c_time < milliseconds_frame_change) { return; }
    c_time = 0.f;
    c_anim_index = (c_anim_index + 1) % animation->size(anim_direction);
    auto& [size_x, size_y, x, y] = animation->get_idx(anim_direction, c_anim_index);
    sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
}

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

sf::Texture& TextureManager::get_ref(const std::string& texture) {
    return textures[texture];
}