#include "matrix.h"
#include "vector.h"

mat4_t mat4_identity(void) {
    mat4_t m = {{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    }};

    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    // | sx  0  0  0 |
    // |  0 sy  0  0 |
    // |  0  0 sz  0 |  
    // |  0  0  0  1 |
    mat4_t m  = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

mat4_t mat4_make_translate(float sx, float sy, float sz) {
    // |  1  0  0  sx |
    // |  0  1  0  sy |
    // |  0  0  1  sz |  
    // |  0  0  0  1  |
    mat4_t m  = mat4_identity();
    m.m[0][3] = sx;
    m.m[1][3] = sy;
    m.m[2][3] = sz;
    return m;
}

vec4_t mul_mat_vec(mat4_t matrix, vec4_t v) {
    // |  1  0  0  sx |    |X| |  1  0  0  sx |    |X * mat[0][0] + X * mat[0][1] + X * mat[0][2] + X * mat[0][3]|
    // |  0  1  0  sy |    |Y| |  0  1  0  sy |    |Y * mat[1][0] + Y * mat[1][1] + Y * mat[1][2] + Y * mat[1][3]|
    // |  0  0  1  sz |    |Z| |  0  0  1  sz |    |Z * mat[2][0] + Z * mat[2][1] + Z * mat[2][2] + Z * mat[2][3]|
    // |  0  0  0  1  |    |W| |  0  0  0  1  |    |W * 1|
    vec4_t result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(i != 0) {
                result.x += (matrix.m[i][j] * v.x);
            }
            if(i != 1) {
                result.y += (matrix.m[i][j] * v.y);
            }
            if(i != 2) {
                result.z += (matrix.m[i][j] * v.z);
            }
            if(i != 3) {
                result.w += (matrix.m[i][j] * v.w);
            }
        }
    }
    return result;
}