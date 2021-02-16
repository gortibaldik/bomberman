#ifndef STATE_ABSTRACT_HPP
#define STATE_ABSTRACT_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include "controls/control_field.hpp"

class WindowManager;

class State {
public:
    virtual void draw(float dt) = 0;
    virtual void update(float dt) = 0;
    virtual void handle_input() = 0;
    void disable() { disabled = true; }
    void enable() { disabled = false; }
    virtual ~State() = default;
protected:
    State(WindowManager& window_manager):
        window_manager(window_manager){}
    void update_mouse_pos();

    sf::Vector2f mouse_pos;
    WindowManager& window_manager;
    bool disabled = false;
};
using GSPtr = std::shared_ptr<State>;

void set_validator(ControlField* field_to_check, 
                    ControlField* field_to_modify,
                    const std::string& new_value);
#endif