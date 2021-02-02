#ifndef NETWORK_PARAMS_HPP
#define NETWORK_PARAMS_HPP

enum class Network {
    HighestTimeStamp = 2147483647,
    ClientTimeOut = 10000, /* miliseconds */
    ClientConnectTimeOut = 5000, /* miliseconds */
    NullID = -1
};

using ClientID = int;
using PortNumber = unsigned short;

#endif