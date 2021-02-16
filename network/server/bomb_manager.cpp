#include "bomb_manager.hpp"
#include "network/network_params.hpp"
#include "network/packet_types.hpp"
#include <iostream>

bool BombManager::update(const sf::Time& time, sf::Packet& packet) {
    std::vector<int> bombs_to_erase;
    bool result = false;
    for (auto&&b : bombs) {
        b.second.update(time.asSeconds());
        if (b.second.is_new()) {
            result = true;
            packet << sf::Int8(Network::Delimiter);
            add_type_to_packet(packet, PacketType::ServerNewBomb);
            packet << b.second;
        }
        if (b.second.can_explode()) {
            result = true;
            packet << sf::Int8(Network::Delimiter);
            add_type_to_packet(packet, PacketType::ServerEraseBomb);
            packet << b.second;
            b.second.spe.remove_deployed();
            for (auto&& exp : b.second.explode(map, explosion_time)) {
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
            packet << sf::Int8(Network::Delimiter);
            add_type_to_packet(packet, PacketType::ServerEraseExplosion);
            packet << exp.second;
            result = true;
            explosions_to_erase.push_back(exp.first);
        }
    }
    for (auto&& i : bombs_to_erase) {
        bombs.erase(i);
    }

    for (auto&& i : explosions_to_erase) {
        explosions.erase(i);
    }
    return result;
}

void BombManager::create_bomb(const EntityCoords& pos, ServerPlayerEntity& spe) {
    bombs.emplace(n_deployed_bombs, ServerBombEntity(pos, bomb_time, n_deployed_bombs, spe));
    n_deployed_bombs++;
}

bool BombManager::check_damage(Players& players, sf::Packet& packet) {
    bool result = false;
    for (auto&& exp : explosions) {
        for (auto&& p : players) {
            if (!p.second.is_attackable()) { continue; }
            if (naive_bbox_intersect(p.second.actual_pos, exp.second.actual_pos)) {
                std::cout << p.first << " is hit!" << std::endl;
                p.second.actual_pos = p.second.spawn_pos;
                result = true;
                if (p.second.lives == 1) {
                    packet << sf::Int8(Network::Delimiter);
                    add_type_to_packet(packet, PacketType::ServerNotifyPlayerDied);
                    packet << p.first;
                    players.erase(p.first);
                } else {
                    p.second.lives--;
                    p.second.respawn();
                    packet << sf::Int8(Network::Delimiter);
                    add_type_to_packet(packet, PacketType::SpawnPosition);
                    packet << p.second;
                }
                break;
            }
        }
        int i = -1;
        for (auto&& exists : map.get_soft_blocks()) {
            i++;
            if (!exists) { continue; }
            auto pair = map.transform_to_coords(i);
            if (naive_bbox_intersect(pair, exp.second.actual_pos)) {
                map.erase_soft_block(i);
                result = true;
                packet << sf::Int8(Network::Delimiter);
                add_type_to_packet(packet, PacketType::ServerNotifySoftBlockDestroyed);
                packet << sf::Int32(i);
            }
        }
    }
    return result;
}