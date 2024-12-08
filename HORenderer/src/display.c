#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;
float fov_factor = 640.0;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what is the fullscreen max. width and height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create a SDL Window
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);
    
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL Renderer
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    
    return true;
}

void draw_grid(uint32_t color)
{
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color)
{
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_rectangle(int pos_x, int pos_y, int width, int height, uint32_t color)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = pos_x + i;
            int current_y = pos_y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };

    return projected_point;
}

void render_color_buffer(void)
{
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void destroy_window(void)
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}