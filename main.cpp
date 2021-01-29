#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include "window_manager/def.hpp"
#include "states/start.hpp"

int main()
{
    WindowManager wmanager;
    wmanager.push_state(std::make_unique<GameStateStart>(wmanager));
    wmanager.loop();
}