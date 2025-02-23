#include "display.h"

/**
 * Extern values
 */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* colorBufferTexture = NULL;
uint32_t* colorBuffer = NULL;
float* zBuffer = NULL;
int windowWidth = 800;
int windowHeight = 600;

bool initializeWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    /* Set width and height of the SDL window with the max screen resolution */
    SDL_DisplayMode displayMode;

    SDL_GetCurrentDisplayMode(0, &displayMode);

    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    /* Create a SDL Window */
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
    
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    /* Create a SDL Renderer */
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    
    return true;
}

void drawGrid(uint32_t color) {
    for (int y = 0; y < windowHeight; y += 10) {
        for (int x = 0; x < windowWidth; x += 10) {
            colorBuffer[(windowWidth * y) + x] = color;
        }
    }
}

void drawPixel(Point p, uint32_t color) {
    if (p.x >= 0 && p.x < windowWidth && p.y >= 0 && p.y < windowHeight) {
        colorBuffer[(windowWidth * p.y) + p.x] = color;
    }
}

void drawLine(Point p0, Point p1, uint32_t color) {
    int dx = (p0.x - p1.x);
    int dy = (p0.y - p1.y);
    int longestSideLength = (abs(dx) >= abs(dy)) ? abs(dx) : abs(dy);
    float xInc = dx / (float)longestSideLength;
    float yInc = dy / (float)longestSideLength;
    float currentX = p0.x;
    float currentY = p0.y;

    for (int i = 0; i <= longestSideLength; i++) {
        Point pointToDraw = {round(currentX), round(currentY)};
        drawPixel(pointToDraw, color);\
        currentX += xInc;
        currentY += yInc;
    }
}

void drawRect(Point origin, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Point pointToDraw = { .x = origin.x + i, .y = origin.y + j };

            drawPixel(pointToDraw, color);
        }
    }
}

void renderColorBuffer() {
    SDL_UpdateTexture(
        colorBufferTexture, NULL, colorBuffer, (int)(windowWidth * sizeof(uint32_t))
    );

    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void clearColorBuffer(uint32_t color) {
    for (int y = 0; y < windowHeight; y++) {
        for (int x = 0; x < windowWidth; x++) {
            colorBuffer[(windowWidth * y) + x] = color;
        }
    }
}

void clearZBuffer() {
    for (int y = 0; y < windowHeight; y++) {
        for (int x = 0; x < windowWidth; x++) {
            zBuffer[(windowWidth * y) + x] = 1.0;
        }
    }
}

void destroyWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}