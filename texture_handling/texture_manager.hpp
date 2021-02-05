#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <tuple>

class Animation {
public:
    Animation(const sf::Texture& texture, int milliseconds_frame_change);
    void add_animation_state(int x, int y, int size_x, int size_y);
    const sf::Sprite& get_sprite() const;
    void set_position(const sf::Vector2f& pos);
    void update(float) const;

private:
    mutable sf::Sprite sprite;
    std::vector<std::tuple<int, int, int, int>> steps;
    float milliseconds_frame_change;
    mutable float c_time;
    mutable int c_anim_index = 0;
};

class TextureManager {
public:
    void create_animation(const std::string& name, const std::string& texture_name, int milliseconds_frame_change);
    void add_animation_state(const std::string& name, int, int, int, int);
    const Animation& get_animation(const std::string& name) const;
    bool load_texture(const std::string& name, const std::string& filename);
    sf::Texture& get_ref(const std::string& texture);
private:
    std::map<std::string, Animation> animations;
    std::map<std::string, sf::Texture> textures;
};
#endif