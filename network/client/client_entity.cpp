#include "client_entity.hpp"
#include <unordered_map>
#include <iostream>

sf::Packet& operator >>(sf::Packet& packet, ClientPlayerEntity& cpe) {
    packet >> cpe.name;
    sf::Int8 direction = 0;
    packet >> direction;
    cpe.direction = static_cast<EntityDirection>(direction);
    sf::Int8 lives = 0;
    packet >> lives;
    cpe.lives = lives;
    sf::Int8 player_token_type = 0;
    // token is a type of player object
    // now it's just a mockup the functionality
    // will be implemented later
    packet >> player_token_type;
    packet >> cpe.actual_pos.first;
    packet >> cpe.actual_pos.second;
    return packet;
}

#define SPACING_FACTOR 1.5f

static float add_const_speed(float dt, float new_x, float actual_x) {
    if (new_x > actual_x) {
        float diff = new_x - actual_x;
        if (dt > diff) {
            return new_x;
        } else {
            return actual_x + dt;
        }
    } else {
        float diff = actual_x - new_x;
        if (dt > diff) {
            return new_x;
        } else {
            return actual_x - dt;
        }
    }
}

bool ClientPlayerEntity::update(float dt) {
    if (is_spawned) {
        actual_pos = new_pos;
        is_spawned = false;
        return true;
    }
    if (actual_pos == new_pos) { return false; }
    float speed = 5.5f * dt;
    anim_object.update(dt);
    actual_pos.first = add_const_speed(speed, new_pos.first, actual_pos.first);
    actual_pos.second = add_const_speed(speed, new_pos.second, actual_pos.second);
    return true;
}

/* Updates the position of all the entities attached to
 * the current player entity to the actual position
 */
void ClientPlayerEntity::move_to_actual_position(const GameMapRenderable& map) {
    map.transform(anim_object, actual_pos, false);
    anim_object.set_direction(direction);
    auto&& rectangle = anim_object.get_global_bounds();
    auto width = rectangle.width;
    auto&& text_rectangle = player_name_renderable.getGlobalBounds();
    auto text_width = text_rectangle.width;
    auto text_height = text_rectangle.height;
    auto&& heart_rectangle = hearts[0].get_global_bounds();
    auto heart_width = heart_rectangle.width;
    auto heart_height = heart_rectangle.height;
    auto hearts_width = heart_width * hearts.size();
    for (int i = 0; i < hearts.size(); i++) {
        hearts[i].set_position(rectangle.left + width / 2 - (hearts_width - i*2*heart_width) / 2, rectangle.top - SPACING_FACTOR * heart_height);
    }
    player_name_renderable.setPosition(rectangle.left + width / 2 - text_width / 2, rectangle.top - SPACING_FACTOR * text_rectangle.height
                                                                                                  - SPACING_FACTOR * heart_height);
}

void ClientPlayerEntity::render(sf::RenderTarget* target) {
    target->draw(anim_object.get_sprite());
    target->draw(player_name_renderable);
    for (auto&& heart : hearts) {
        target->draw(heart.get_sprite());
    }
}

void ClientPlayerEntity::update_hearts(int lives) {
    this->lives = lives;
    hearts.clear();
    for (int i = 0; i < lives; i++) {
        hearts.push_back(tm.get_anim_object("heart"));
    }
}

sf::Packet& operator >>(sf::Packet& packet, ClientBombEntity& cbe) {
    packet >> cbe.ID;
    packet >> cbe.actual_pos.first;
    return packet >> cbe.actual_pos.second;
}

ClientExplosionEntity ClientExplosionEntity::extract_from_packet(const TextureManager& tm, sf::Packet& packet) {
    const std::unordered_map<ExplosionType, std::string> exp_to_str = {
        {ExplosionType::CENTER,             "exp_center"},
        {ExplosionType::HORIZONTAL_LEFT,    "exp_horizontal"},
        {ExplosionType::HORIZONTAL_RIGHT,   "exp_horizontal"},
        {ExplosionType::VERTICAL_DOWN,      "exp_vertical"},
        {ExplosionType::VERTICAL_UP,        "exp_vertical"},
        {ExplosionType::RIGHT_END,          "exp_right"},
        {ExplosionType::LEFT_END,           "exp_left"},
        {ExplosionType::UP_END,             "exp_up"},
        {ExplosionType::DOWN_END,           "exp_down"}
    };
    sf::Int32 id = 0;
    sf::Int8 type = 0;
    float f = 0.f, s = 0.f;
    packet >> id >> type >> f >> s;
    auto cee = ClientExplosionEntity(tm, exp_to_str.at(static_cast<ExplosionType>(type)), id);
    cee.actual_pos = EntityCoords(f, s);
    return cee;
}