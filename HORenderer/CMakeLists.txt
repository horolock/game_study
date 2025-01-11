cmake_minimum_required(VERSION 3.8)

project(HORenderer VERSION 0.1)

include_directories("C:/SDL2/include")
include_directories("include")
link_directories("C:/SDL2/lib/x64")

set(C_SOURCES 
    src/main.c
    src/display.c
    src/vector.c
    src/mesh.c
    src/triangle.c
    src/array.c
    src/matrix.c
    src/light.c
    src/texture.c
    src/swap.c
)

set (HEADER_FILES 
    include/array.h
    include/display.h
    include/mesh.h
    include/triangle.h
    include/vector.h
    include/matrix.h
    include/light.h
    include/texture.h
    include/swap.h
)

add_executable(${PROJECT_NAME} WIN32
    ${C_SOURCES}
    ${HEADER_FILES}
)

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
)