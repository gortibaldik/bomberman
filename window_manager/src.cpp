#include "def.hpp"
#include "texture_handling/texture_loader.hpp"

#include <SFML/System.hpp>
#include <iostream>

void WindowManager::push_state(GSPtr&& state) {
    states.push_back(std::move(state));
}

void WindowManager::pop_states(int n) {
    while (n > 1) {
        if (states.empty()) {
            throw std::runtime_error("Couldn't pop as many states!");
        } else {
            states.erase(states.end()-2);
            n--;
        }
    }
    remove_top = true;
}

void WindowManager::change_state(GSPtr&& state) {
    if (!states.empty()) {
        pop_states(1);
        states.insert(states.end()-1, std::move(state));
    } else {
        push_state(std::move(state));
    }
}

const GSPtr& WindowManager::peek_state() {
    if (states.empty()) {
        return null_placeholder;
    } else {
        return states.back();
    }
}

void WindowManager::loop() {
    sf::Clock clock;
    while(window.isOpen()) {
        if (remove_top) {
            std::cout << "removing state" << std::endl;
            states.pop_back();
            remove_top = false;
        }
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        auto&& state_ptr = peek_state();
        if (!state_ptr) {
            continue;
        }
        
        state_ptr->handle_input();
        state_ptr->update(dt);
        window.clear(sf::Color::Black);
        state_ptr->draw(dt);
        window.display();
    }
}

void WindowManager::resize_window(unsigned int width, unsigned int height) {
    background.setPosition(
        window.mapPixelToCoords(
            sf::Vector2i(0,0)
        )
    );
    auto texture = background.getTexture()->getSize();
    background.setScale(
        float(width) / texture.x,
        float(height) / texture.y
    );
}

void WindowManager::close_window() {
    window.close();
}

WindowManager::WindowManager(): null_placeholder() {
    // load all the textures
    TextureLoader tl("media/config.cfg");
    tl.load(texture_manager);

    window.create(sf::VideoMode(800,600), "Bomberman");
    window.setFramerateLimit(60);
    background.setTexture(texture_manager.get_ref("background"));
}