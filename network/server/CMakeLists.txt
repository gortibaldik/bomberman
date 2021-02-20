cmake_minimum_required(VERSION 3.13.0)
target_sources(Bomberman
  PUBLIC
    server.hpp
    server.cpp
    server_entity.hpp
    server_entity.cpp
    client_info.hpp
    client_info.cpp
    game_server.hpp
    game_server.cpp
    ai_escaper.hpp
    ai_escaper.cpp
  )
