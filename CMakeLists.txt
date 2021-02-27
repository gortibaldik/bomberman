cmake_minimum_required(VERSION 3.12 FATAL_ERROR)


# Set project name and version
project(Bomberman VERSION 1.0)
add_executable(Bomberman main.cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# wanna link statically
# thanks to https://en.sfml-dev.org/forums/index.php?topic=7549.0
set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS OFF)
set_property(TARGET Bomberman PROPERTY POSITION_INDEPENDENT_CODE FALSE)
set(SFML_DIR /home/gortibaldik/Documents/utils/SFML/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release
    CACHE STRING
      "Choose the type of build (Debug or Release)"
      FORCE
  )
endif()

find_package(SFML 2.5.1
  COMPONENTS
    system window graphics network
      REQUIRED
)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)


# tell CMake to look to subdirs for CMakeLists containing more
# instructions
add_subdirectory(states)
add_subdirectory(window_manager)
add_subdirectory(texture_handling)
add_subdirectory(controls)
add_subdirectory(network)
add_subdirectory(game)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# include assets as project dependencies
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/media ${CMAKE_BINARY_DIR}/media
)
add_dependencies(Bomberman copy_assets)

target_include_directories(Bomberman
  PUBLIC
  "${SFML_INCLUDE_DIR}"
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(Bomberman
  sfml-graphics
  sfml-window
  sfml-network
  sfml-system
)



