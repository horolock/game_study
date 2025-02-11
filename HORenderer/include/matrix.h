#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

/**
 * 4x4 Matrix type
 * For 3D calculation
 */
typedef struct {
    float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);     // Same as mat4_eye(void)
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);

/**
 * Multiply methods
 */
vec4_t mat4_multiply_vec4(mat4_t m, vec4_t v);
mat4_t mat4_multiply_mat4(mat4_t a, mat4_t b);
vec4_t mat4_multiply_vec4_project(mat4_t projectionMatrix, vec4_t v);

#endif /* MATRIX_H */