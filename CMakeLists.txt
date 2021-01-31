cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(Bomberman VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug 
    CACHE STRING 
      "Choose the type of build (Debug or Release)" 
      FORCE
    )
endif()

# Find SFML shared libraries
set(CMD_OPTION "x64-Debug")
set(SFML_V "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.5.1")
set(SFML_PREFIX "${SFML_V}/out/install/${CMD_OPTION} (default)")
set(SFML_INCLUDES "${SFML_V}/include")
set(SFML_DIR "${SFML_PREFIX}/lib/cmake/SFML/")
find_package(SFML 2.5.1
    COMPONENTS 
        system window graphics network audio REQUIRED
  )

# Find Boost asio libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost
    COMPONENTS
        system thread REQUIRED
    PATHS
	"C:/Program Files/boost_1_75_0/stage/lib/cmake/Boost-1.75.0/")

# set cmake multithreaded flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Compile executable
link_directories("${SFML_PREFIX}/bin")
add_executable(Bomberman main.cpp)

# tell CMake to look to subdirs for CMakeLists containing more
# instructions
add_subdirectory(states)
add_subdirectory(window_manager)
add_subdirectory(texture_manager)
add_subdirectory(controls)

# include assets as project dependencies
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/media ${CMAKE_BINARY_DIR}/media
)
add_dependencies(Bomberman copy_assets)

# Set include directory search paths
# For adding functionality of including
# from subdir1 header from subdir2
# we need to make CMAKE_SOURCE_DIR as 
# path to included files
target_include_directories(Bomberman 
  PUBLIC
  "${SFML_INCLUDES}"
  "${CMAKE_SOURCE_DIR}"
  ${Boost_INCLUDE_DIRS}
  )

# Link executable to target libraries
target_link_libraries(Bomberman sfml-graphics ${Boost_LIBRARIES})

# Install target
install(TARGETS Bomberman DESTINATION "${CMAKE_SOURCE_DIR}/bin")

# Instal game assets
install(DIRECTORY "${CMAKE_SOURCE_DIR}/media/"
          DESTINATION "${CMAKE_SOURCE_DIR}/bin/media")
