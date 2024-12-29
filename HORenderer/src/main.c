#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

bool is_running = false;

int previous_frame_time = 0;

bool setup(void)
{
    // Initialize render mode and triangle culling method.
    RenderMethod = RENDER_WIRE;
    CullMethod = CULL_BACKFACE;

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

    load_cube_mesh_data();
    // load_obj_file_data("C:/Users/hojoon/Developer/game_study/HORenderer/assets/f22.obj");

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
            {
                is_running = false;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                RenderMethod = RENDER_WIRE_VERTEX;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                RenderMethod = RENDER_WIRE_VERTEX;
            }
            if (event.key.keysym.sym == SDLK_2)
            {
                RenderMethod = RENDER_WIRE;
            }
            if (event.key.keysym.sym == SDLK_3)
            {
                RenderMethod = RENDER_FILL_TRIANGLE;
            }
            if (event.key.keysym.sym == SDLK_4)
            {
                RenderMethod = RENDER_FILL_TRIANGLE_WIRE;
            }
            if (event.key.keysym.sym == SDLK_c)
            {
                CullMethod = CULL_BACKFACE;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                CullMethod = CULL_NONE;
            }
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

    // Initialize the array of triangles to render
    triangles_to_render = NULL;

    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.01f;
    mesh.rotation.z += 0.01f;

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // Translate the vertex away from the camera
            transformed_vertex.z += 5;

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        if (CullMethod == CULL_BACKFACE) {
            vec3_t vector_a = transformed_vertices[0];      /* A */
            vec3_t vector_b = transformed_vertices[1];      /* B */
            vec3_t vector_c = transformed_vertices[2];      /* C */

            // Get the vector subtraction of B-A and C-A
            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);

            // Compute the face normal (using the cross product to find perpendicular)
            vec3_t normal = vec3_cross(vector_ab, vector_ac);

            // Normalize the face normal vector
            vec3_normalize(&normal);

            // Find the vector between a point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(camera_position, vector_a);

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = vec3_dot(normal, camera_ray);

            // Bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }
        
        vec2_t projected_points[3];

        // Loop all three vertices to perform projection
        for (int j = 0; j < 3; j++) {
            // Project the current vertex
            projected_points[j] = project(transformed_vertices[j]);

            // Scale and translate the projected points to the middle of the screen
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }

        // Calculate the average depth for each face based on the vertices after transformation
        float avgDepth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

        triangle_t projected_triangle = {
            .points = {
                projected_points[0].x, projected_points[0].y,
                projected_points[1].x, projected_points[1].y,
                projected_points[2].x, projected_points[2].y,
            },
            .color = mesh_face.color,
            .avgDepth = avgDepth
        };

        // Save the projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort the triangles to render by their avgDepth
    int numTriangles = array_length(triangles_to_render);
    for (int i = 0; i < numTriangles; i++) {
        for (int j = i; j < numTriangles; j++) {
            if (triangles_to_render[i].avgDepth < triangles_to_render[j].avgDepth) {
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;
            }
        }
    }
}

void render(void)
{
    draw_grid(0xFF333333);
    
    // Loop all projected points and render them
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        // Draw filled triangle
        if (RenderMethod == RENDER_FILL_TRIANGLE || RenderMethod == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(
                (int)triangle.points[0].x, (int)triangle.points[0].y,
                (int)triangle.points[1].x, (int)triangle.points[1].y,
                (int)triangle.points[2].x, (int)triangle.points[2].y,
                triangle.color);
        }

        // Draw triangle wireframe
        if (RenderMethod == RENDER_WIRE || RenderMethod == RENDER_WIRE_VERTEX || RenderMethod == RENDER_FILL_TRIANGLE_WIRE) {
            draw_triangle(
                (int)triangle.points[0].x, (int)triangle.points[0].y,
                (int)triangle.points[1].x, (int)triangle.points[1].y,
                (int)triangle.points[2].x, (int)triangle.points[2].y,
                0xFFFFFFFF);
        }

        // Draw triangle vertex points
        if (RenderMethod == RENDER_WIRE_VERTEX) {
            draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }
    }

    // Clear the array of triangles to render every frame loop
    array_free(triangles_to_render);
    
    render_color_buffer();

    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    free(color_buffer);
    array_free(mesh.vertices);
    array_free(mesh.faces);
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
    free_resources();

    return 0;
}