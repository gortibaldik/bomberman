#include "game_server.hpp"
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <sstream>

GameServer::GameServer(const std::string& name_of_map)
                      : state(ServerState::WAITING_ROOM)
                      , map()
                      , bomb_manager(map) {
    try {
        map.load_from_config(name_of_map);
        max_clients = static_cast<int>(map.get_max_players());
    } catch (std::runtime_error& e) {
        std::stringstream ss;
        ss << "--- ERROR ---" << std::endl;
        ss << e.what() << std::endl;
        ss << "--- in GameServer::set_ready_game";
        throw std::runtime_error(ss.str());
    }
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

void GameServer::handle_starting_state(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    if (ptype == PacketType::ClientReady) {
        auto iter = players.find(client_name);
        if (iter != players.end()) { return; }
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        players.emplace(client_name, std::make_unique<ServerPlayerEntity>(client_name, coords, coords, EntityDirection::UP, type, player_lives, move_factor));
        std::cout << "SERVER : player " << client_name << " approved starting the game!" << std::endl;
    }
}

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
                auto d = static_cast<EntityDirection>(dir);
                if (p->reflect) {
                    row *= -1;
                    col *= -1;
                    switch (d) {
                    case EntityDirection::UP:
                        d = EntityDirection::DOWN; break;
                    case EntityDirection::DOWN:
                        d = EntityDirection::UP; break;
                    case EntityDirection::RIGHT:
                        d = EntityDirection::LEFT; break;
                    case EntityDirection::LEFT:
                        d = EntityDirection::RIGHT; break;
                    }
                }
                EntityCoords coords(row*p->move_factor+p->actual_pos.first, col*p->move_factor+p->actual_pos.second);
                map.collision_checking(p->move_factor, coords, d);
                p->update_pos_dir(std::move(coords), d);
                p->updated = true;
            }
            break;
        case PacketType::ClientDeployBomb:
            {
                std::unique_lock<std::mutex> l(players_mutex);
                auto it = players.find(client_name);
                if (it == players.end()) { return; }
                auto&& p = it->second;
                if (!p->can_deploy()) {
                    return;
                }
                p->deploy();
                bomb_manager.create_bomb(p->actual_pos, *p);
            }

        }
    }
}


void GameServer::set_ready_game() {
    if (state == ServerState::RUNNING) { return; }
    if (state == ServerState::WAITING_ROOM) {
        disable_adding_new_clients();
    }
    state = ServerState::STARTING;
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::GetReady);
    packet << map.get_name();
    broadcast(packet);
}

void GameServer::start_game() {
    // this function isn't synchronized, because
    // only main thread accesses players container
    // at the moment of starting the game
    if (state == ServerState::RUNNING) { return; }
    state = ServerState::RUNNING;
    sf::Packet p;
    // create the packet with all the player positions
    add_type_to_packet(p, PacketType::StartGame);
    for (auto&& player : players) {
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::SpawnPosition);
        p << *player.second;
    }
    // place simple ai to the game if it isn't full
    if (players.size() < max_clients) {
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        const std::string ai_name = "ai_escaper";
        players.emplace(ai_name, std::make_unique<AIEscaper>( ai_name
                                                            , coords
                                                            , coords
                                                            , EntityDirection::UP
                                                            , type
                                                            , player_lives
                                                            , move_factor * 1.8f));
        ais.emplace(ai_name, dynamic_cast<AIEscaper*>(players.at(ai_name).get()));
        ai_thread = std::thread([player = ais.at(ai_name)](){ player->update_loop(); });
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::SpawnPosition);
        p << *(players.at(ai_name));
    }
    // and all the positions of the soft blocks
    int i = -1;
    for (auto&& b : map.get_soft_blocks()) {
        i++;
        if (b) {
            p << sf::Int8(Network::Delimiter);
            add_type_to_packet(p, PacketType::ServerNotifySoftBlockExists);
            p << sf::Int32(i); /* ID of the soft block */
            sf::Int8 type(0); /* soft block is of 0 type, power ups are of types 1 to 3*/
            p << type;
        }
    }
    broadcast(p);
    notifier = std::thread([this]() { game_notify_loop(); });
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
            std::cout << "Updating " << p.first << std::endl;
            int power_up_id = 0;
            auto pu = map.is_on_power_up(p.second->actual_pos, power_up_id);
            if (pu != PowerUpType::NONE) {
                sf::Time duration = sf::seconds(10.f);
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerNotifyPowerUpDestroyed);
                packet << sf::Int32(power_up_id);
                packet << sf::Int8(static_cast<int>(pu));
                packet << p.second->name;
                if (pu == PowerUpType::REFLECT) {
                    for (auto&& victim : players) {
                        if (victim.first.compare(p.second->name) == 0) { continue; }
                        victim.second->apply_power_up(pu, duration);
                    }
                } else {
                    p.second->apply_power_up(pu, duration);
                }
            }
            p.second->update(time.asSeconds());
            if (p.second->updated) {
                p.second->updated = false;
                at_least_one = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerPlayerUpdate);
                packet << *p.second;
            }
        }
        at_least_one = bomb_manager.update(time, packet) || at_least_one;
        at_least_one = bomb_manager.check_damage(players, packet) || at_least_one;
        
        
        if (at_least_one && !end_notifier) {
            broadcast(packet);
        }

        // ais run in separate threads and aren't direct part
        // of the client server architecture, therefore we
        // treat them separately
        for (auto&& ai : ais) {
            ai.second->notify(packet);
        }

        if (players.size() == 1) {
            std::cout << "SERVER : only one player left!" << std::endl;
            sf::Packet game_ending_packet;
            add_type_to_packet(game_ending_packet, PacketType::ServerNotifyGameEnd);
            game_ending_packet << players.begin()->second->name;
            broadcast(game_ending_packet);
            end_notifier = true;
            sf::Time till_end = time;
            while ((time - till_end).asSeconds() <= 3.f) {
                sf::Packet ending_packet;
                std::this_thread::sleep_for(100ms);
                time += clock.restart();
                add_type_to_packet(ending_packet, PacketType::Update);
                at_least_one = bomb_manager.update(time, ending_packet);
                if (at_least_one) {
                    broadcast(ending_packet);
                }
            }
            break;
        }
    }
    while (players.size() > 0) {
        sf::Packet packet;
        add_type_to_packet(packet, PacketType::ServerNotifyPlayerDisconnect);
        packet << players.begin()->first;
        broadcast(packet);
        players.erase(players.begin());
    }
    terminate();
}
void GameServer::notify_disconnect(const std::string& client_name) {
    {
        std::unique_lock<std::mutex> l(players_mutex);
        auto iter = players.find(client_name);
        if (iter == players.end()) { return; }
        players.erase(client_name);
    }
    std::cout << "SERVER : Erased player " << client_name << std::endl;
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::ServerNotifyPlayerDisconnect);
    packet << client_name;
    broadcast(packet);
}

GameServer::~GameServer() {
    if (notifier.joinable()) {
        end_notifier = true;
        notifier.join();
        std::cout << "SERVER : joined notifier!" << std::endl;
    } else {
        std::cout << "SERVER : notifier already joined!" << std::endl;
    }
    if (ai_thread.joinable()) {
        ai_thread.join();
        std::cout << "SERVER : joined ai_thread!" << std::endl;
    } else {
        std::cout << "SERVER : ai_thread already joined!" << std::endl;
    }
}