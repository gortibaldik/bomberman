#ifndef GAME_AI_ENTITY_ESCAPER_HPP
#define GAME_AI_ENTITY_ESCAPER_HPP

#include "server_entity.hpp"

// The basic AI, doesn't want to die
class AIEscaper: public ServerPlayerEntity {
public:
    AIEscaper(const std::string& name
             , EntityCoords spawn_pos
             , EntityCoords actual_pos
             , EntityDirection direction
             , sf::Int8 type
             , int lives
             , float move_factor)
             : ServerPlayerEntity( name
                                 , spawn_pos
                                 , actual_pos
                                 , direction
                                 , type
                                 , lives
                                 , move_factor) {}
    void update_loop();
    void notify(sf::Packet& packet);
    void update(float dt) override;
    float review_time = 0.f;
};

#endif