#include "anim_object.hpp"

AnimObject::AnimObject(const sf::Sprite& sprite
                       , float milliseconds_frame_change
                       , const Animation* animation
                       , Direction default_direction)
                      : sprite(sprite)
                      , milliseconds_frame_change(milliseconds_frame_change)
                      , animation(animation)
                      , anim_direction(default_direction)
                      , actual_direction(EntityDirection::STATIC)
                      , c_time(0.f) {
    switch(default_direction) {
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

void AnimObject::set_direction(EntityDirection dir) {
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

void AnimObject::set_default() {
    if (c_anim_index == 0) { return; }
    c_anim_index = 0;
    auto& [size_x, size_y, x, y] = animation->get_idx(anim_direction, c_anim_index);
    sprite.setTextureRect(sf::IntRect(x, y, size_x, size_y));
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