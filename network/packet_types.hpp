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
    ClientMainSetReady,
    ClientMainStart,
    ServerNewBomb,
    ServerEraseBomb, /* 12 */
    ServerPlayerUpdate,
    ServerCreateExplosion,
    ServerEraseExplosion,
    ServerNotifyPlayerDisconnect, /* 16 */
    ServerNotifyPlayerDied,
    ServerNotifySoftBlockDestroyed,
    ServerNotifySoftBlockExists,
    ServerNotifyPowerUpDestroyed, /* 20 */
    ServerNotifyGameEnd,
    ServerNotifyLeaderboard,
    Update,
    Invalid
};

void add_type_to_packet(sf::Packet& packet, PacketType type);

#endif
