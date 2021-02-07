#include "game_server.hpp"

void GameServer::set_ready_game() {
    state = ServerState::STARTING;
    sf::Packet packet;
    add_type_to_packet(packet, PacketType::GetReady);
    broadcast(packet);
}

void GameServer::handle_others(sf::Packet& packet, PacketType ptype) {
    switch(state) {
    case ServerState::WAITING_ROOM:
        // in the waiting room, don't serve any other than connection and heartbeat packets
        return;
    case ServerState::STARTING:
        
    }
}