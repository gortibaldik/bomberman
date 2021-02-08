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

void GameClient::server_state_update(sf::Packet& packet) {
    sf::Int8 delimiter;
    std::unique_lock<std::mutex> l(resources_mutex);
    while ((packet >> delimiter) && (delimiter == Network::Delimiter)) {
        ClientPlayerEntity cpe(tm);
        packet >> cpe;
        auto it = players.find(cpe.name);
        if (it == players.end()) {
            map.transform(cpe.anim_object, cpe.actual_pos);
            players.emplace(cpe.name, cpe);
            std::cout << cpe.name << " registered! <- client side" << std::endl;
        } else {
            it->second.actual_pos = cpe.actual_pos;
            it->second.direction = cpe.direction;
            it->second.anim_object = std::move(cpe.anim_object);
            map.transform(it->second.anim_object, it->second.actual_pos);
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
        std::cout << "Gonna start the game!" << std::endl;
        game_started = true;
        server_state_update(packet);
        break;
    }
}

void GameClient::notify_disconnect() {
    approved = false;
    game_started = false;
}

void GameClient::render_players(sf::RenderTarget* target) {
    for (auto&& p : players) {
        target->draw(p.second.anim_object.get_sprite());
    }
}