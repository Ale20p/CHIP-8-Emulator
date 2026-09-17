cmake_minimum_required(VERSION 3.16)
project(Chip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate SDL2 via vcpkg CMake config
find_package(SDL2 CONFIG REQUIRED)

# Execute target
add_executable(chip8 src/main.cpp)

target_include_directories(chip8 PRIVATE src)

# Link SDL2 libraries
target_link_libraries(chip8 PRIVATE 
    SDL2::SDL2
    SDL2::SDL2main
)