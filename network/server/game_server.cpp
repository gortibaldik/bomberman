#include "game_server.hpp"
#include <unordered_map>
#include <stdexcept>
#include <iostream>


void GameServer::set_ready_game(const std::string& name_of_map) {
    if (state == ServerState::RUNNING) { return; }
    if (state == ServerState::WAITING_ROOM) {
        try {
            map.load_from_config(name_of_map);
        } catch (std::runtime_error& e) {
            std::cout << "--- ERROR ---" << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << "--- in GameServer::set_ready_game";
            throw std::runtime_error("");
        }
        state = ServerState::STARTING;
        disable_adding_new_clients();
    }
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::GetReady);
    packet << name_of_map;
    std::cout << "Broadcasting GetReady packet!" << std::endl;
    broadcast(packet);
}

void GameServer::handle_others(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    switch(state) {
    case ServerState::WAITING_ROOM:
        // in the waiting room, don't serve any other than connection and heartbeat packets
        return;
    case ServerState::STARTING:
        handle_starting_state(client_name, packet, ptype);
        break;
    }
}

void GameServer::start_game() {
    state = ServerState::RUNNING;
    sf::Packet p;
    add_type_to_packet(p, PacketType::SpawnPosition);
    for (auto&& player : players) {
        p << sf::Int8(Network::Delimiter);
        p << player.second;
    }
    for (auto&& player : players) {
        send(player.second.name, p);
    }
}

void GameServer::handle_starting_state(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    if (ptype == PacketType::ClientReady) {
        auto iter = players.find(client_name);
        if (iter != players.end()) { return; }
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        players.emplace(client_name, ServerPlayerEntity(client_name, coords, coords, EntityDirection::UP, type));
        std::cout << "player " << client_name << " approved starting the game!" << std::endl;
    }
}

void GameServer::notify_disconnect(const std::string& client_name) {
    auto iter = players.find(client_name);
    if (iter == players.end()) { return; }
    players.erase(client_name);
}