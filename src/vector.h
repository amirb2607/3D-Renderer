#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

typedef struct {
    vec3_t position;
    vec3_t rotation;
    float fov_angle;
} camera_t;

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
vec3_t vec3_subtract(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);
float vec3_length(vec3_t v);
////////////////////////////////////////////////////////////////////////////////////////
//Declarations of Vector 2d operation functions
////////////////////////////////////////////////////////////////////////////////////////
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);  
vec2_t vec2_subtract(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t v, float scalar);
void normalize_2d(vec2_t* v);
////////////////////////////////////////////////////////////////////////////////////////
//Declartions of Vector 3d operation functions
////////////////////////////////////////////////////////////////////////////////////////

#endif