cmake_minimum_required(VERSION 3.8)

project(HORenderer VERSION 0.1)

include_directories("C:/SDL2/include")
include_directories("include")
link_directories("C:/SDL2/lib/x64")

set(SOURCES 
    src/main.c src/display.c
    include/display.h
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
)