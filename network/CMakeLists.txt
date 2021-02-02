cmake_minimum_required(VERSION 3.13.0)
add_subdirectory(client)
add_subdirectory(server)
target_sources(Bomberman
  PUBLIC
    packet_types.hpp
    network_params.hpp)
