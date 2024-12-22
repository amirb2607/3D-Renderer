#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "vector.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

bool is_running = false;
uint32_t *color_buffer = NULL;

int window_width = 1920;
int window_height = 1080;
int scaling_factor = 500;

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)
int previous_frame_time = 0;

#define N_CUBE_POINTS 9 * 9 * 9
vec3_t cube_model[N_CUBE_POINTS];
vec3_t cube_rotation = {.x=0, .y=0, .z=0};
vec3_t camera_position = {0,0,-5};

vec2_t projected_points[N_CUBE_POINTS];

bool initalize_windowing_system();
void process_keyboard_input();
void run_render_pipeline();
void clean_up_windowing_system();
void setup_memory_buffers();
void clear_color_buffer(uint32_t color);
void update_state();
void draw_grid(int grid_size, uint32_t color);
void draw_rect(int pos_x, int pos_y, int width, int height, uint32_t color);
void draw_dots(int total_dots);
void scale_shape();
vec2_t orthographic_project_point(vec3_t point_3d);
vec2_t perspective_project_point(vec3_t point_3d);
void build_cube_model();
uint32_t random_uint32();
void draw_pixel(int x,int y, uint32_t color);
void project_model();

int main(void) {
    is_running = initalize_windowing_system();
    setup_memory_buffers();
    build_cube_model();
    while(is_running) {
        //game loop
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
    for (int i = 0; i < N_CUBE_POINTS; i++) {
        vec3_t point = cube_model[i];
        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
        transformed_point.z -= camera_position.z;
        //vec2_t projected_point = orthographic_project_point(point);
        vec2_t projected_point = perspective_project_point(transformed_point);
        projected_points[i] = projected_point;
    }
}

void update_state() {
    previous_frame_time = SDL_GetTicks();
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    clear_color_buffer(0xFF000000);
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;
    project_model();
    scale_shape();
    for(int i = 0; i < N_CUBE_POINTS; i++) {
        vec2_t point = projected_points[i];
        draw_rect((point.x + (window_width / 2)), (point.y + (window_height / 2)), 4, 4, 0xFF00FFFF);
        //draw_rect(point.x + (window_width / 2), point.y + (window_height / 2), 4, 4, random_uint32());
    }
}

void scale_shape(){
    for(int i = 0; i < N_CUBE_POINTS; i++) {
        projected_points[i].x *= scaling_factor;
        projected_points[i].y *= scaling_factor;
    }
}
//perspective
vec2_t perspective_project_point(vec3_t point_3d) //TODO
{
    vec2_t projected_point = {point_3d.x /point_3d.z, point_3d.y / point_3d.z};
    return projected_point;
}

vec2_t orthographic_project_point(vec3_t point_3d) {
    vec2_t projected_point = {point_3d.x, point_3d.y};
    return projected_point;
}

void build_cube_model() {
    int point_count = 0;
    for(float x = -1; x <= 1; x += 0.25)
        for(float y = -1; y <= 1; y += 0.25)
            for(float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_model[point_count++] = new_point;
            }
}

void draw_pixel(int x, int y, uint32_t color) {
    // confirm pixel is in the visible window space
    if(x < window_width && y < window_height)
        color_buffer[ (y * window_width) + x ] = color;
}

void draw_dots(int total_dots) {
    srand(time(NULL));

    for(int i = 0; i < total_dots; i++) {
        int x = rand() % window_width;
        int y = rand() % window_height;
        color_buffer[(y * window_width )+ x] = random_uint32();
    }
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

void draw_grid(int grid_size, uint32_t color) {
    //Draws the vertical lines.
    for(int x = 0; x < window_width; x += grid_size) {
        for(int y = 0; y < window_height; y++) {
            color_buffer[(y * window_width )+ x] = color;
        }
    }

    for (int y = 0; y < window_height; y += grid_size) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(y * window_width) + x] = color;
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