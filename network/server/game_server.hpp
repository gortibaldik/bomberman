#ifndef SERVER_COMMUNICATION_MANAGER_HPP
#define SERVER_COMMUNICATION_MANAGER_HPP
#include "server.hpp"

namespace ServerState {
    enum ServerState {
        WAITING_ROOM,
        STARTING,
        RUNNING
    };
};

class GameServer: public Server {
public:
    GameServer(int max_clients): Server(max_clients), state(ServerState::WAITING_ROOM) {}
    void set_ready_game();
    bool is_in_waiting_room() { return state == ServerState::WAITING_ROOM; }
protected:
    void handle_others(sf::Packet&, PacketType) override;
    void handle_ack()
    ServerState::ServerState state;
};

#endif
