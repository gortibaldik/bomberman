#include "game_client.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

void GameClient::get_ready(sf::Packet& packet) {
    std::string file_name;
    if (!(packet >> file_name)) {
        std::cout << "Invalid file_name: " << file_name << std::endl;
        return;
    }
    try {
        std::unique_lock<std::mutex> l(resources_mutex);
        map.load_from_config(file_name);
    } catch (std::runtime_error& e) {
        std::stringstream ss;
        ss << "--- ERROR ---" << std::endl;
        ss << e.what();
        ss << "--- GameClient::get_ready" << std::endl;
        throw std::runtime_error(ss.str());
    }
    std::cout << "CLIENT : game ready socket loaded!" << std::endl;
    approved = true;
    sf::Packet answer;
    add_type_to_packet(answer, PacketType::ClientReady);
    send(answer);
}

void GameClient::update_player(sf::Packet& packet, bool spawn) {
    ClientPlayerEntity cpe(tm);
    packet >> cpe;
    auto it = players.find(cpe.name);
    if (it == players.end()) {
        cpe.anim_object = tm.get_anim_object("p" + std::to_string(cpe.type));
        // players shall be scaled during fitting of the window
        // at the receiving the initial socket with all the info
        // about the game, thus they aren't scaled afterwards
        map.transform(cpe.anim_object, cpe.actual_pos, false);
        cpe.update_hearts(cpe.lives);
        cpe.player_name_renderable.setFont(font);
        cpe.player_name_renderable.setFillColor(sf::Color::White);
        cpe.player_name_renderable.setCharacterSize(20);
        cpe.player_name_renderable.setString(cpe.name);
        cpe.new_pos = cpe.actual_pos;
        //cpe.move_to_actual_position(map);
        players.emplace(cpe.name, cpe);
        if (cpe.name == player_name) {
            me = &players.at(cpe.name);
        }
        std::cout << cpe.name << " registered! <- client side" << std::endl;
    } else {
        it->second.new_pos = cpe.actual_pos;
        it->second.direction = cpe.direction;
        if (it->second.lives != cpe.lives) {
            it->second.update_hearts(cpe.lives);
        }
        if (spawn) {
            it->second.actual_pos = cpe.actual_pos;
            it->second.spawn();
            received_messages.enqueue("player " + it->first + " was hit!");
        }
    }
}

void GameClient::create_bomb(sf::Packet& packet) {
    ClientBombEntity cbe(tm);
    packet >> cbe;
    auto it = bombs.find(cbe.ID);
    if (it == bombs.end()) {
        map.transform(cbe.anim_object, cbe.actual_pos, true);
        bombs.emplace(cbe.ID, cbe);
    }
}

void GameClient::erase_bomb(sf::Packet& packet) {
    ClientBombEntity cbe(tm);
    packet >> cbe;
    auto it = bombs.find(cbe.ID);
    if (it != bombs.end()) {
        bombs.erase(cbe.ID);
    }
}

void GameClient::create_explosion(sf::Packet& packet) {
    auto cee = ClientExplosionEntity::extract_from_packet(tm, packet);
    auto it = explosions.find(cee.ID);
    if (it == explosions.end()) {
        map.transform(cee.anim_object, cee.actual_pos, true);
        explosions.emplace(cee.ID, cee);
    }
}

void GameClient::erase_explosion(sf::Packet& packet) {
    auto cee = ClientExplosionEntity::extract_from_packet(tm, packet);
    auto it = explosions.find(cee.ID);
    if (it != explosions.end()) {
        explosions.erase(cee.ID);
    }
}

void GameClient::erase_player(sf::Packet& packet) {
    std::string name = "";
    packet >> name;
    if (this->player_name.compare(name) == 0) {
        only_viewer = true;
    }
    players.erase(name);
    received_messages.enqueue("player " + name + " died!");
}

void GameClient::create_soft_block(sf::Packet& packet) {
    sf::Int32 i = 0;
    packet >> i;
    auto it = soft_blocks.find(i);
    if (it == soft_blocks.end()) {
        soft_blocks.emplace(i, tm.get_anim_object("soft_block"));
        map.transform(soft_blocks.at(i).anim_object, map.transform_to_coords(i), false);
    }
}

