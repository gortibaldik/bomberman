#include "def.hpp"
#include "texture_handling/texture_loader.hpp"

#include <SFML/System.hpp>
#include <iostream>

void WindowManager::push_state(GSPtr&& state) {
    states.push_back(std::move(state));
}

void WindowManager::pop_states(int n) {
    while (n > 0) {
        if (states.empty()) {
            throw std::runtime_error("Couldn't pop as many states!");
        } else {
            states.pop_back();
            n--;
        }
    }
}

void WindowManager::change_state(GSPtr&& state) {
    if (!states.empty()) {
        pop_states(1);
    }
    push_state(std::move(state));
}

GSPtr WindowManager::peek_state() {
    if (states.empty()) {
        return null_placeholder;
    } else {
        return states.back();
    }
}

void WindowManager::loop() {
    sf::Clock clock;
    while(window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        actual_state = peek_state();
        if (actual_state == nullptr) {
            throw std::runtime_error("WINDOW MANAGER: NOTHING TO SHOW!");
            continue;
        }
        
        actual_state->handle_input();
        actual_state->update(dt);
        window.clear(sf::Color::Black);
        actual_state->draw(dt);
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

WindowManager::WindowManager(): null_placeholder()
                              , window_name("Bomberman")
                              , actual_state(nullptr) {
    // load all the textures
    TextureLoader tl("media/config.cfg");
    tl.load(texture_manager);

    StyleLoader sl("media/button_style.cfg", texture_manager);
    sl.load(styles_holder);

    CGStyleLoader cgsl("media/menu_configs.cfg", styles_holder);
    cgsl.load(cgstyles_holder);

    window.create(sf::VideoMode(800,600), window_name);
    window.setFramerateLimit(60);
    background.setTexture(texture_manager.get_texture("background"));
    std::cout << "The configs were successfully processed!" << std::endl;
}