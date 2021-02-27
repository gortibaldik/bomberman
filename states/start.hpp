#ifndef STATES_START_HPP
#define STATES_START_HPP

#include "menu_state.hpp"
class StartState: public MenuState {
public:
    StartState(WindowManager& window_manager);
protected:
    void handle_btn_pressed() override;
};
#endif
