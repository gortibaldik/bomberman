#include "window_manager/def.hpp"
#include "states/start.hpp"

int main(int argc, char** argv) {
    WindowManager wm;
    wm.push_state(std::make_unique<StartState>(wm));
    wm.loop();
}