cmake_minimum_required(VERSION 3.8)

project(HORenderer VERSION 0.1)

include_directories("C:/SDL2/include")
include_directories("include")
link_directories("C:/SDL2/lib/x64")

set(SOURCES 
    src/main.c src/display.c src/vector.c src/mesh.c src/triangle.c
    include/display.h
    include/vector.h
    include/mesh.h
    include/triangle.h
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
)