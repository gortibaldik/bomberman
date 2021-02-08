#ifndef SERVER_COMMUNICATION_MANAGER_HPP
#define SERVER_COMMUNICATION_MANAGER_HPP
#include "server.hpp"
#include "game/map.hpp"

namespace ServerState {
    enum ServerState {
        WAITING_ROOM,
        STARTING,
        RUNNING
    };
};

struct PlayerInfo {
    std::string name;

    PlayerInfo(const std::string& client_name): name(client_name) {}
};

using Players = std::unordered_map<std::string, PlayerInfo>;

class GameServer: public Server {
public:
    GameServer(int max_clients): Server(max_clients), state(ServerState::WAITING_ROOM) {}
    void set_ready_game(const std::string& = "");
    void start_game();
    bool is_in_waiting_room() { return state == ServerState::WAITING_ROOM; }
protected:
    void handle_others(const std::string& client_name, sf::Packet&, PacketType) override;
    void notify_disconnect(const std::string& client_name) override;
    void handle_starting_state(const std::string& client_name, sf::Packet&, PacketType);
    ServerState::ServerState state;
    Players players;
    GameMapLogic map;
};

#endif
