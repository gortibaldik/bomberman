#ifndef GAME_CLIENT_HPP
#define GAME_CLIENT_HPP
#include "base.hpp"
#include "game/map.hpp"

class GameClient : public Client {
public:
    GameClient(const std::string& name, const TextureManager& tm): Client(name), map(tm) {}
    bool is_game_started() { return game_started; }
    bool is_approved() { return approved; }
    GameMapRenderable& get_game_map() { return map; } 
private:
    void handle_others(sf::Packet&, PacketType) override;
    void get_ready(sf::Packet&);
    void notify_disconnect() override;
    bool game_started = false;
    bool approved = false;
    GameMapRenderable map;
};
#endif
