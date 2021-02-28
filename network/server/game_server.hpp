#ifndef SERVER_COMMUNICATION_MANAGER_HPP
#define SERVER_COMMUNICATION_MANAGER_HPP

#include <unordered_map>
#include <memory>
#include "server.hpp"
#include "server_entity.hpp"
#include "ai_escaper.hpp"
#include "game/map_logic.hpp"

enum class ServerState {
        WAITING_ROOM,
        STARTING,
        RUNNING,
        END_GAME
};

using AIPlayers = std::unordered_map<std::string, AIEscaper*>;
using ServerPlayers = std::unordered_map<std::string, std::unique_ptr<ServerPlayerEntity>>;
using Scores = std::unordered_map<std::string, ScoreInfo>;

class GameServer: public Server {
public:
    GameServer(const std::string& map_cfg_name);
    ~GameServer();
    void set_ready_game();
    void start_game();
    size_t get_max_players() { return map.get_max_players(); }
    bool is_in_waiting_room() { return state == ServerState::WAITING_ROOM; }
protected:
    void listener_handle_others(const std::string& client_name, sf::Packet&, PacketType) override;
    void notify_disconnect(const std::string& client_name) override;
    void handle_starting_state(const std::string& client_name, sf::Packet&, PacketType);
    void handle_running_state(const std::string& client_name, sf::Packet&, PacketType);
    void game_notify_loop();
    void game_end_notify_loop(sf::Time&, sf::Clock&);
    bool update_bombs_explosions(float dt, sf::Packet&);
    bool update_players_damage(sf::Packet&);
    bool update_soft_blocks(sf::Packet&);
    bool update_player(const sf::Time&, ServerPlayerEntity&, sf::Packet&);
    void handle_player_die_event(const std::string& player_name);
    ServerState state;

    ServerPlayers players;
    Scores players_scores;
    AIPlayers ais;
    GameMapLogic map;

    std::thread notifier;
    std::atomic<bool> end_notifier;
    std::mutex players_mutex;

    int n_deployed_bombs = 0;
    int n_exploded_squares = 0;
    float move_factor = 0.15f;
    int player_lives = 3;
};

#endif
