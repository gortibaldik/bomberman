#ifndef GAME_AI_ENTITY_ESCAPER_HPP
#define GAME_AI_ENTITY_ESCAPER_HPP

#include "server_entity.hpp"
#include "bomb_manager.hpp"

// The basic AI, doesn't want to die
class AIEscaper: public ServerPlayerEntity {
public:
    AIEscaper(const std::string& name
             , EntityCoords spawn_pos
             , EntityCoords actual_pos
             , EntityDirection direction
             , sf::Int8 type
             , int lives
             , float move_factor
             , const GameMapLogic& map
             , const BombManager& bomb_manager
             , const Players& players)
             : ServerPlayerEntity( name
                                 , spawn_pos
                                 , actual_pos
                                 , direction
                                 , type
                                 , lives
                                 , move_factor)
             , map(map)
             , bomb_manager(bomb_manager)
             , players(players) {}
    void update(float dt) override;
    std::vector<EntityDirection> find_bomb_free_views();
    std::vector<EntityDirection> find_player_free_views();
    const GameMapLogic& map;
    const BombManager& bomb_manager;
    const Players& players;
    float review_time = 0.f;
};

#endif