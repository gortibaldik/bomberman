#include "window_manager/def.hpp"
#include "states/start.hpp"
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Invalid arg count" << std::endl;
    return 2;
  }
  WindowManager wm(argv[1]);
  wm.push_state(std::make_unique<StartState>(wm));
  wm.loop();
}