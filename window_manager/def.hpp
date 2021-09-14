#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <deque>
#include <string>
#include <SFML/Graphics.hpp>
#include "states/abstract.hpp"
#include "texture_handling/texture_manager.hpp"
#include "controls/style_loader.hpp"

class WindowManager {
public:
    sf::RenderWindow window;
    sf::Sprite background;

    void loop();
    void pop_states(int);
    void push_state(GSPtr&& state);
    void change_state(GSPtr&& state);

    void close_window();
    void resize_window(unsigned int width, unsigned int height);
    sf::Vector2f get_window_size() { return sf::Vector2f(window.getSize()); }

    GSPtr peek_state();
    const TextureManager& get_tm() { return texture_manager; }
    const StylesHolder<GStyle>& get_sh() { return styles_holder; }
    const StylesHolder<CGStyle>& get_cgsh() { return cgstyles_holder;}
    const std::string& get_media_dir() { return media_dir; }

    WindowManager(const std::string& media_dir);
private:

    GSPtr null_placeholder;
    GSPtr actual_state;
    bool remove_top = false;
    std::deque<GSPtr> states;
    std::string window_name;
    std::string media_dir;
    TextureManager texture_manager;
    StylesHolder<GStyle> styles_holder;
    StylesHolder<CGStyle> cgstyles_holder;
};

#endif