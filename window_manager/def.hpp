#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <deque>
#include <string>
#include <SFML/Graphics.hpp>
#include "states/abstract.hpp"
#include "texture_handling/texture_manager.hpp"

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
    const TextureManager& get_tm() { return texture_manager; }

    WindowManager();
private:

    GSPtr null_placeholder;
    bool remove_top = false;
    std::deque<GSPtr> states;
    TextureManager texture_manager;
};

#endif