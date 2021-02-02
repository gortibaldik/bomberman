#include "network/client/def.hpp"
#include "network/server/def.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        Client client("Ferino");
        client.connect(sf::IpAddress("10.128.0.207"), 15000);
    } else {
        Server server;
        server.start(15000);
    }
}