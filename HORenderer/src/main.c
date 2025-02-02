#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"
#include "matrix.h"
#include "upng.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

bool is_running = false;

int previous_frame_time = 0;

mat4_t projectionMatrix;

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
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating SDL Texture.\n");
        return false;
    }

    /* Initialize the perspective projection matrix */
    float fov = M_PI / 3.0;
    float aspect = ((float)window_height / (float)window_width);
    float znear = 0.1;
    float zfar = 100.0;
    projectionMatrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // Manually load the hardcoded texture data from the static array
    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    // texture_width = 64;
    // texture_height = 64;

    load_cube_mesh_data();
    // load_obj_file_data("C:/Users/hojoon/Developer/game_study/HORenderer/assets/f22.obj");

    /* Load the texture information from an external PNG file */
    load_png_texture_data("C:/Users/hojoon/Developer/game_study/HORenderer/assets/cube.png");

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
            if (event.key.keysym.sym == SDLK_5)
            {
                RenderMethod = RENDER_TEXTURED;
            }
            if (event.key.keysym.sym == SDLK_6)
            {
                RenderMethod = RENDER_TEXTURED_WIRE;
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

    // Change the mesh scale/rotation values per animation frame
    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.01f;
    mesh.rotation.z += 0.01f;
    mesh.translation.z = 5.0f;

    // Create a scale, rotation and translation matrix that will be used to multiply the mesh vertices
    mat4_t scaleMatrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translationMatrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotationMatrixX = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotationMatrixY = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotationMatrixZ = mat4_make_rotation_z(mesh.rotation.z);

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a World Matrix combining scale, rotation, and translation matrices
            mat4_t worldMatrix = mat4_identity();

            // Order matters : First scale, then rotate, the translate. 
            // [T] * [R] * [S] * v
            worldMatrix = mat4_multiply_mat4(scaleMatrix, worldMatrix);
            worldMatrix = mat4_multiply_mat4(rotationMatrixZ, worldMatrix);
            worldMatrix = mat4_multiply_mat4(rotationMatrixY, worldMatrix);
            worldMatrix = mat4_multiply_mat4(rotationMatrixX, worldMatrix);
            worldMatrix = mat4_multiply_mat4(translationMatrix, worldMatrix);

            // Multiply the world matrix by the original 
            transformed_vertex = mat4_multiply_vec4(worldMatrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);      /* A */
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);      /* B */
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);      /* C */

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

        if (CullMethod == CULL_BACKFACE) {
            // Bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }
        
        vec4_t projected_points[3];

        // Loop all three vertices to perform projection
        for (int j = 0; j < 3; j++) {
            // Project the current vertex
            projected_points[j] = mat4_multiply_vec4_project(projectionMatrix, transformed_vertices[j]);

            // Scale into the view 
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // Invert the y values to account for flipped screen y coordinate
            // Because screen y coordinates is growing down, but model y coordinates is growing up
            projected_points[j].y *= -1;

            // Translate the projected points to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calculate the average depth for each face based on the vertices after transformation
        float avgDepth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

        // Calculate the shade intensity based on how aligned is the face normal and thje light vector
        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points = {
                projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w,
                projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w,
                projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w,
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v }
            },
            .color = triangle_color,
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

        // Draw textured triangle
        if (RenderMethod == RENDER_TEXTURED || RenderMethod == RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }

        // Draw triangle wireframe
        if (RenderMethod == RENDER_WIRE || RenderMethod == RENDER_WIRE_VERTEX || RenderMethod == RENDER_FILL_TRIANGLE_WIRE || RenderMethod == RENDER_TEXTURED_WIRE) {
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
    upng_free(png_texture);
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