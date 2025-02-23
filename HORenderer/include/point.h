#ifndef POINT_H_
#define POINT_H_

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    int x;
    int y;
    float z;
    float w;
} Point4;

typedef struct
{
    int x;
    int y;
    float z;
    float w;
    float u;
    float v;
} TexturePoint;

void swapPoint4(Point4 *p0, Point4 *p1);
void swapTexturePoint(TexturePoint *p0, TexturePoint *p1);

#endif /* POINT_H_ */