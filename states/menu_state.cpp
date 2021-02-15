#include "start.hpp"
#include "window_manager/def.hpp"
#include <iostream>
#include <unordered_map>

void MenuState::draw(float dt) {
    window_manager.window.setView(view);
    window_manager.window.draw(window_manager.background);
    menu.render(&window_manager.window);
}

void MenuState::handle_input() {
    sf::Event event;
    while(window_manager.window.pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            window_manager.close_window();
            break;
        case sf::Event::MouseMoved: case sf::Event::MouseButtonPressed:
            update_mouse_pos();
            menu.handle_input(mouse_pos, event);
            handle_btn_pressed();
            break;
        default:
            menu.handle_input(mouse_pos, event);
            break;
        }
    }
}

MenuState::MenuState(WindowManager& mngr
                    , const std::string& style_name)
                    : State(mngr)
                    , factor(mngr.get_cgsh().get_style(style_name).factor)
                    , menu( mngr.get_cgsh().get_style(style_name)
                          , mngr.get_window_size()) {
    sf::Vector2f pos(mngr.window.getSize());
    view.setSize(pos);
    pos *= 0.5f;
    view.setCenter(pos);
}

MenuState::MenuState(WindowManager& mngr
                    , const sf::View& view
                    , const std::string& style_name)
                    : State(mngr)
                    , view(view)
                    , factor(mngr.get_cgsh().get_style(style_name).factor)
                    , menu( mngr.get_cgsh().get_style(style_name)
                          , mngr.get_window_size()){}

void MenuState::update(float dt) {
    menu.update();
}