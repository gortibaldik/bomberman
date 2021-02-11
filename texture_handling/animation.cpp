#include "animation.hpp"
#include "anim_object.hpp"

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