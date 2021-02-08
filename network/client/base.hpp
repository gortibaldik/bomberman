#ifndef CLIENT_DEF_HPP
#define CLIENT_DEF_HPP

#include <SFML/Network.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "network/network_params.hpp"
#include "network/packet_types.hpp"
#include "network/utils.hpp"

enum class ClientStatus {
    Connected,
    TryingToConnect,
    Failed,
    NotStarted,
    Terminated,
    Duplicate
};

class Client {
public:
    Client(const std::string& player_name): player_name(player_name), status(ClientStatus::NotStarted) {}
    ~Client();
    bool connect(const sf::IpAddress&, PortNumber);
    void listen();
    void send(sf::Packet&);
    void update(const sf::Time& dt);
    ClientStatus get_status() { return status; }
    bool is_connected() { return status == ClientStatus::Connected; }
    void terminate();

    ReceiverQueue& get_received_messages() { return received_messages; }
protected:
    bool handle_first_server_answer(sf::Packet&, sf::Int8);
    void handle_heartbeat(sf::Packet&);
    virtual void handle_others(sf::Packet&, PacketType) = 0;
    virtual void notify_disconnect() = 0;
    void update_time_overflow();
    friend class CA; /* Client Accessor */
    std::string player_name;

    std::atomic<ClientStatus> status;
    sf::IpAddress server_ip;
    PortNumber server_port_out, server_port_in;
    sf::Time server_time;
    sf::Time last_heart_beat;   

    sf::UdpSocket socket;
    std::mutex socket_mutex;
    std::thread worker;

    ReceiverQueue received_messages;
};

#endif