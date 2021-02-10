#ifndef PACKET_TYPES_HPP__
#define PACKET_TYPES_HPP__

#include <SFML/Network.hpp>

enum class PacketType {
    Disconnect = -1,
    Connect,
    Duplicate,
    SpawnPosition,
    HeartBeat,
    GetReady, /* 4 */
    ClientReady,
    ClientMove,
    ClientDeployBomb,
    ServerNewBomb, /* 8 */
    ServerEraseBomb,
    ServerPlayerUpdate,
    ServerCreateExplosion,
    ServerEraseExplosion, /* 12 */
    ServerNotifyPlayerDisconnect,
    ServerNotifyPlayerDied,
    Update,
    Invalid
};

void add_type_to_packet(sf::Packet& packet, PacketType type);

#endif
