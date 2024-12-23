#include "triangle.h"

void int_swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
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

    // TODO: Draw flat-bottom triangle
    // fill_flat_bottom_triangle();

    // TODO: Draw flat-top triangle
    // fill_flat_top_triangle();
}