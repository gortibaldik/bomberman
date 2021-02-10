#ifndef GAME_CLIENT_HPP
#define GAME_CLIENT_HPP
#include <unordered_map>
#include <mutex>
#include "base.hpp"
#include "game/map.hpp"
#include "client_entity.hpp"

using PlayerEntities = std::unordered_map<std::string, ClientPlayerEntity>;
using BombEntities = std::unordered_map<int, ClientBombEntity>;
using ExplosionEntities = std::unordered_map<int, ClientExplosionEntity>;

class GameClient : public Client {
public:
    GameClient(const std::string& name, const TextureManager& tm): Client(name), map(tm), tm(tm), me(nullptr) {}
    bool is_game_started() { return game_started; }
    bool is_approved() { return approved; }
    GameMapRenderable& get_game_map() { return map; }
    void fit_entities_to_window();
    void render_entities(sf::RenderTarget* target);
    const ClientPlayerEntity* me;
private:
    void handle_others(sf::Packet&, PacketType) override;
    void get_ready(sf::Packet&);
    void notify_disconnect() override;
    void server_state_update(sf::Packet&);
    void update_player(sf::Packet& packet);
    void create_bomb(sf::Packet& packet);
    void erase_bomb(sf::Packet& packet);
    void create_explosion(sf::Packet& packet);
    void erase_explosion(sf::Packet& packet);
    bool game_started = false;
    bool approved = false;
    bool only_viewer = false;

    GameMapRenderable map;
    const TextureManager& tm;
    PlayerEntities players;
    BombEntities bombs;
    ExplosionEntities explosions;
    std::mutex resources_mutex;
};
#endif
