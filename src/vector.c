#include "vector.h" //todo: create function implemenations for vector operations.
#include <math.h>

//Create function

//mul_mat_vec(mat4_t, vec4_t) {
//
//}

////////////////////////////////////////////////////////////////////////////////////////
// 3D Functions ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = ((v.x * cos(angle)) - (v.y * sin(angle))),
        .y = ((v.x * sin(angle)) + (v.y * cos(angle))),
        .z = v.z
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = (v.x * cos(angle) + (v.z * sin(angle))),
        .y = v.y,
        .z = (-v.x * sin(angle) + (v.z * cos(angle)))
    };
    return rotated_vector;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x,
        .y = (v.y * cos(angle) - (v.z * sin(angle))),
        .z = (v.y * sin(angle) + (v.z * cos(angle)))
    };
    return rotated_vector;
}

float vec3_length(vec3_t v) {
   return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vec3_t vec3_add(vec3_t a, vec3_t b) {    
    vec3_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return result;
}

vec3_t vec3_subtract(vec3_t a, vec3_t b) {    
    vec3_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return result;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}
float vec3_dot(vec3_t a, vec3_t b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z *  b.z);
}

////////////////////////////////////////////////////////////////////////////////////////
// 2D Functions ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
vec2_t vec2_add(vec2_t a, vec2_t b) {    
    vec2_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

vec2_t vec2_subtract(vec2_t a, vec2_t b) {    
    vec2_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

vec2_t vec2_mul(vec2_t v, float scalar) {
    vec2_t result = {
        .x = v.x * scalar,
        .y = v.y * scalar
    };
    return result;
}

vec2_t vec2_div(vec2_t v, float scalar) {
    vec2_t result = {
        .x = v.x / scalar,
        .y = v.x / scalar
    };
    return result;
};

float vec2_length(vec2_t v) {
   return sqrt((v.x * v.x) + (v.y * v.y));
}

float vec2_dot(vec2_t a, vec2_t b) {
    return (a.x * b.x) + (a.y * b.y);
}

void normalize_2d(vec2_t* v) {
  float length = sqrt((v->x * v->x) + (v->y * v->y));
  v->x /= length;
  v->y /= length;
}