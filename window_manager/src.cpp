#include "def.hpp"

#include <SFML/System.hpp>
#include <stdexcept>
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
            states.pop_back();
            std::cout << states.size() << std::endl;
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

void WindowManager::load_textures() {
    texture_manager.load_texture("background", "media/background.jpeg");
}

void WindowManager::load_fonts() {
    sf::Font f;
    f.loadFromFile("media/font.ttf");
    fonts["main_font"] = std::move(f);
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

const sf::Font* WindowManager::get_font(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return &it->second;
    }
    throw std::runtime_error("Referencing unknown font!");
}

WindowManager::WindowManager(): null_placeholder() {
    load_textures();
    load_fonts();
    window.create(sf::VideoMode(800,600), "Bomberman");
    window.setFramerateLimit(60);
    background.setTexture(texture_manager.get_ref("background"));
}