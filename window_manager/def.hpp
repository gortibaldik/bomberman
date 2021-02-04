#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <deque>
#include <SFML/Graphics.hpp>
#include "states/abstract.hpp"
#include "texture_manager/def.hpp"

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

    WindowManager();
private:
    void load_textures();
    void load_fonts();

    GSPtr null_placeholder;
    bool remove_top = false;
    std::deque<GSPtr> states;
    std::map<std::string, sf::Font> fonts;
    TextureManager texture_manager;
};

#endif