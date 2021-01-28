#include "start.hpp"
#include "window_manager/def.hpp"
#include <iostream>

static const int mb_width = 100;
static const int mb_height = 50;
static const std::string mb_starttxt = "New Game";

void GameStateStart::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
    for (auto&& button : menu) {
        button->render(&window_manager.window);
    }
}

void handle_resize_menu(GameStateStart& gss, unsigned int width, unsigned int height) {
    sf::Vector2f pos = sf::Vector2f(width, height);
    pos *= 0.5f;
    pos = gss.window_manager.window.mapPixelToCoords(sf::Vector2i(pos), gss.view);
    for (auto&& button : gss.menu) {
        button->move_pos(pos.x, pos.y);
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
            view.setSize(event.size.width, event.size.height);
            window_manager.resize_window(event.size.width, event.size.height);
            break;
        case sf::Event::MouseMoved: case sf::Event::MouseLeft:
            update_mouse_pos();
            for (auto&& button : menu) {
                button->handle_input(mouse_pos);
            }
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
                        0.f) {
    sf::Vector2f pos(mngr.window.getSize());
    view.setSize(pos);
    pos *= 0.5f;
    view.setCenter(pos);
    menu.push_back(std::make_unique<Button>(pos.x, pos.y, mb_height, mb_starttxt, &menu_btn_style));
}


void GameStateStart::update(float dt) {
    for (auto&& button : menu) {
        button->update();
    }
}