void GameClient::destroy_soft_block(sf::Packet& packet) {
    sf::Int32 i = 0;
    packet >> i;
    auto it = soft_blocks.find(i);
    if (it != soft_blocks.end()) {
        soft_blocks.erase(i);
    }
}

void GameClient::server_state_update(sf::Packet& packet) {
    sf::Int8 delimiter;
    std::unique_lock<std::mutex> l(resources_mutex);
    while ((packet >> delimiter) && (delimiter == Network::Delimiter)) {
        sf::Int8 ptype = 0;
        packet >> ptype;
        switch(static_cast<PacketType>(ptype)) {
        case PacketType::SpawnPosition:
            update_player(packet, true);
            break;
        case PacketType::ServerPlayerUpdate:
            update_player(packet, false);
            break;
        case PacketType::ServerNewBomb:
            create_bomb(packet);
            break;
        case PacketType::ServerEraseBomb:
            erase_bomb(packet);
            break;
        case PacketType::ServerCreateExplosion:
            create_explosion(packet);
            break;
        case PacketType::ServerEraseExplosion:
            erase_explosion(packet);
            break;
        case PacketType::ServerNotifyPlayerDied:
            erase_player(packet);
            break;
        case PacketType::ServerNotifySoftBlockExists:
            create_soft_block(packet);
            break;
        case PacketType::ServerNotifySoftBlockDestroyed:
            destroy_soft_block(packet);
            break;
        }
    }
}

void GameClient::fit_entities_to_window() {
    std::unique_lock<std::mutex> l(resources_mutex);
    for (auto&& player : players) {
        map.transform(player.second.anim_object, player.second.actual_pos);
    }
    int i = -1;
    for (auto&& s_b : soft_blocks) {
        i++;
        map.transform(s_b.second.anim_object, map.transform_to_coords(s_b.first));
    }
}

void GameClient::handle_others(sf::Packet& packet, PacketType ptype) {
    std::string token;
    switch(ptype) {
    case PacketType::GetReady:
        if (approved) { return; }
        get_ready(packet);
        break;
    case PacketType::StartGame:
        if (!approved) { return; }
        game_started = true;
        server_state_update(packet);
        break;
    case PacketType::Update:
        if (!approved) { return; }
        server_state_update(packet);
        break;
    case PacketType::ServerNotifyPlayerDisconnect:
        packet >> token;
        std::cout << "CLIENT : got ServerNotifyPlayerDisconnect from " << token << std::endl;
        {
            std::unique_lock<std::mutex> l(resources_mutex);
            players.erase(token);
        }
        received_messages.enqueue("client " + token + " disconnected");
        break;
    }
}

void GameClient::notify_disconnect() {
    approved = false;
    game_started = false;
}

void GameClient::render_entities(sf::RenderTarget* target) {
    std::unique_lock<std::mutex> l(resources_mutex);
    for (auto&& s_b : soft_blocks) {
        target->draw(s_b.second.anim_object.get_sprite());
    }
    for (auto&& b : bombs) {
        target->draw(b.second.anim_object.get_sprite());
    }
    for (auto&& exp : explosions) {
        target->draw(exp.second.anim_object.get_sprite());
    }
}

void GameClient::render_players(sf::RenderTarget* target) {
    std::unique_lock<std::mutex> l(resources_mutex);
    for (auto&& p : players) {
        p.second.render(target);
    }
}

#define STALL_TIME 0.1f

void GameClient::update(float dt) {
    for (auto&& bomb : bombs) {
        bomb.second.anim_object.update(dt);
    }
    for (auto&& player : players) {
        if (player.second.update(dt)) {
            player.second.move_to_actual_position(map);
        } else {
            player.second.stall_time += sf::seconds(dt);
        }
        // if player isn't walking for long enough, set its animation
        // to idle default animation
        if (player.second.stall_time.asSeconds() >= STALL_TIME) {
            player.second.anim_object.set_default();
        }
    }
}

GameClient::~GameClient() {
    terminate();
}