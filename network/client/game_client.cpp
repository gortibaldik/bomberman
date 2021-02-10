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
    std::cout << "Game ready socket loaded!" << std::endl;
    approved = true;
    sf::Packet answer;
    add_type_to_packet(answer, PacketType::ClientReady);
    send(answer);
}

void GameClient::update_player(sf::Packet& packet) {
    ClientPlayerEntity cpe(tm);
    packet >> cpe;
    auto it = players.find(cpe.name);
    if (it == players.end()) {
        map.transform(cpe.anim_object, cpe.actual_pos, false);
        players.emplace(cpe.name, cpe);
        if (cpe.name == player_name) {
            me = &players.at(cpe.name);
        }
        std::cout << cpe.name << " registered! <- client side" << std::endl;
    } else {
        it->second.actual_pos = cpe.actual_pos;
        it->second.direction = cpe.direction;
        map.transform(it->second.anim_object, it->second.actual_pos, false);
        it->second.anim_object.set_direction(cpe.direction);
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

void GameClient::server_state_update(sf::Packet& packet) {
    sf::Int8 delimiter;
    std::unique_lock<std::mutex> l(resources_mutex);
    while ((packet >> delimiter) && (delimiter == Network::Delimiter)) {
        sf::Int8 ptype = 0;
        packet >> ptype;
        switch(static_cast<PacketType>(ptype)) {
        case PacketType::ServerPlayerUpdate:
            update_player(packet);
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
        }
    }
}

void GameClient::fit_entities_to_window() {
    std::unique_lock<std::mutex> l(resources_mutex);
    for (auto&& player : players) {
        map.transform(player.second.anim_object, player.second.actual_pos);
    }
}

void GameClient::handle_others(sf::Packet& packet, PacketType ptype) {
    std::string token;
    switch(ptype) {
    case PacketType::GetReady:
        if (approved) { return; }
        get_ready(packet);
        break;
    case PacketType::SpawnPosition:
        if (!approved) { return; }
        game_started = true;
        server_state_update(packet);
        break;
    case PacketType::Update:
        if (!approved) { return; }
        server_state_update(packet);
        break;
    }
}

void GameClient::notify_disconnect() {
    approved = false;
    game_started = false;
}

void GameClient::render_entities(sf::RenderTarget* target) {
    std::unique_lock<std::mutex> l(resources_mutex);
    for (auto&& b : bombs) {
        target->draw(b.second.anim_object.get_sprite());
    }
    for (auto&& exp : explosions) {
        target->draw(exp.second.anim_object.get_sprite());
    }
    for (auto&& p : players) {
        target->draw(p.second.anim_object.get_sprite());
    }
}