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
        break;
    }
}

void GameClient::notify_disconnect() {
    approved = false;
    game_started = false;
}