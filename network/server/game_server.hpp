#ifndef SERVER_COMMUNICATION_MANAGER_HPP
#define SERVER_COMMUNICATION_MANAGER_HPP
#include "server.hpp"
#include "server_entity.hpp"
#include "game/map.hpp"

namespace ServerState {
    enum ServerState {
        WAITING_ROOM,
        STARTING,
        RUNNING
    };
};

using Players = std::unordered_map<std::string, ServerPlayerEntity>;
using Bombs = std::unordered_map<int, ServerBombEntity>;
using Explosions = std::unordered_map<int, ServerExplosionEntity>;

class GameServer: public Server {
public:
    GameServer(int max_clients): Server(max_clients), state(ServerState::WAITING_ROOM) {}
    ~GameServer();
    void set_ready_game(const std::string& = "");
    void start_game();
    bool is_in_waiting_room() { return state == ServerState::WAITING_ROOM; }
protected:
    void handle_others(const std::string& client_name, sf::Packet&, PacketType) override;
    void notify_disconnect(const std::string& client_name) override;
    void handle_starting_state(const std::string& client_name, sf::Packet&, PacketType);
    void handle_running_state(const std::string& client_name, sf::Packet&, PacketType);
    void game_notify_loop();
    ServerState::ServerState state;
    Players players;
    Bombs bombs;
    Explosions explosions;
    GameMapLogic map;

    std::thread notifier;
    std::atomic<bool> end_notifier = false;
    std::mutex players_mutex;

    int n_deployed_bombs = 0;
    int n_exploded_squares = 0;
    float move_factor = 0.15f;
    int player_lives = 3;
};

#endif