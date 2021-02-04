#include "window_manager/def.hpp"
#include "network/server/def.hpp"
#include "states/start.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        Server server;
        server.start(15000);
        sf::Clock c;
        c.restart();
        while(server.is_running()) {
            server.update(c.restart());
        }
    } else {
        WindowManager wm;
        wm.push_state(std::make_unique<StartState>(wm));
        wm.loop();
    }
}