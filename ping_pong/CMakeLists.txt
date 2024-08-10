cmake_minimum_required(VERSION 3.20)

project(MYGAME)

add_executable(sdl-test main.cpp game.cpp)

include_directories(".")
link_directories(".")

target_link_libraries(sdl-test SDL2 SDL2main)