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
    case ServerState::RUNNING:
        handle_running_state(client_name, packet, ptype);
        break;
    }
}

#define EXPLOSION_TIME 2.5f
#define EXPLOSION_VIEW_TIME 0.5f

void GameServer::handle_running_state(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    if (ptype != PacketType::Update) { return; }
    sf::Int8 t = 0;
    while ((packet >> t) && (t == Network::Delimiter)) {
        sf::Int8 type = 0;
        if (!(packet >> type)) { break; }
        switch(static_cast<PacketType>(type)) {
        case PacketType::ClientMove:
            {
                std::unique_lock<std::mutex> l(players_mutex);
                auto it = players.find(client_name);
                if (it == players.end()) { return; }
                auto&& p = it->second;
                sf::Int8 dir = 0, row = 0, col = 0;
                if(!(packet >> row >> col >> dir) ){
                    std::cout << "Invalid packet!" << std::endl;
                    return;
                }
                EntityCoords c(row*move_factor+p.actual_pos.first, col*move_factor+p.actual_pos.second);
                auto d = (EntityDirection::EntityDirection)dir;
                map.collision_checking(move_factor, c, d);
                p.update_pos_dir(std::move(c), d);
                p.updated = true;
            }
            break;
        case PacketType::ClientDeployBomb:
            {
                std::unique_lock<std::mutex> l(players_mutex);
                auto it = players.find(client_name);
                if (it == players.end()) { return; }
                auto&& p = it->second;
                if (!p.can_deploy()) {
                    return;
                }
                p.deploy();
                bombs.emplace(n_deployed_bombs, ServerBombEntity(p.actual_pos, EXPLOSION_TIME, n_deployed_bombs, p));
                n_deployed_bombs++;
            }

        }
    }
}

void GameServer::game_notify_loop() {
    using namespace std::chrono;
    sf::Clock clock;
    while(!end_notifier) {
        std::this_thread::sleep_for(100ms);
        std::unique_lock<std::mutex> l(players_mutex);
        bool at_least_one = false;
        sf::Packet packet;
        add_type_to_packet(packet, PacketType::Update);
        sf::Time time = clock.restart();
        for(auto&& p : players) {
            p.second.update(time.asSeconds());
            bool died = false;
            if (p.second.is_attackable()) {
                for (auto&& exp : explosions) {
                    if (naive_bbox_intersect(p.second.actual_pos, exp.second.actual_pos)) {
                        std::cout << p.first << " is hit!" << std::endl;
                        p.second.actual_pos = p.second.spawn_pos;
                        if (p.second.lives == 1) {
                            at_least_one = true;
                            packet << sf::Int8(Network::Delimiter);
                            add_type_to_packet(packet, PacketType::ServerNotifyPlayerDied);
                            packet << p.first;
                            players.erase(p.first);
                            died = true;
                        } else {
                            p.second.lives--;
                            p.second.respawn();
                            p.second.updated = true;
                        }
                        break;
                    }
                }
            }
            if (p.second.updated && !died) {
                p.second.updated = false;
                at_least_one = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerPlayerUpdate);
                packet << p.second;
            }
        }
        std::vector<int> bombs_to_erase;
        for (auto&&b : bombs) {
            b.second.update(time.asSeconds());
            if (b.second.is_new()) {
                at_least_one = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerNewBomb);
                packet << b.second;
            }
            if (b.second.can_explode()) {
                at_least_one = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerEraseBomb);
                packet << b.second;
                b.second.spe.remove_deployed();
                for (auto&& exp : b.second.explode(map, EXPLOSION_VIEW_TIME)) {
                    exp.ID = n_exploded_squares;
                    packet << sf::Int8(Network::Delimiter);
                    add_type_to_packet(packet, PacketType::ServerCreateExplosion);
                    packet << exp; 
                    explosions.emplace(n_exploded_squares, std::move(exp));
                    n_exploded_squares++;
                }
                bombs_to_erase.push_back(b.first);
            }
        }

        std::vector<int> explosions_to_erase;
        for (auto&& exp: explosions) {
            exp.second.update(time.asSeconds());
            if (exp.second.can_be_erased()) {
                at_least_one = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerEraseExplosion);
                packet << exp.second;
                explosions_to_erase.push_back(exp.first);
            }
        }
        for (auto&& i : bombs_to_erase) {
            bombs.erase(i);
        }

        for (auto&& i : explosions_to_erase) {
            explosions.erase(i);
        }
        
        if (at_least_one) {
            broadcast(packet);
        }
    }
    std::cout << "Notifier terminated!" << std::endl;
}

void GameServer::start_game() {
    // this function isn't synchronized, because
    // only main thread accesses players container
    if (state == ServerState::RUNNING) { return; }
    state = ServerState::RUNNING;
    sf::Packet p;
    // create the packet with all the plazer positions
    add_type_to_packet(p, PacketType::SpawnPosition);
    for (auto&& player : players) {
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::ServerPlayerUpdate);
        p << player.second;
    }
    broadcast(p);
    notifier = std::thread([this]() { game_notify_loop(); });
}

void GameServer::handle_starting_state(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    if (ptype == PacketType::ClientReady) {
        auto iter = players.find(client_name);
        if (iter != players.end()) { return; }
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        players.emplace(client_name, ServerPlayerEntity(client_name, coords, coords, EntityDirection::UP, type, player_lives));
        std::cout << "player " << client_name << " approved starting the game!" << std::endl;
    }
}

void GameServer::notify_disconnect(const std::string& client_name) {
    {
        std::unique_lock<std::mutex> l(players_mutex);
        auto iter = players.find(client_name);
        if (iter == players.end()) { return; }
        players.erase(client_name);
    }
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::ServerNotifyPlayerDisconnect);
    broadcast(packet);
}

GameServer::~GameServer() {
    if (notifier.joinable()) {
        end_notifier = true;
        notifier.join();
    }
}