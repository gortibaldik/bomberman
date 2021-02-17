/* Bomb manager on server side should have records of all the bombs
 * which are currently active, number of all the bombs deployed, timings
 * and power of bombs, about the explosions
 * 
 * -> it is the server responsibility to know about the players
 * proposed methods: 
 *  - update(float dt)
 *  - act
 */

#ifndef GAME_SERVER_BOMB_MANAGER_HPP
#define GAME_SERVER_BOMB_MANAGER_HPP

#include "server_entity.hpp"
#include "game/map_logic.hpp"
#include <SFML/Network.hpp>

using Bombs = std::unordered_map<int, ServerBombEntity>;
using Explosions = std::unordered_map<int, ServerExplosionEntity>;
using Players = std::unordered_map<std::string, PlayerPtr>;

class BombManager {
public:
    BombManager(GameMapLogic& map) : map(map) {}
    bool update(const sf::Time&, sf::Packet&);
    bool check_damage(Players&, sf::Packet&);
    void create_bomb(const EntityCoords&, ServerPlayerEntity&);
private:
    Bombs bombs;
    Explosions explosions;
    GameMapLogic& map;

    int n_deployed_bombs = 0;
    int n_exploded_squares = 0;

    const float bomb_time = 1.5f;
    const float explosion_time = 0.5f;
};

#endif
