#include "network/client/def.hpp"
#include "network/server/def.hpp"

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
        Client client(argv[1]);
        client.connect(sf::IpAddress("192.168.43.246"), 15000);
        while(client.is_connected()) {}
    }
}