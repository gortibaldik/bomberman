#include "game_server.hpp"
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <sstream>

#define TIME_TO_EXPLODE 1.5f

GameServer::GameServer(const std::string& name_of_map)
                      : state(ServerState::WAITING_ROOM)
                      , map()
                      , end_notifier(false) {
    try {
        map.load_from_config(name_of_map);
        max_clients = static_cast<int>(map.get_max_players());
    } catch (std::runtime_error& e) {
        std::stringstream ss;
        ss << "--- ERROR ---" << std::endl;
        ss << e.what() << std::endl;
        ss << "--- in GameServer::ctor";
        throw std::runtime_error(ss.str());
    }
}

void GameServer::listener_handle_others(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
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
    case ServerState::END_GAME:
        handle_ending_state(client_name, packet, ptype);
        return;
    }
}

void GameServer::handle_starting_state(const std::string& client_name, sf::Packet& packet, PacketType ptype) {
    if (ptype == PacketType::ClientReady) {
        auto iter = players.find(client_name);
        if (iter != players.end()) { return; }
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        std::cout << "SERVER : emplacing " << client_name << std::endl;
        ScoreInfo si;
        auto it = players_scores.find(client_name);
        if (it != players_scores.end()) {
            si = it->second;
        }
        players.emplace( client_name
                       , std::make_unique<ServerPlayerEntity>( client_name
                                                             , coords
                                                             , coords
                                                             , EntityDirection::UP
                                                             , type
                                                             , player_lives
                                                             , move_factor
                                                             , si ));
        std::cout << "SERVER : player " << client_name << " approved starting the game!" << std::endl;
    } else if (ptype == PacketType::ClientMainStart) {
        std::cout << "SERVER : starting the game because of the order from " << client_name << std::endl;
        start_game();
    }
}

void GameServer::handle_running_state( const std::string& client_name
                                     , sf::Packet& packet
                                     , PacketType ptype) {
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
                if(!(packet >> dir) ){
                    std::cout << "Invalid packet!" << std::endl;
                    return;
                }
                auto direction = static_cast<EntityDirection>(dir);
                if (p->reflect) {
                    direction = opposite(direction);
                }
                EntityCoords next_position = p->actual_pos;
                go(next_position,direction, p->move_factor);
                map.collision_checking(p->move_factor, next_position, direction);
                p->update_pos_dir(std::move(next_position), direction);
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
                map.place_bomb(p->actual_pos, *p, TIME_TO_EXPLODE);
            }

        }
    }
}

void GameServer::handle_ending_state( const std::string& client_name
                                    , sf::Packet& packet
                                    , PacketType ptype) {
    switch (ptype) {
    case PacketType::ClientMainSetReady:
        std::cout << "SERVER : main client(" << client_name << ") wants to restart the game!" << std::endl;
        map.load_from_config(map.get_name());
        set_ready_game();
        break;
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
    std::cout << "SERVER : Notifying about all the players!" << std::endl;
    add_type_to_packet(p, PacketType::StartGame);
    for (auto&& player : players) {
        std::cout << "SERVER : Notifying about " << player.first << std::endl;
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::SpawnPosition);
        p << *player.second;
    }
    // place simple ai to the game if it isn't full
    int ai_number = 0;
    std::cout << "SERVER : Creating ais" << std::endl;
    while (players.size() < max_clients) {
        auto [row, column, type] = map.get_spawn_pos();
        std::pair<int, int> coords(row, column);
        std::string ai_name = "ai_escaper_" + std::to_string(ai_number);
        while (players.find(ai_name) != players.end()) {
            ai_name = "ai_escaper_" + std::to_string(++ai_number);
        }
        std::cout << "SERVER : emplacing " << ai_name << std::endl;
        ScoreInfo si;
        auto it = players_scores.find(ai_name);
        if (it != players_scores.end()) {
            si = it->second;
        }
        players.emplace(ai_name, std::make_unique<AIEscaper>( ai_name
                                                            , coords
                                                            , coords
                                                            , EntityDirection::UP
                                                            , type
                                                            , player_lives
                                                            , move_factor * 1.8f
                                                            , si
                                                            , map
                                                            , TIME_TO_EXPLODE));
        ais.emplace(ai_name, dynamic_cast<AIEscaper*>(players.at(ai_name).get()));
        ais.at(ai_name)->start();
        p << sf::Int8(Network::Delimiter);
        add_type_to_packet(p, PacketType::SpawnPosition);
        p << *(players.at(ai_name));
        ai_number++;
    }
    // and all the positions of the soft blocks
    int i = -1;
    std::cout << "SERVER : Getting the soft blocks" << std::endl;
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
    std::cout << "SERVER : created everything possible!" << std::endl;
    if (notifier.joinable()) { 
        std::cout << "SERVER : needed to join notifier before start!" << std::endl;
        notifier.join();
    }
    end_notifier = false;
    notifier = std::thread([this]() { game_notify_loop(); });
}

