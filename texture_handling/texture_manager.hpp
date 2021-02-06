#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <tuple>

enum class Direction {
    UP,
    DOWN,
    SIDE,
    STATIC
};

class AnimObject;

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

class AnimObject {
public:
    AnimObject(const sf::Sprite&, float milliseconds_frame_change, const Animation& animation, Direction default_direction);
    void set_position(const sf::Vector2f& pos);
    void update(float);
    const sf::Sprite& get_sprite() const { return sprite; }
private:
    sf::Sprite sprite;
    const Animation& animation;
    float milliseconds_frame_change;
    float c_time;
    int c_anim_index = 0;
    Direction actual_direction;
};

class TextureManager {
public:
    void create_animation(const std::string& name
                         , const std::string& texture_name
                         , Direction default_direction);
    void add_animation_frame_rate(const std::string&, int, Direction);
    void add_animation_state(const std::string& name, int, int, int, int, Direction);
    AnimObject get_anim_object(const std::string& name) const;
    bool load_texture(const std::string& name, const std::string& filename);
    sf::Texture& get_ref(const std::string& texture);
private:
    std::map<std::string, Animation> animations;
    std::map<std::string, sf::Texture> textures;
};
#endif