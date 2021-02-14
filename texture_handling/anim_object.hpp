#ifndef GAME_ANIM_OBJECT_HPP
#define GAME_ANIM_OBJECT_HPP

#include <SFML/Graphics.hpp>
#include "texture_manager.hpp"

class AnimObject {
public:
    AnimObject(const sf::Sprite&, float milliseconds_frame_change, const Animation* animation, Direction default_direction);
    void set_position(const sf::Vector2f& pos);
    void set_position(float, float);
    void set_direction(EntityDirection);

    sf::FloatRect get_global_bounds() { return sprite.getGlobalBounds(); }
    void scale(float, float);
    void update(float);
    const sf::Sprite& get_sprite() const { return sprite; }
    void set_default();
private:
    sf::Sprite sprite;
    const Animation* animation;
    float milliseconds_frame_change;
    float c_time;
    int c_anim_index = 0;
    EntityDirection actual_direction;
    Direction anim_direction;
};

#endif