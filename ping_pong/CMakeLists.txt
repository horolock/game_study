cmake_minimum_required(VERSION 3.30)

project(MYGAME)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")

include_directories("C:/Program Files (x86)/SDL2/include")
link_directories("C:/Program Files (x86)/SDL2/lib")

add_executable(sdl-test main.cpp)

target_link_libraries(sdl-test SDL2 SDL2main)