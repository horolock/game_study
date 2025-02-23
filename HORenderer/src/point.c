#include "point.h"

void swapPoint4(Point4 *p0, Point4 *p1) {
    Point4 temp = *p0;
    *p0 = *p1;
    *p1 = temp;
}

void swapTexturePoint(TexturePoint *p0, TexturePoint *p1) {
    TexturePoint temp = *p0;

    *p0 = *p1;
    *p1 = temp;
}