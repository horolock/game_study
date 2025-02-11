#include "display.h"
#include "triangle.h"
#include "swap.h"

/**
 * Draw a filled a triangle with a flat bottom
 *              (x0, y0)
 *                 /\
 *                /  \
 *               /    \
 *              /      \
 *             /        \
 *        (x1, y1)------(x2, y2) == (Mx, My)
 */
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) 
{
    // Find the two slopes (two triangle legs)
    float inverseSlope1 = (float)(x1 - x0) / (y1 - y0);
    float inverseSlope2 = (float)(x2 - x0) / (y2 - y0);

    // Start x_start and x_end from the top vertex(x0, y0)
    float xStart = x0;
    float xEnd = x0;

    // Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; y++) {
        draw_line(xStart, y, xEnd, y, color);

        xStart += inverseSlope1;
        xEnd += inverseSlope2;
    }
}

/**
 *      (x0, y0)--------(x1, y1)
 *          \               /
 *           \             /
 *            \           /
 *             \         /
 *              \       /
 *               \     /
 *                \   /
 *                 \ /
 *               (x0, y0)
 * 
 */
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) 
{
    // Find the two slopes (two triangle legs)
    float leftSlope = (float)(x2 - x0) / (y2 - y0);
    float rightSlope = (float)(x2 - x1) / (y2 - y1);

    // Start x_start and x_end from the bottom vertex(x2, y2)
    float xStart = x2;
    float xEnd = x2;

    // Loop all the scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        draw_line(xStart, y, xEnd, y, color);

        xStart -= leftSlope;
        xEnd -= rightSlope;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

/**
 * Function to draw the textured pixel at position x and y using interpolation
 */
void draw_texel(int x, int y, uint32_t* texture, vec4_t point_a, vec4_t point_b, vec4_t point_c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    /**
     * Variables to store the interpolated values of U, V, and also 1/W for the current pixel
     */
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;
    
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    /**
     * Also interpolate the valuue of 1/W for the current pixel
     */
    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    /**
     * Now we can divide back both interpolated values by 1/W
     */
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    /**
     * Map the UV coordinate to the full texture width and height
     */
    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));

    if (tex_x >= 0 && tex_x <= texture_width && tex_y >= 0 && tex_y <= texture_height) {
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    // We need to start the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2) {
        // We can simply draw the flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        // We can simply draw the flat-top triangle
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        // Calculate the new vertex (Mx, My) using triangle similarity
        int My = y1;
        int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
    }
}

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture
) {
    /**
     * We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
     */
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    /**
     * Flip the v component to account for inverted UV-coordinates (v grows downwards)
     */
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    /**
     * Create vector points after we sort the vertices
     */
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    /**
     * ---------------------------------------------------
     * Render the upper part of the triangle (Flat-bottom)
     * ---------------------------------------------------
     */
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) {inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);}
    if (y2 - y0 != 0) {inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);}

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            /* Swap if x_start is the right of x_end */
            if (x_end < x_start) { int_swap(&x_start, &x_end); }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    /**
     * ---------------------------------------------------
     * Render the bottom part of the triangle (Flat-top)
     * ---------------------------------------------------
     */
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) { inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1); }
    if (y2 - y0 != 0) { inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0); }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    vec2_t ap = vec2_sub(p, a);

    /**
     * Area of the full paralleleogram (triangle ABC) using cross product
     * | ac.x | \/ | ab.x |
     * | ac.y | /\ | ab.y |
     * ac.x * ab.y - ac.y * ab.x
     */
    float area_parallelogram_abc = (ac.x * ab.y) - (ac.y * ab.x);

    /**
     * Alpha = area of parallelogram PBC over the area of the full parallelogram ABC
     * || PC x PB ||
     * -------------
     * || AC x AB ||
     * 
     * Beta = area of parallelogram APC over the area of the full parallelogram ABC
     * || AC x AP ||
     * -------------
     * || AC x AB ||
     * 
     * Gamma can be easily found since barycentric coordinates alwasy add up to 1.0
     * gamma = 1.0 - alpha - beta
     */
    float alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / area_parallelogram_abc;
    float beta = ((ac.x * ap.y) - (ac.y * ap.x)) / area_parallelogram_abc;
    float gamma = 1.0 - alpha - beta;

    vec3_t weights = {alpha, beta, gamma};

    return weights;
}