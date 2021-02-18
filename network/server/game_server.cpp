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
                EntityCoords c(row*p->move_factor+p->actual_pos.first, col*p->move_factor+p->actual_pos.second);
                auto d = static_cast<EntityDirection>(dir);
                map.collision_checking(p->move_factor, c, d);
                p->update_pos_dir(std::move(c), d);
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
    if (players.size() < 4) {
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        auto player = 
        players.emplace("ai_escaper", std::make_unique<AIEscaper>( "ai_escaper"
                                                                 , coords
                                                                 , coords
                                                                 , EntityDirection::UP
                                                                 , type
                                                                 , player_lives
                                                                 , move_factor
                                                                 , map
                                                                 , bomb_manager
                                                                 , players));
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::SpawnPosition);
        p << *(players.at("ai_escaper"));
    }
    // and all the positions of the soft blocks
    int i = -1;
    for (auto&& b : map.get_soft_blocks()) {
        i++;
        if (b) {
            p << sf::Int8(Network::Delimiter);
            add_type_to_packet(p, PacketType::ServerNotifySoftBlockExists);
            p << sf::Int32(i);
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
    }
    while (players.size() > 0) {
        sf::Packet packet;
        add_type_to_packet(packet, PacketType::ServerNotifyPlayerDisconnect);
        packet << players.begin()->first;
        broadcast(packet);
        players.erase(players.begin());
    }
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
}