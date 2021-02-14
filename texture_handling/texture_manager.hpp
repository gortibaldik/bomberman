#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include "game/entity.hpp"
#include "animation.hpp"

using Fonts = std::map<std::string, sf::Font>;

class TextureManager {
public:
    void create_animation(const std::string& name
                         , const std::string& texture_name
                         , Direction default_direction);
    void add_animation_frame_rate(const std::string&, int, Direction);
    void add_animation_state(const std::string& name, int, int, int, int, Direction);
    AnimObject get_anim_object(const std::string& name) const;

    bool load_texture(const std::string& name, const std::string& filename);
    sf::Texture& get_texture(const std::string& texture);

    bool load_font(const std::string& name, const std::string& filename);
    const sf::Font& get_font(const std::string& font_name) const;
private:
    std::map<std::string, Animation> animations;
    std::map<std::string, sf::Texture> textures;
    Fonts fonts;
};
#endif