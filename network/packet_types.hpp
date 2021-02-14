#ifndef PACKET_TYPES_HPP__
#define PACKET_TYPES_HPP__

#include <SFML/Network.hpp>

enum class PacketType {
    Disconnect = -1,
    Connect,
    Duplicate,
    SpawnPosition,
    StartGame,
    HeartBeat, /* 4 */
    GetReady,
    ClientReady,
    ClientMove,
    ClientDeployBomb, /* 8 */
    ServerNewBomb,
    ServerEraseBomb,
    ServerPlayerUpdate,
    ServerCreateExplosion, /* 12 */
    ServerEraseExplosion,
    ServerNotifyPlayerDisconnect,
    ServerNotifyPlayerDied,
    ServerNotifySoftBlockDestroyed,
    ServerNotifySoftBlockExists,
    Update,
    Invalid
};

void add_type_to_packet(sf::Packet& packet, PacketType type);

#endif
