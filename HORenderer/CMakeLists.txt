cmake_minimum_required(VERSION 3.8)

project(HORenderer VERSION 0.1)

include_directories("C:/SDL2/include")
link_directories("C:/SDL2/lib/x64")

add_executable(${PROJECT_NAME} WIN32
    src/main.c
)

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
)