#ifndef GAME_AI_ENTITY_ESCAPER_HPP
#define GAME_AI_ENTITY_ESCAPER_HPP

#include "server_entity.hpp"

// The basic AI, doesn't want to die
class AIEscaper: public ServerPlayerEntity {
    using ServerPlayerEntity::ServerPlayerEntity;
};

#endif