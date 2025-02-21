#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"
#include "matrix.h"
#include "upng.h"

/**
 * Global variables for execution status and game loop
 */
bool isRunning = false;
int previousFrameTime = 0;
vec3_t cameraPosition = {0, 0, 0};

/**
 * Array to store triangles that should be rendered each frame
 */
#define MAX_TRIANGLES           10000
triangle_t trianglesToRender[MAX_TRIANGLES];
int numTrianglesToRender = 0;

/**
 * Global Transformation Matrices
 */
mat4_t worldMatrix;
mat4_t projectMatrix;

bool setup(void)
{
    // Initialize render mode and triangle culling method.
    RenderMethod = RENDER_WIRE;
    CullMethod = CULL_BACKFACE;

    // Allocate the required bytes in memory for the color buffer and the z-buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);   
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

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
    projectMatrix = mat4_make_perspective(fov, aspect, znear, zfar);

    /* Load the vertex and face values for the mesh data structure */
    load_obj_file_data("C:/Users/hojoon/Developer/game_study/HORenderer/assets/f22.obj");

    /* Load the texture information from an external PNG file */
    load_png_texture_data("C:/Users/hojoon/Developer/game_study/HORenderer/assets/f22.png");

    return true;
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
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
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - previousFrameTime);

    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) { SDL_Delay(timeToWait); }

    previousFrameTime = SDL_GetTicks();
    numTrianglesToRender = 0;

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
        face_t meshFace = mesh.faces[i];
        vec3_t faceVertices[3];

        /**
         * Fix here?
         */
        faceVertices[0] = mesh.vertices[meshFace.a];
        faceVertices[1] = mesh.vertices[meshFace.b];
        faceVertices[2] = mesh.vertices[meshFace.c];

        vec4_t transformedVertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(faceVertices[j]);

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
            transformedVertices[j] = transformed_vertex;
        }
        
        vec3_t vectorA = vec3_from_vec4(transformedVertices[0]);
        vec3_t vectorB = vec3_from_vec4(transformedVertices[1]);
        vec3_t vectorC = vec3_from_vec4(transformedVertices[2]);
        vec3_t vectorAB = vec3_sub(vectorB, vectorA);
        vec3_t vectorAC = vec3_sub(vectorC, vectorA);

        vec3_normalize(&vectorAB);
        vec3_normalize(&vectorAC);

        /* Compute the face normal (using cross product to find perpendicular) */
        vec3_t normal = vec3_cross(vectorAB, vectorAC);
        vec3_normalize(&normal);

        /* Find the vector between vertex A in the triangle and the camera origin */
        vec3_t cameraRay = vec3_sub(cameraPosition, vectorA);

        /* Calculate how aligned the camera ray is with the face normal */
        float dotNormalCamera = vec3_dot(normal, cameraRay);

        /* Backface culling test to see if the current face should be projected */
        if (CullMethod == CULL_BACKFACE) {
            if (dotNormalCamera < 0) { continue; }
        }

        vec4_t projectedPoints[3];

        /* Loop all three vertices to perform projection and conversion to screen space */
        for (int j = 0; j < 3; j++) {
            /* Project the current Vertex */
            projectedPoints[j] = mat4_multiply_vec4_project(projectMatrix, transformedVertices[j]);

            /**
             * Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space
             * y values grow top->down
             */
            projectedPoints[i].y *= -1;
            
            /* Scale into the view */
            projectedPoints[j].x *= (window_width / 2.0);
            projectedPoints[j].y *= (window_height / 2.0);

            /* Translate the projected points to the middle of the screen */
            projectedPoints[j].x += (window_width / 2.0);
            projectedPoints[j].y += (window_height / 2.0);
        }

        
        float lightIntensityFactor = -vec3_dot(normal, light.direction);
        uint32_t triangleColor = light_apply_intensity(meshFace.color, lightIntensityFactor);
        triangle_t projectedTriangle = {
            .points = {
                projectedPoints[0].x, projectedPoints[0].y, projectedPoints[0].z, projectedPoints[0].w,
                projectedPoints[1].x, projectedPoints[1].y, projectedPoints[1].z, projectedPoints[1].w,
                projectedPoints[2].x, projectedPoints[2].y, projectedPoints[2].z, projectedPoints[2].w
            },
            .texcoords = {
                { meshFace.a_uv.u, meshFace.a_uv.v },
                { meshFace.b_uv.u, meshFace.b_uv.v },
                { meshFace.c_uv.u, meshFace.c_uv.v }
            },
            .color = triangleColor
        };

        /* Save the projected triangle in the array of triangles to render */
        if (numTrianglesToRender < MAX_TRIANGLES) { trianglesToRender[numTrianglesToRender++] = projectedTriangle; }
    }
}

void render(void)
{
    SDL_RenderClear(renderer);

    draw_grid(0xFF333333);
    
    /* Loop all projected triangles and render */
    for (int i = 0; i < numTrianglesToRender; i++) {
        triangle_t triangle = trianglesToRender[i];

        /* Draw Filled Triangle */
        if (RenderMethod == RENDER_FILL_TRIANGLE || RenderMethod == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(
                (int)triangle.points[0].x, (int)triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                (int)triangle.points[1].x, (int)triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                (int)triangle.points[2].x, (int)triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color);
        }

        /* Draw Textured Triangle */
        if (RenderMethod == RENDER_TEXTURED || RenderMethod == RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(
                (int)triangle.points[0].x, (int)triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                (int)triangle.points[1].x, (int)triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                (int)triangle.points[2].x, (int)triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }

        /* Draw Triangle Wireframe */
        if (RenderMethod == RENDER_WIRE || RenderMethod == RENDER_WIRE_VERTEX || RenderMethod == RENDER_FILL_TRIANGLE_WIRE || RenderMethod == RENDER_TEXTURED_WIRE) {
            draw_triangle(
                (int)triangle.points[0].x, (int)triangle.points[0].y,
                (int)triangle.points[1].x, (int)triangle.points[1].y,
                (int)triangle.points[2].x, (int)triangle.points[2].y,
                0xFFFFFFFF);
        }

        /* Draw Triangle Vertex Points */
        if (RenderMethod == RENDER_WIRE_VERTEX) {
            draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }
    }
    
    render_color_buffer();

    clear_color_buffer(0xFF000000);
    clear_z_buffer();
    
    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
    array_free(mesh.vertices);
    array_free(mesh.faces);
}

int main(int argc, char* argv[])
{
    isRunning = initialize_window();
    isRunning = setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }
    
    destroy_window();
    free_resources();

    return 0;
}