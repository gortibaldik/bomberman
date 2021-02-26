#ifndef GAME_AI_ENTITY_ESCAPER_HPP
#define GAME_AI_ENTITY_ESCAPER_HPP

#include "server_entity.hpp"
#include "game/map_logic.hpp"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

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
             , float server_bomb_explosion_time)
             : ServerPlayerEntity( name
                                 , spawn_pos
                                 , actual_pos
                                 , direction
                                 , type
                                 , lives
                                 , move_factor)
             , map(map)
             , next_move(EntityDirection::UP)
             , updater([this](){ update_loop(); })
             , server_bomb_explosion_time(server_bomb_explosion_time)
             , is_running(true)
             , new_pos_calculated(false) {}
    ~AIEscaper();
    void respawn() override ;
    void update_pos_dir(EntityCoords&&, EntityDirection) override ;
    void apply_power_up(PowerUpType, const sf::Time&) override ;
    void update_loop();
    void notify_new_bomb(BombInfo&);
    void notify_sb_destroyed(int i);
    void update(float dt) override;
    void terminate();
private:
    float server_bomb_explosion_time;
    void BFS();
    EntityDirection next_move;
    GameMapLogic map;
    std::atomic<bool> is_running;
    std::atomic<bool> new_pos_calculated;
    std::mutex resources_m, cond_m, solution_m;
    std::vector<std::thread> workers;
    std::thread updater;
    std::condition_variable cond, solution_found;
};

#endif
