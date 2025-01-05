#include <math.h>

#include "matrix.h"

mat4_t mat4_identity(void) 
{
    /**
     * 1 0 0 0
     * 0 1 0 0
     * 0 0 1 0
     * 0 0 0 1
     */
    mat4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
    /**
     * sx   0   0   0
     * 0    sy  0   0
     * 0    0   sz  0
     * 0    0   0   1
     */
    mat4_t m = mat4_identity();

    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
    /**
     * 1    0   0   tx
     * 0    1   0   ty
     * 0    0   1   tz
     * 0    0   0   1
     */
    mat4_t m = mat4_identity();

    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;

    return m;
}

mat4_t mat4_make_rotation_x(float angle)
{
    /**
     * 1    0           0           0
     * 0    cos(a)      -sin(a)     0
     * 0    sin(a)      cos(a)      0
     * 0    0           0           1
     */
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    mat4_t m = mat4_identity();

    m.m[1][1] = cosAngle;
    m.m[1][2] = -sinAngle;
    m.m[2][1] = sinAngle;
    m.m[2][2] = cosAngle;

    return m;
}

mat4_t mat4_make_rotation_y(float angle)
{
    /**
     * cos(a)       0       sin(a)      0
     * 0            1       0           0
     * -sin(a)      0       cos(a)      0
     * 0            0       0           1
     */
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    mat4_t m = mat4_identity();

    m.m[0][0] = cosAngle;
    m.m[0][2] = sinAngle;
    m.m[2][0] = -sinAngle;
    m.m[2][2] = cosAngle;

    return m;
}

mat4_t mat4_make_rotation_z(float angle)
{
    /**
     * cos(a)   -sin(a)     0       0
     * sin(a)   cos(a)      0       0
     * 0        0           1       0
     * 0        0           0       1
     */
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    mat4_t m = mat4_identity();

    m.m[0][0] = cosAngle;
    m.m[0][1] = -sinAngle;
    m.m[1][0] = sinAngle;
    m.m[1][1] = cosAngle;

    return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
    /**
     * (h/w) * (1 / tan(fov/2))   0                         0                                   0
     *          0           (1 / tan(fov/2))                0                                   0
     *          0                 0    (zfar/(zfar-znear) - (zfar * znear) / (zfar - znear))    0
     *          0                 0                         1                                   0
     *                                                      |
     *                                                      -> we'll save the original unchanged 'z' value inside 'w'
     */
    mat4_t projectionMatrix = {{{ 0 }}};

    projectionMatrix.m[0][0] = aspect * (1 / (tan(fov/2)));
    projectionMatrix.m[1][1] = (1 / (tan(fov/2)));
    projectionMatrix.m[2][2] = ((zfar / (zfar - znear)) - (zfar * znear) / (zfar - znear));
    projectionMatrix.m[3][2] = 1.0; // This '1' is for saving original 'z' value in 'w'

    return projectionMatrix;
}

vec4_t mat4_multiply_vec4(mat4_t m, vec4_t v)
{
    vec4_t result;

    result.x = (m.m[0][0] * v.x) + (m.m[0][1] * v.y) + (m.m[0][2] * v.z) + (m.m[0][3] * v.w);
    result.y = (m.m[1][0] * v.x) + (m.m[1][1] * v.y) + (m.m[1][2] * v.z) + (m.m[1][3] * v.w);
    result.z = (m.m[2][0] * v.x) + (m.m[2][1] * v.y) + (m.m[2][2] * v.z) + (m.m[2][3] * v.w);
    result.w = (m.m[3][0] * v.x) + (m.m[3][1] * v.y) + (m.m[3][2] * v.z) + (m.m[3][3] * v.w);

    return result;
}

mat4_t mat4_multiply_mat4(mat4_t a, mat4_t b)
{
    mat4_t result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = (a.m[i][0] * b.m[0][j]) + (a.m[i][1] * b.m[1][j]) + (a.m[i][2] * b.m[2][j]) + (a.m[i][3] * b.m[3][j]);
        }
    }

    return result;
}

vec4_t mat4_multiply_vec4_project(mat4_t projectionMatrix, vec4_t v)
{
    /* Multiplication by the Projection Matrix */
    vec4_t result = mat4_multiply_vec4(projectionMatrix, v);

    /**
     * Perspective Divide
     * Perform perspective divide with original z-value that is now stored in w
     */
    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}