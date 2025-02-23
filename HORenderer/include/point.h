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

#endif /* POINT_H_ */