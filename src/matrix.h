#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"


typedef struct {
    float m[4][4];
} mat4_t;

//returns ideentiry matrix
mat4_t mat4_identity(void);

mat4_t mat4_make_scale(float sx, float sy, float sz);

vec4_t mul_mat_vec(mat4_t matrix, vec4_t vector);

mat4_t mat4_make_translate(float, float, float);

//TODO
//getTransationMatrix
//getRotationMatrixX,Y,Z
//ADD NEW TYPE VEC4_T
//Convert to vec3 to vec4 and vice versa.
#endif