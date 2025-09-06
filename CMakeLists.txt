cmake_minimum_required(VERSION 4.0)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 20)
add_executable(chip8_emulator main.cpp)
target_link_libraries(chip8_emulator SDL2main SDL2)
