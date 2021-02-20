#ifndef GAME_AI_ENTITY_ESCAPER_HPP
#define GAME_AI_ENTITY_ESCAPER_HPP

#include "server_entity.hpp"
#include "game/map_logic.hpp"
#include <atomic>

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
             , const GameMapLogic& map)
             : ServerPlayerEntity( name
                                 , spawn_pos
                                 , actual_pos
                                 , direction
                                 , type
                                 , lives
                                 , move_factor)
             , map(map) {}
    void update_loop();
    void notify_new_bomb(const IDPos&);
    void notify_sb_destroyed(int i);
    void update(float dt) override;
    void terminate() { is_running = false; }
    float review_time = 0.f;
private:
    void BFS();
    GameMapLogic map;
    std::atomic<bool> is_running = true;
};

#endif