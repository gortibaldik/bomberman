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
    server_connect.hpp
    server_connect.cpp
    server_connect_waiting.hpp
    server_connect_waiting.cpp
)
