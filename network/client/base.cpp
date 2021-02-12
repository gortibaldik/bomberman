#include "base.hpp"
#include "network/packet_types.hpp"
#include <iostream>

enum WAIT_RESULT {
    CONTINUE,
    SUCCESS,
    FAILURE
};

/* Client Accessor */
class CA {
public:
    static void initialize_init_packet(const Client& c, sf::Packet& p) {
        add_type_to_packet(p, PacketType::Connect);
        p << c.player_name;
    }

    static bool try_send_to_server(Client& c,
                                  sf::Packet& p,
                                  const sf::IpAddress& server_ip,
                                  PortNumber server_port) {
        return (c.socket.send(p, server_ip, server_port) == sf::Socket::Done);
    }

    static WAIT_RESULT wait_for_server(Client& c, const sf::IpAddress& server_ip) {
        auto status = c.socket.receive(packet, receiver_ip, receiver_port);
        if (status != sf::Socket::Done || receiver_ip != server_ip) {
            return CONTINUE;
        }
        if (!(packet >> ptype)) { return FAILURE; }
        std::cout << "from " << receiver_port << std::endl;
        return SUCCESS;
    }
    
    static WAIT_RESULT listen_to_server(Client& c) {
        auto status = c.socket.receive(packet, receiver_ip, receiver_port);
        if (status != sf::Socket::Done) {
            if (c.is_connected()) {
                return CONTINUE;
            }
            // packet receiving failed because the connection was closed
            return FAILURE; 
        }
        // unknown sender, or undecodable packet
        if ((receiver_ip != c.server_ip) || !(packet >> ptype)) { return CONTINUE; }
        auto t = (PacketType)ptype;
        // invalid type of packet
        if ((t < PacketType::Disconnect) || (t >= PacketType::Invalid)) { return CONTINUE; }
        return SUCCESS;

    }

    static sf::Int8 ptype;
    static sf::IpAddress receiver_ip;
    static PortNumber receiver_port;
    static sf::Packet packet;

};
sf::Int8 CA::ptype;
sf::IpAddress CA::receiver_ip;
PortNumber CA::receiver_port;
sf::Packet CA::packet;

bool Client::connect(const sf::IpAddress& server_ip, PortNumber server_port) {
    if (is_connected()) {
        return false;
    }
    status = ClientStatus::TryingToConnect;
    socket.bind(sf::Socket::AnyPort);
    std::cout << "Client sending and listening on: " << socket.getLocalPort() << std::endl;
    sf::Packet packet;
    CA::initialize_init_packet(*this, packet);
    if (! CA::try_send_to_server(*this, packet, server_ip, server_port)) {
        socket.unbind();
        status = ClientStatus::Failed;
        return false;
    }
    socket.setBlocking(false);
    sf::Clock timer;
    timer.restart();
    std::cout << "Connecting to " << server_ip << ":" << server_port << std::endl;
    while (timer.getElapsedTime().asMilliseconds() < sf::Int32(Network::ClientConnectTimeOut)) {
        auto result = CA::wait_for_server(*this, server_ip);
        auto ptype = (PacketType)CA::ptype;
        if (result == FAILURE) { break; }
        if (ptype == PacketType::Duplicate) {
            std::cout << "Duplicate name !" << std::endl;
            status = ClientStatus::Duplicate;
            return false;
        }
        if ((result == CONTINUE) || (ptype != PacketType::Connect)) { continue; }
        this->server_ip = CA::receiver_ip;
        this->server_port_out = CA::receiver_port;
        this->server_port_in = server_port;

        status = ClientStatus::Connected;
        socket.setBlocking(true);
        updater = std::thread(&Client::update_loop, this);
        listener = std::thread(&Client::listen, this);
        std::cout << "SUCCESS" << std::endl;
        return true;
    }
    status = ClientStatus::Failed;
    std::cout << "FAILURE" << std::endl;
    return false;
}

/* Keep the actual connection with the server until the server
 * responds to the heartbeats
 *  - doesn't interfere with any other interface of the client 
 *    (except answering heartbeat messages to server)
 */
void Client::update_loop() {
    using namespace std::chrono_literals;
    sf::Clock c;
    c.restart();
    while(is_connected()) {
        server_time += c.restart();
        // overflow may happen after really long time
        // but just for the sake of completeness...
        if (server_time.asMilliseconds() < 0) {
            update_time_overflow();
        }
        // if the client hadn't received the heartbeat response
        // from the server for long enough, it terminates the connection
        if (server_time.asMilliseconds() - last_heart_beat.asMilliseconds() >= Network::ClientTimeOut) {
            std::cout << "Timed out!" << std::endl;
            terminate();
        }
        std::this_thread::sleep_for(100ms);
    }
    terminate();
}

void Client::handle_heartbeat(sf::Packet& p) {
    sf::Int32 t;
    sf::Packet packet;
    if (!(p >> t)) {
        std::cout << "Invalid heartbeat!" << std::endl;
    }
    server_time = sf::milliseconds(t);
    last_heart_beat = server_time;
    add_type_to_packet(packet, PacketType::HeartBeat);
    if (socket.send(packet, this->server_ip, this->server_port_in) != sf::Socket::Done) {
        std::cout << "Failed to respond to a heartbeat!" << std::endl;
    }
}

void Client::send(sf::Packet& packet) {
    socket.send(packet, server_ip, server_port_in);
}

void Client::listen() {
    while (is_connected()) {
        sf::Packet packet;
        CA::packet = packet;
        auto result = CA::listen_to_server(*this);
        if (result == FAILURE) { break; }
        if (result == CONTINUE) { continue; }
        switch(static_cast<PacketType>(CA::ptype)) {
        case PacketType::HeartBeat:
            handle_heartbeat(CA::packet);
            break;
        case PacketType::Disconnect:
            socket.unbind();
            status = ClientStatus::Terminated;
            notify_disconnect();
            break;
        default:
            handle_others(CA::packet, static_cast<PacketType>(CA::ptype));
        }
    }
    status = ClientStatus::Terminated;
}

void Client::update_time_overflow() {
    server_time -= sf::milliseconds(Network::HighestTimeStamp);
    last_heart_beat = server_time;
}

void Client::terminate() {
    if(is_connected()) {
        status = ClientStatus::Terminated;
        sf::Packet p;
        add_type_to_packet(p, PacketType::Disconnect);
        socket.send(p, server_ip, server_port_in);
        socket.send(p, sf::IpAddress::getLocalAddress(), socket.getLocalPort());
        std::cout << "Client sent terminate socket!" << std::endl;
        socket.unbind();
        if (listener.joinable()) {
            std::cout << "Joining client listener!" << std::endl;
            listener.join();
        } else {
            std::cout << "Client listener already joined!" << std::endl;
        }
    }
}

Client::~Client() {
    // end both updater and listener threads
    terminate();
    if (listener.joinable()) {
        listener.join();
        std::cout << "Joined client listener!" << std::endl;
    } else {
        std::cout << "Client listener already joined!" << std::endl;
    }

    if (updater.joinable()) {
        updater.join();
        std::cout << "Joined client updater!" << std::endl;
    }
    else {
        std::cout << "Client updater already joined!" << std::endl;
    }
}