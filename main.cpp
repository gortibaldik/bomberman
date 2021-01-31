#include "window_manager/def.hpp"
#include "states/start.hpp"

int main() {
    WindowManager wmanager;
    wmanager.push_state(std::make_unique<StartState>(wmanager));
    wmanager.loop();
}