static void add_id_pos_to_packet( sf::Packet& packet
                                , const GameMapLogic& map
                                , const IDPos& idp) {
    packet << sf::Int32(idp.first); /* id */
    auto coords = map.transform_to_coords(idp.second);
    packet << static_cast<float>(coords.first); /* pos.row */
    packet << static_cast<float>(coords.second); /* pos.col */
}

static void add_id_pos_to_packet( sf::Packet& packet
                                , const GameMapLogic& map
                                , const BombInfo& bi) {
    packet << sf::Int32(bi.first); /* id */
    packet << bi.second.first.first; /* pos.row */
    packet << bi.second.first.second; /* pos.col */
}

bool GameServer::update_bombs_explosions(float dt, sf::Packet& packet) {
    IDPosVector erased_bombs, erased_explosions;
    BombVector new_bombs;
    IDPosTypeVector new_explosions;
    map.update(dt, erased_bombs, erased_explosions, new_bombs, new_explosions);
    bool result;
    for (auto&& bomb : new_bombs) {
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerNewBomb);
        add_id_pos_to_packet(packet, map, bomb);
        // notify all the agents about the new bomb
        for (auto&& ai : ais) {
            ai.second->notify_new_bomb(bomb);
        }
        result = true;
    }
    for (auto&& exp : new_explosions) {
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerCreateExplosion);
        packet << sf::Int8(exp.second); /* add type of explosion to the packet */
        add_id_pos_to_packet(packet, map, exp.first);
        result = true;
    }
    for (auto&& bomb : erased_bombs) {
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerEraseBomb);
        packet << sf::Int32(bomb.first); /* ID of the erased bomb */
        result = true;
    }
    for (auto&& exp : erased_explosions) {
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerEraseExplosion);
        packet << sf::Int32(exp.first); /* ID of the erased explosion */
        result = true;
    }
    return result;
}

/* Update player score and add the entry to
 * players_scores table
 *  WARNING : unsafe, doesn't check if the player exists
 */
void GameServer::handle_player_die_event(const std::string& player_name) {
    auto&& player = players.at(player_name);
    auto ssi = player->score;
    ssi.update_ds(max_clients - players.size());
    players_scores.emplace(player_name, ssi);
}

bool GameServer::update_players_damage(sf::Packet& packet) {
    bool result = false;
    std::vector<std::string> to_erase;
    for (auto&& player: players) {
        if (!map.check_damage(player.second->actual_pos)) { continue; }
        if (!player.second->is_attackable()) { continue; }
        /* someone must be the winner, 
         * he shouldn't die 
         */
        if ((players.size() - to_erase.size()) == 1) { continue; } 
        result = true;
        if (player.second->lives == 1) {
            packet << sf::Int8(Network::Delimiter);
            add_type_to_packet(packet, PacketType::ServerNotifyPlayerDied);
            packet << player.first;
            auto it = ais.find(player.first);
            if (it != ais.end()) {
                ais.at(player.first)->terminate();
                ais.erase(player.first);
            }
            handle_player_die_event(player.first);
            to_erase.emplace_back(player.first);
        } else {
            player.second->lives--;
            player.second->respawn();
            packet << sf::Int8(Network::Delimiter);
            add_type_to_packet(packet, PacketType::SpawnPosition);
            packet << *player.second;
        }
    }
    for (auto&& str : to_erase) {
        players.erase(str);
    }
    return result;
}

