#include "abstract.hpp"
#include "window_manager/def.hpp"

void GameState::update_mouse_pos() {
    mouse_pos = window_manager.window.mapPixelToCoords(sf::Mouse::getPosition(window_manager.window));
}

void set_validator(ControlField* field_to_check, 
                    ControlField* field_to_modify,
                    const std::string& new_value) {
    std::string new_content = "";
    if (!field_to_check->is_valid()) {
        new_content = new_value;
    }
    field_to_modify->set_content(new_content);
}