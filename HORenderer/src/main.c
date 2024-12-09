#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t triangles_to_render[M_MESH_FACES];

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

bool is_running = false;

int previous_frame_time = 0;

bool setup(void)
{
    // Allocate the required bytes in memory for the color buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);   

    // Check color buffer successfully allocated
    if (!color_buffer) {
        fprintf(stderr, "Allocating color buffer failed.\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating SDL Texture.\n");
        return false;
    }

    return true;
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void update(void)
{
    // Wait for target frame time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01f;
    cube_rotation.y += 0.01f;
    cube_rotation.z += 0.01f;

    for (int i = 0; i < M_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate the vertex away from the camera
            transformed_vertex.z -= camera_position.z;

            // Project the current vertex
            vec2_t projected_point = project(transformed_vertex);

            // Scale and translate the projected points to the middle of the screen
            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles to render
        triangles_to_render[i] = projected_triangle;
    }
}

void render(void)
{
    draw_grid(0xFF333333);
    
    // Loop all projected points and render them
    for (int i = 0; i < M_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];

        draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
    }
    
    render_color_buffer();

    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    is_running = initialize_window();

    is_running = setup();

    vec3_t my_vector = {2.0, 3.0, -1.0};        // Just for test

    while(is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}