cmake_minimum_required(VERSION 3.13.0)
target_sources(Bomberman
  PUBLIC
    abstract.hpp
    abstract.cpp
    menu_state.hpp
    menu_state.cpp
    start.hpp
    start.cpp
    server_create.hpp
    server_create.cpp
    server_create_waiting.hpp
    server_create_waiting.cpp
    client_connect.hpp
    client_connect.cpp
    client_connect_waiting.hpp
    client_connect_waiting.cpp
)