bool GameServer::update_soft_blocks(sf::Packet& packet) {
    bool result = false;
    for (auto&& sb : map.check_soft_blocks()) {
        result = true;
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerNotifySoftBlockDestroyed);
        packet << sf::Int32(sb.first); /* identification of the soft block to erase on the client side */
        packet << sf::Int8(sb.second); /* type of the power up hidden behind the soft block (0 in case of none) */
        for (auto&& ai : ais) {
            ai.second->notify_sb_destroyed(sb.first);
        }
    }
    return result;
}

bool GameServer::update_player( const sf::Time& time
                              , ServerPlayerEntity& player
                              , sf::Packet& packet) {
    bool result = false;
    int power_up_id = 0;
    auto pu = map.is_on_power_up(player.actual_pos, power_up_id);
    if (pu != PowerUpType::NONE) {
        result = true;
        sf::Time duration = sf::seconds(10.f);
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerNotifyPowerUpDestroyed);
        packet << sf::Int32(power_up_id);
        packet << sf::Int8(static_cast<int>(pu));
        packet << player.name;
        if (pu == PowerUpType::REFLECT) {
            for (auto&& victim : players) {
                if (victim.first.compare(player.name) == 0) { continue; }
                victim.second->apply_power_up(pu, duration);
            }
        } else {
            player.apply_power_up(pu, duration);
        }
    }
    player.update(time.asSeconds());
    if (player.updated) {
        // this situation happens only if player wants to
        // place a new bomb => and only for ais
        if (player.direction == EntityDirection::STATIC) {
            map.place_bomb(player.actual_pos, player, TIME_TO_EXPLODE);
            return result;
        }
        result = true;
        player.updated = false;
        packet << sf::Int8(Network::Delimiter);
        add_type_to_packet(packet, PacketType::ServerPlayerUpdate);
        packet << player;
    }
    return result;
}

void GameServer::game_end_notify_loop( sf::Time& time
                                     , sf::Clock& clock) {
    using namespace std::chrono;
    sf::Packet game_ending_packet;
    add_type_to_packet(game_ending_packet, PacketType::ServerNotifyGameEnd);
    game_ending_packet << players.begin()->second->name;
    broadcast(game_ending_packet);
    sf::Time till_end = time;
    for (auto&& ai : ais) {
        std::cout << "SERVER : terminating " << ai.second->name << std::endl;
        ai.second->terminate();
    }
    while ((time - till_end).asSeconds() <= 3.f) {
        sf::Packet packet;
        std::this_thread::sleep_for(100ms);
        add_type_to_packet(packet, PacketType::Update);
        bool at_least_one = false;
        auto dt = clock.restart();
        time += dt;
        at_least_one = update_bombs_explosions(dt.asSeconds(), packet);
        at_least_one = update_soft_blocks(packet) || at_least_one;
        if (at_least_one) {
            broadcast(packet);
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
            at_least_one = update_player(time, *p.second, packet) || at_least_one;
        }
        at_least_one = update_bombs_explosions(time.asSeconds(), packet) || at_least_one;
        at_least_one = update_players_damage(packet) || at_least_one;
        at_least_one = update_soft_blocks(packet) || at_least_one;
        
        if (at_least_one && !end_notifier) {
            broadcast(packet);
        }

        // we are at the end of the game if only one player is left
        if (players.size() == 1) {
            std::cout << "SERVER : only one player left!" << std::endl;
            game_end_notify_loop(time, clock);
            end_notifier = true;
            break;
        }
    }
    // at the end of the game, copy
    // all the scores of the remaining players
    // to the table of players scores
    while (players.size() > 0) {
        sf::Packet packet;
        add_type_to_packet(packet, PacketType::ServerNotifyPlayerDisconnect);
        packet << players.begin()->first;
        broadcast(packet);
        handle_player_die_event(players.begin()->first);
        players.erase(players.begin());
    }
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::ServerNotifyLeaderboard);
    // sorting
    auto scores_tmp = flip_map(players_scores);
    for (auto&& score : scores_tmp) {
        packet << score.second +
                  " : " +
                  std::to_string(score.first.dying_score);
    }
    broadcast(packet);
    map.clear();
    ais.clear();
    state = ServerState::END_GAME;
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
    for (auto&& ai : ais) {
        ai.second->terminate();
    }
    if (notifier.joinable()) {
        end_notifier = true;
        notifier.join();
        std::cout << "SERVER : joined notifier!" << std::endl;
    } else {
        std::cout << "SERVER : notifier already joined!" << std::endl;
    }
}
