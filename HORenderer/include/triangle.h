#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>

#include "point.h"
#include "vector.h"
#include "texture.h"

typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;0
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
} triangle_t;

void drawTriangle(Point p0, Point p1, Point p2, uint32_t color);
void drawFilledTriangle(Point4 p0, Point4 p1, Point4 p2, uint32_t color);
void drawTexturedTriangle(TexturePoint p, TexturePoint p1, TexturePoint p2, uint32_t* texture);

#endif /* TRIANGLE_H */