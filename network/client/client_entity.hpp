#ifndef GAME_CLIENT_ENTITY_HPP
#define GAME_CLIENT_ENTITY_HPP

#include "game/entity.hpp"
#include "texture_handling/texture_manager.hpp"
#include <SFML/Network.hpp>

class ClientPlayerEntity: public PlayerEntity {
public:
    ClientPlayerEntity( const TextureManager& tm
                      , const sf::Font& font)
                      : tm(tm)
                      , anim_object(tm.get_anim_object("p1"))
                      , player_name_renderable("", font, 20) {
        player_name_renderable.setFillColor(sf::Color::White);
        
    }    
    ClientPlayerEntity( const std::string& name
                      , EntityCoords spawn_pos
                      , EntityCoords actual_pos
                      , EntityDirection::EntityDirection direction
                      , AnimObject anim_object
                      , const TextureManager& tm
                      , const sf::Font& font)
                      : PlayerEntity(name, actual_pos, direction)
                      , anim_object(anim_object)
                      , tm(tm)
                      , player_name_renderable(name, font) {}
    void update_position();
    AnimObject anim_object;
    sf::Text player_name_renderable;
    
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

class ClientExplosionEntity : public Entity {
public:
    ClientExplosionEntity(const TextureManager& tm
                         , const std::string& sprite_name
                         , sf::Int32 ID)
                         : tm(tm)
                         , ID(ID)
                         , anim_object(tm.get_anim_object(sprite_name)) {}
    AnimObject anim_object;
    sf::Int32 ID;
    const TextureManager& tm;
    static ClientExplosionEntity extract_from_packet(const TextureManager& tm, sf::Packet&);
};

sf::Packet& operator >>(sf::Packet&, ClientPlayerEntity&);
sf::Packet& operator >>(sf::Packet&, ClientBombEntity&);

#endif
