#ifndef GAME_CLIENT_HPP
#define GAME_CLIENT_HPP
#include <unordered_map>
#include <mutex>
#include "base.hpp"
#include "game/map_renderable.hpp"
#include "client_entity.hpp"

using PlayerEntities = std::unordered_map<std::string, ClientPlayerEntity>;
using BombEntities = std::unordered_map<int, ClientBombEntity>;
using ExplosionEntities = std::unordered_map<int, ClientExplosionEntity>;
using SoftBlocks = std::unordered_map<int, ClientSoftBlockEntity>;
using PlayerScores = std::vector<std::string>;

enum class GameClientStatus {
    IDLE,
    GAME_STARTED,
    GAME_APPROVED,
    GAME_ENDED,
    GAME_ASK_READY
};

class GameClient : public Client {
public:
    GameClient( const std::string& name
              , const TextureManager& tm
              , const sf::Font& font
              , bool main = false)
              : Client(name)
              , map(tm)
              , tm(tm)
              , me(nullptr)
              , font(font)
              , main(main) {}
    ~GameClient();
    bool is_game_started() { return status == GameClientStatus::GAME_STARTED; }
    bool is_approved() { return (status == GameClientStatus::GAME_STARTED) ||
                                (status == GameClientStatus::GAME_APPROVED); }
    bool is_main() { return main; }
    void send_set_ready();
    void send_start();
    GameClientStatus get_game_status() { return status; }
    PlayerScores& get_players_scores() { return players_scores; }
    void update(float dt);
    GameMapRenderable& get_game_map() { return map; }
    ReceiverQueue& get_received_messages() { return received_messages; }
    void fit_entities_to_window();
    void render_entities(sf::RenderTarget* target);
    void render_players(sf::RenderTarget* target);
    const ClientPlayerEntity* me;
private:
    void handle_others(sf::Packet&, PacketType) override;
    void get_ready(sf::Packet&);
    void notify_disconnect() override;
    void server_state_update(sf::Packet&);
    void update_player(sf::Packet& packet, bool spawn);
    void create_bomb(sf::Packet& packet);
    void erase_bomb(sf::Packet& packet);
    void create_explosion(sf::Packet& packet);
    void erase_explosion(sf::Packet& packet);
    void erase_player(sf::Packet& packet);
    void create_soft_block(sf::Packet& packet);
    void destroy_soft_block(sf::Packet& packet);
    void destroy_power_up(sf::Packet& packet);

    GameClientStatus status = GameClientStatus::IDLE;
    const bool main;

    GameMapRenderable map;
    const TextureManager& tm;
    const sf::Font& font;

    PlayerEntities players;
    BombEntities bombs;
    ExplosionEntities explosions;
    SoftBlocks soft_blocks;
    PlayerScores players_scores;
    ReceiverQueue received_messages;

    std::mutex resources_mutex;
};
#endif
