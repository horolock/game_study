#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#include "point.h"
#include "vector.h"

#define FPS 60
#define FRAME_TARGET_TIME   (1000 / FPS)     

enum CullMethod {
    CULL_NONE,
    CULL_BACKFACE
} CullMethod;

enum RenderMethod {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
} RenderMethod;

/**
 * SDL variables
 */
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* colorBufferTexture;

extern uint32_t* colorBuffer;
extern float* zBuffer;
extern int windowWidth;
extern int windowHeight;

/**
 * Methods prototypes for display
 */
bool initializeWindow();
void drawGrid(uint32_t color);
void drawPixel(Point p, uint32_t color);
void drawLine(Point p0, Point p1, uint32_t color);
void drawRect(Point origin, int width, int height, uint32_t color);
void renderColorBuffer();
void clearColorBuffer(uint32_t color);
void clearZBuffer();
void destroyWindow();

#endif  /* DISPLAY_H */