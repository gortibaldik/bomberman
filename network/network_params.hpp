#ifndef NETWORK_PARAMS_HPP
#define NETWORK_PARAMS_HPP

namespace Network {
    enum Network {
        HighestTimeStamp = 2147483647,
        ClientTimeOut = 10000, /* milliseconds */
        ClientConnectTimeOut = 5000, /* milliseconds */
        NullID = -1,
        HeartBeatInterval = 1000, /* milliseconds */
        NumberOfRetries = 10
    };
}

using ClientID = int;
using PortNumber = unsigned short;

#endif