#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "vector.h"
#include "mesh.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

bool is_running = false;
uint32_t *color_buffer = NULL;

int window_width = 1920;
int window_height = 1080;

int half_width = 1920/2;
int half_height = 1080/2;
int scaling_factor = 400;

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)
int previous_frame_time = 0;

vec3_t rotation = {.x=0, .y=0, .z=0};
vec3_t camera_position = {0,0,0};
int t_cnt = 0;
triangle_t triangles_to_render[N_MESH_FACES];

bool initalize_windowing_system();
void stable_fps();
void process_keyboard_input();
void run_render_pipeline();
void clean_up_windowing_system();
void setup_memory_buffers();
void clear_color_buffer(uint32_t color);
void update_state();
void draw_rect(int pos_x, int pos_y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void bres_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int ver_x1, int vert_y1, int ver_x2, int vert_y2, int ver_x3, int vert_y3, uint32_t color);
void scale_shape();
vec2_t orthographic_project_point(vec3_t point_3d);
vec2_t perspective_project_point(vec3_t point_3d);
uint32_t random_uint32();
void draw_pixel(int x,int y, uint32_t color);
void project_model();

int main(void) {
    is_running = initalize_windowing_system();
    setup_memory_buffers();
    while(is_running) { //game loop
        process_keyboard_input();
        update_state();
        run_render_pipeline();   
    }
    clean_up_windowing_system();
    printf("Hello Professor Z this is Amir");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// Fill our 2d array with projected points
////////////////////////////////////////////////////////////////////////////////////////
void project_model() {
    for(int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];
        vec3_t transformed_vertices[3];
        for(int j = 0; j < 3; j++) { 
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, rotation.z);
            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }
        //Back Face Culling
        vec3_t vertex_a = transformed_vertices[0];
        vec3_t vertex_b = transformed_vertices[1];
        vec3_t vertex_c = transformed_vertices[2];
        //Vector Subtraction
        vec3_t vector_ab = vec3_subtract(vertex_b, vertex_a);
        vec3_t vector_ac = vec3_subtract(vertex_c, vertex_a);
        //Compute Face Normal (using cross product to find perpendiculiar vector)
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        //Find the vector vetween a point in the triangle and the camera origin.
        vec3_t camera_ray = vec3_subtract(camera_position, vertex_a);
        //Calculate how aligned the camera ray is with the face normal (using the dot product)
        float dot_normal_camera = vec3_dot(camera_ray, normal);
        //Ignore Triangle Faces that are looking away from the camera
        if(dot_normal_camera <= 0) {
            continue;
        }
        //Project Triangle
        triangle_t projected_triangle;
        for(int x = 0; x < 3; x++) {
            vec2_t projected_point = perspective_project_point(transformed_vertices[x]);

            projected_triangle.points[x] = projected_point;
        }
        triangles_to_render[t_cnt++] = projected_triangle;
    }
}

void update_state() {
    stable_fps();
    clear_color_buffer(0xFF000000);
    rotation.x += 0.01;
    rotation.y += 0.01;
    rotation.z += 0.01;
    project_model();
    scale_shape();
    for(int i = 0; i < t_cnt; i++) {
        triangle_t triangle = triangles_to_render[i]; 
        draw_rect(triangle.points[0].x+half_width, triangle.points[0].y+half_height, 5, 5, 0xFF00FF00);
        draw_rect(triangle.points[1].x+half_width, triangle.points[1].y+half_height, 5, 5, 0xFF00FF00);
        draw_rect(triangle.points[2].x+half_width, triangle.points[2].y+half_height, 5, 5, 0xFF00FF00);
        draw_triangle(triangle.points[0].x+half_width, triangle.points[0].y+half_height, 
                      triangle.points[1].x+half_width, triangle.points[1].y+half_height,
                      triangle.points[2].x+half_width, triangle.points[2].y+half_height, 0xFF00FF00);
    }
    t_cnt = 0;
}

void draw_triangle(int ver_x1, int vert_y1, int ver_x2, int vert_y2, int ver_x3, int vert_y3, uint32_t color) {
    bres_line(ver_x1, vert_y1, ver_x2, vert_y2, color);
    bres_line(ver_x1, vert_y1, ver_x3, vert_y3, color);
    bres_line(ver_x2, vert_y2, ver_x3, vert_y3, color);
}

void bres_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;) {  /* loop */
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int steps = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : (delta_y);
    float x_inc = delta_x / (float)steps;
    float y_inc = delta_y / (float)steps;
    float x = x0, y = y0;
    for(int i = 0; i <= steps; i++) {
        draw_pixel(x, y, color);
        x += x_inc;
        y += y_inc;
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    // confirm pixel is in the visible window space
    if(x < window_width && y < window_height)
        color_buffer[ (y * window_width) + x ] = color;
}

void scale_shape(){
    for(int i = 0; i < N_MESH_FACES; i++) {
        for(int j = 0; j < 3;  j++) {
            triangles_to_render[i].points[j].x *= scaling_factor;
            triangles_to_render[i].points[j].y *= scaling_factor;
        }
    }
}
//perspective
vec2_t perspective_project_point(vec3_t point_3d) //TODO
{
    vec2_t projected_point = {point_3d.x /point_3d.z,
                              point_3d.y / point_3d.z};
    return projected_point;
}

vec2_t orthographic_project_point(vec3_t point_3d) {
    vec2_t projected_point = {point_3d.x, point_3d.y};
    return projected_point;
}

void draw_rect(int pos_x, int pos_y, int width, int height, uint32_t color) {
     // Check if the rectangle is within the bounds of the window
    if (pos_x < 0 || pos_y < 0 || pos_x + width > window_width || pos_y + height > window_height) {
        printf("Error: Rectangle is out of bounds.\n");
        return;
    }
    //Using loops to draw the rect at desired location with desired color.
    for(int y = pos_y; y < pos_y + height; y++) {
        for(int x =  pos_x; x < pos_x + width; x++) { 
            color_buffer[(y * window_width)+ x] = color;
        }
    }
}

void clear_color_buffer(uint32_t color) {
    for(int y = 0; y < window_height; y++) {
        for(int x = 0; x < window_width; x++) {
            color_buffer[(y * window_width) + x] = color;
        }
    }
}

uint32_t random_uint32() {
    uint32_t result = 0;
    for(int i = 0; i < 4; ++i) {
        result <<= 8;
        result |= (uint32_t)rand() & 0xFF;
    }
    return result;
}

bool initalize_windowing_system() {
    if(SDL_Init(SDL_INIT_EVERYTHING != 0)) {
        fprintf(stderr, "SDL_Init_Failed");
        return false;
    }
    /*SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);	//address of display_mode
    window_width = display_mode.w;
    window_height = display_mode.h;*/

    //go to Real Fullscreen
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);

    if(!window) {
        fprintf(stderr, "Create_Window_Failed");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Create_Render_Failed");
        return false;
    }
    return true;
}

void process_keyboard_input() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
                break;
            }
    }  
}

void setup_memory_buffers() {
    //When navigating the (x,y) coord system to find the value of any (x,y) in a array will be =
    // color_buffer[(y * [max_size_of_x] + x)]
    color_buffer = (uint32_t *) malloc(window_width * window_height * sizeof(uint32_t));
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
}

void run_render_pipeline() {
    SDL_UpdateTexture(texture, NULL, color_buffer, (int) (window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer); //Presents the back-buffer.
}

void clean_up_windowing_system() {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void stable_fps() {
    previous_frame_time = SDL_GetTicks();
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
}