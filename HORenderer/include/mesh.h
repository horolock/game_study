#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICES     8
extern vec3_t mesh_vertices[N_MESH_VERTICES];

// 6 Cube faces, 2 Triangles per face
#define M_MESH_FACES    12
extern face_t mesh_faces[M_MESH_FACES];

#endif /* MESH_H */