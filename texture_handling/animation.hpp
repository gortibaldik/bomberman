#ifndef GAME_ANIMATION_HPP
#define GAME_ANIMATION_HPP

#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

class TextureManager;
class AnimObject;

enum class Direction {
    UP,
    DOWN,
    SIDE,
    STATIC
};

using Steps = std::vector<std::tuple<int, int, int, int>>;
using Directions = std::map<Direction, Steps>;
using FrameRates = std::map<Direction, float>;

class Animation {
public:
    Animation(const sf::Texture& texture, Direction default_dir);
    void add_animation_state(int x, int y, int size_x, int size_y, Direction);
    void add_frame_rate(int, Direction);
    AnimObject get_anim_object() const;
    const std::size_t size(Direction dir) const;
    const std::tuple<int, int, int, int>& get_idx(Direction, std::size_t idx) const; 

private:
    sf::Sprite sprite;
    Direction default_direction;
    Directions directions;
    FrameRates frame_rates;
};

#endif
