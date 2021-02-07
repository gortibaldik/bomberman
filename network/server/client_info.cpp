#include "client_info.hpp"

ClientInfo::TimeDifference ClientInfo::check_time(const sf::Time& server_time) {
    auto elapsed = server_time.asMilliseconds() - last_sent_heartbeat.asMilliseconds();
    if (elapsed >= Network::ClientTimeOut) {
        return TimeDifference::TimeOut;
    }
    if (elapsed < (retries + 1) * Network::HeartBeatInterval) {
        return TimeDifference::Correct;
    }
    if (retries == 0) {
        waiting_for_heartbeat = true;
        last_sent_heartbeat = server_time;
    }
    retries++;
    return TimeDifference::HeartBeat;
}

void ClientInfo::uncheck_heartbeat(const sf::Time& c_time) {
    latency = c_time - last_sent_heartbeat;
    retries = 0;
    waiting_for_heartbeat = false;
}