#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <deque>
#include <string>
#include <SFML/Graphics.hpp>
#include "states/abstract.hpp"
#include "texture_handling/texture_manager.hpp"

using Fonts = std::map<std::string, sf::Font>;

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

    const GSPtr& peek_state();
    const sf::Font* get_font(const std::string&);
    const TextureManager& get_tm() { return texture_manager; }

    WindowManager();
private:
    void load_from_config(const std::string& name_of_file);

    GSPtr null_placeholder;
    bool remove_top = false;
    std::deque<GSPtr> states;
    Fonts fonts;
    TextureManager texture_manager;
};

#endif