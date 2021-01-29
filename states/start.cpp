#include "start.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

enum BTN {
    NEW_GAME,
    CONNECT,
    CREATE,
    QUIT
};

static const unsigned int mb_txt_size = 30;
static const std::unordered_map<std::string, BTN> mb_entries = { 
    {"Quit", QUIT}, {"Connect to a server", CONNECT}, {"Create a server", CREATE}, {"New Game", NEW_GAME}
};

void GameStateStart::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
    menu.render(&window_manager.window);
}

void handle_resize_menu(GameStateStart& gss, unsigned int width, unsigned int height) {
    float before = (float(gss.view.getSize().x) + float(gss.view.getSize().y)) / 2.f;
    float after = (float(width) + float(height)) / 2.f;
    sf::Vector2f pos = sf::Vector2f(width, height);
    pos *= 0.38f;
    pos = gss.window_manager.window.mapPixelToCoords(sf::Vector2i(pos), gss.view);
    gss.menu.move_pos(after/before, pos.x, pos.y);
}

void handle_btn_pressed(GameStateStart& gss) {
    auto&& btn = gss.menu.get_pressed_btn();
    if (btn) {
        auto it = mb_entries.find(btn->get_text());
        if (it == mb_entries.end()) {
            return;
        }
        switch (it->second) {
        case QUIT:
            gss.window_manager.window.close();
            break;
        }
    }
}

void GameStateStart::handle_input() {
    sf::Event event;
    while(window_manager.window.pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            window_manager.close_window();
            break;
        case sf::Event::Resized:
            handle_resize_menu(*this, event.size.width, event.size.height);
            view.setSize(event.size.width, event.size.height);
            window_manager.resize_window(event.size.width, event.size.height);
            break;
        case sf::Event::MouseMoved: case sf::Event::MouseButtonPressed:
            update_mouse_pos();
            menu.handle_input(mouse_pos);
            handle_btn_pressed(*this);
            break;
        }
    }
}

GameStateStart::GameStateStart(WindowManager& mngr):
        GameState(mngr),
        menu_btn_style( sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Transparent,
                        sf::Color::Black,
                        sf::Color::Blue,
                        mngr.get_font("main_font"),
                        1.f) {
    sf::Vector2f pos(mngr.window.getSize());
    view.setSize(pos);
    pos *= 0.5f;
    view.setCenter(pos);
    menu.initialize(pos.x, pos.y, mb_txt_size, &menu_btn_style);
    for (auto&& it : mb_entries) {
        menu.add_button(it.first);
    }
}


void GameStateStart::update(float dt) {
    menu.update();
}