#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>
#include "vector.h"

#define FPS 60
// 1000MS (1SEC) / 30
#define FRAME_TARGET_TIME   (1000 / FPS)     

enum CullMethod {
    CULL_NONE,          /* Disable Backface-Culling */
    CULL_BACKFACE       /* Enable Backface-Culling */
} CullMethod;

enum RenderMethod {
    RENDER_WIRE,                /* Enable Wireframe */
    RENDER_WIRE_VERTEX,         /* Display Wireframe with Vertex */
    RENDER_FILL_TRIANGLE,       /* Fill triangle with color */
    RENDER_FILL_TRIANGLE_WIRE,  /* Fill triangle with color and display wireframe */
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
} RenderMethod;

/**
 * SDL variables
 */
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;

extern uint32_t* color_buffer;
extern float* z_buffer;
extern int window_width;
extern int window_height;

/**
 * Methods prototypes for display
 */
bool initialize_window(void);
void draw_grid(uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer();
void destroy_window(void);

#endif  /* DISPLAY_H */