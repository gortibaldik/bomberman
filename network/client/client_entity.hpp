#ifndef GAME_CLIENT_ENTITY_HPP
#define GAME_CLIENT_ENTITY_HPP

#include "game/entity.hpp"
#include "texture_handling/texture_manager.hpp"
#include <SFML/Network.hpp>

class ClientPlayerEntity: public PlayerEntity {
public:
    ClientPlayerEntity(const TextureManager& tm)
                      : tm(tm)
                      , anim_object(tm.get_anim_object("p1")) {}    
    ClientPlayerEntity(const std::string& name
                      , EntityCoords spawn_pos
                      , EntityCoords actual_pos
                      , EntityDirection::EntityDirection direction
                      , AnimObject anim_object
                      , const TextureManager& tm)
                      : PlayerEntity(name, actual_pos, direction)
                      , anim_object(anim_object)
                      , tm(tm) {}
    AnimObject anim_object;
    const TextureManager& tm;
};

class ClientBombEntity: public Entity {
public:
    ClientBombEntity(const TextureManager& tm)
                    : tm(tm)
                    , anim_object(tm.get_anim_object("bomb_no_explosion"))
                    , ID(0) {}
    AnimObject anim_object;
    sf::Int32 ID;
    const TextureManager& tm;
};

sf::Packet& operator >>(sf::Packet&, ClientPlayerEntity&);
sf::Packet& operator >>(sf::Packet&, ClientBombEntity&);

#endif
