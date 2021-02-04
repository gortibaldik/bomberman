cmake_minimum_required(VERSION 3.13.0)
add_subdirectory(client)
add_subdirectory(server)
target_sources(Bomberman
  PUBLIC
    packet_types.hpp
    packet_types.cpp
    network_params.hpp
    utils.hpp
    utils.cpp)
