#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "vector.h"
#include "mesh.h"
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

bool is_running = false;
uint32_t *color_buffer = NULL;

int window_width = 1920;
int window_height = 1080;

int half_width = 1920/2;
int half_height = 1080/2;
int scaling_factor = 600;

#define EXPLOSION_TIME 250
#define EXPLOSION_RADIUS 25
#define NUM_PARTICLES 15
typedef struct {
    int x, y;
    float dx, dy;
} Particle;

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)
int previous_frame_time = 0;
//Cube
#define N_CUBE_POINTS 9 * 9 * 9
vec3_t cube_model[N_CUBE_POINTS];
vec3_t rotation = {.x=0, .y=0, .z=0};
vec3_t cube_rotation = {.x=0, .y=0, .z=0};
vec3_t camera_position = {0,0,-5};
vec2_t velocity = {.x = 1, .y = 1,}; // Initialize velocity
vec2_t projected_points[N_CUBE_POINTS];
//Border Animation Global Variables
int timings[] = {0, -25, -50, -75, -1*(3*60), -1*(5*60), -1*(7*60),-1*(9*60), -1*(11*60), -1*(13*60)};
int scale = 0;

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
void bres_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int ver_x1, int vert_y1, int ver_x2, int vert_y2, int ver_x3, int vert_y3, uint32_t color);
void scale_shape();
vec2_t orthographic_project_point(vec3_t point_3d);
vec2_t perspective_project_point(vec3_t point_3d);
uint32_t random_uint32();
void draw_pixel(int x,int y, uint32_t color);
void fade_color_buffer(uint32_t color);
void animate_border(int timing, uint32_t color);
void project_model();
void first_animation();
void second_animation();
int generateRandomNumber(int lower, int upper);
void draw_circle(int x0, int y0, int radius, uint32_t color);
void draw_circle_frame(int x0, int y0, int radius, uint32_t color);
void countdown();
void update_state_two();
void update_state_two_static();
void init_rectangles();
void update_rectangles();
void third_animation();
void draw_firework_explosion();
void second_animation_static();
void third_animation_static();

#define MAX_RECTANGLES 100
typedef struct {
    int pos_x;
    int pos_y;
    int width;
    int height;
    uint32_t color;
    int speed;
} Rectangle;
Rectangle rectangles[MAX_RECTANGLES];

int main(void) {
    is_running = initalize_windowing_system();
    setup_memory_buffers();
    uint32_t start_time = SDL_GetTicks(); // Get the start time
    while(is_running) { //game loop
        process_keyboard_input();
        if(SDL_GetTicks() - start_time <= 4000) {
            countdown();    
        }
        if(SDL_GetTicks() - start_time >= 4000 && SDL_GetTicks() - start_time <= 37000) {
            first_animation();
        }
        if(SDL_GetTicks() - start_time >= 37000 && SDL_GetTicks() - start_time <= 42000) {
            update_state_two();
        }
        if(SDL_GetTicks() - start_time >= 42000 && SDL_GetTicks() - start_time <= 58000) {
            update_state_two_static();
        }
        if(SDL_GetTicks() - start_time >= 58000 && SDL_GetTicks() - start_time <= 58050) {
            clear_color_buffer(0xFF000000);
        }
        if(SDL_GetTicks() - start_time >= 58051 && SDL_GetTicks() - start_time <= 75000) {
            update_state();
        }
        if(SDL_GetTicks() - start_time >= 75000 && SDL_GetTicks() - start_time <= 95000) {
            second_animation();
        }
        if(SDL_GetTicks() - start_time >= 95000 && SDL_GetTicks() - start_time <= 110000) {
            second_animation_static();
        }
        if(SDL_GetTicks() - start_time >= 110000 && SDL_GetTicks() - start_time <= 135000) {
            third_animation();
        }
        if(SDL_GetTicks() - start_time >= 135000 && SDL_GetTicks() - start_time <= 200000) {
            third_animation_static();
        }
        run_render_pipeline();   
    }
    clean_up_windowing_system();
    printf("Hello Professor Z this is Amir");
    return 0;
}

void first_animation() {
    stable_fps();
    clear_color_buffer(0xFF000000);
    for(int i = 0; i < sizeof(timings) / sizeof(int); i++) {
        animate_border(timings[i], random_uint32()); //0xFF9146FF
        timings[i]++;
    }
}

void second_animation() {
    stable_fps();
    clear_color_buffer(0xFF000000);
    init_rectangles();
    update_rectangles();
}

void second_animation_static() {
    stable_fps();
    init_rectangles();
    update_rectangles();
}

void third_animation() {
    stable_fps();
    clear_color_buffer(0xFF000000);
    draw_firework_explosion();
}

void third_animation_static() {
    stable_fps();
    draw_firework_explosion();
}

void draw_firework_explosion() {
    // Initialize random number generator
    srand(time(NULL));

    // Generate random position for the explosion
    int pos_x = rand() % window_width;
    int pos_y = rand() % window_height;

    // Create particles
    Particle particles[NUM_PARTICLES];
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x = pos_x;
        particles[i].y = pos_y;

        // Generate random velocity for the particle
        float angle = 2 * M_PI * (float)rand() / RAND_MAX;
        float speed = (0.5 + ((float)rand() / RAND_MAX) / 2) * EXPLOSION_RADIUS; // Adjust speed for size of explosion
        particles[i].dx = speed * cos(angle);
        particles[i].dy = speed * sin(angle);
    }

    // Animate the explosion
    for (int t = 0; t < EXPLOSION_TIME; t++) {
        for (int i = 0; i < NUM_PARTICLES; i++) {
            // Update particle position
            particles[i].x += particles[i].dx;
            particles[i].y += particles[i].dy;

            // Check if the particle is within the window's width and height before drawing
            if (particles[i].x >= 0 && particles[i].x < window_width && particles[i].y >= 0 && particles[i].y < window_height) {
                // Draw the particle
                bres_line(pos_x, pos_y, particles[i].x, particles[i].y, random_uint32()); // White color
            }

            // Slowly decrease the speed of particles to simulate gravity and friction
            particles[i].dx *= 0.99;
            particles[i].dy *= 0.99;
        }
    }

    // Clear the explosion
    draw_rect(pos_x - EXPLOSION_RADIUS, pos_y - EXPLOSION_RADIUS, 2 * EXPLOSION_RADIUS, 2 * EXPLOSION_RADIUS, 0x000000); // Black color
}


void init_rectangles() {
    for(int i = 0; i < MAX_RECTANGLES; i++) {
        rectangles[i].pos_x = rand() % window_width; // Random x position
        rectangles[i].pos_y = rand() % window_height; // Random y position
        rectangles[i].width = 5; // Width of the rectangle
        rectangles[i].height = 5; // Height of the rectangle
        rectangles[i].color = random_uint32(); // Random color
        rectangles[i].speed = rand() % 2 + 1; // Random speed between 1 and 2
    }
}

void update_rectangles() {
    for(int i = 0; i < MAX_RECTANGLES; i++) {
        rectangles[i].pos_y += rectangles[i].speed; // Move the rectangle down
        // If the rectangle is out of the window, reset its position and speed
        if(rectangles[i].pos_y > window_height) {
            rectangles[i].pos_y = -rectangles[i].height;
            rectangles[i].speed = rand() % 2 + 1;
        }
        // Check if the rectangle is within the window's width and height before drawing
        if (rectangles[i].pos_x >= 0 && rectangles[i].pos_x + rectangles[i].width <= window_width && rectangles[i].pos_y >= 0 && rectangles[i].pos_y + rectangles[i].height <= window_height) {
            draw_rect(rectangles[i].pos_x, rectangles[i].pos_y, rectangles[i].width, rectangles[i].height, rectangles[i].color);
        }
    }
}

void draw_three(int x, int y, int size, uint32_t color) {
    draw_rect(x, y, size, size, color); // Top horizontal bar
    draw_rect(x + size, y, size, size * 2, color); // Right vertical bar
    draw_rect(x, y + size * 2, size, size, color); // Middle horizontal bar
    draw_rect(x + size, y + size * 3, size, size * 2, color); // Right vertical bar
    draw_rect(x, y + size * 5, size, size, color); // Bottom horizontal bar
}

void draw_two(int x, int y, int size, uint32_t color) {
    draw_rect(x, y, size * 2, size, color); // Top horizontal bar
    draw_rect(x + size, y + size, size, size * 2, color); // Right vertical bar
    draw_rect(x, y + size * 3, size * 2, size, color); // Middle horizontal bar
    draw_rect(x, y + size * 4, size, size * 2, color); // Left vertical bar
    draw_rect(x, y + size * 6, size * 2, size, color); // Bottom horizontal bar
}

void draw_one(int x, int y, int size, uint32_t color) {
    draw_rect(x + size, y, size, size * 7, color); // Vertical bar
}

void draw_go(int x, int y, int size, uint32_t color) {
    // G
    draw_rect(x, y, size, size * 7, color); // G - vertical bar
    draw_rect(x, y, size * 3, size, color); // G - top horizontal bar
    draw_rect(x, y + size * 6, size * 3, size, color); // G - bottom horizontal bar
    draw_rect(x + size * 2, y + size * 3, size, size * 3, color); // G - inner vertical bar
    draw_rect(x + size * 2, y + size * 3, size * 2, size, color); // G - inner horizontal bar

    // O
    draw_rect(x + size * 4 + 20, y, size, size * 7, color); // O - left vertical bar
    draw_rect(x + size * 4 + 20, y, size * 3, size, color); // O - top horizontal bar
    draw_rect(x + size * 4 + 20, y + size * 6, size * 3, size, color); // O - bottom horizontal bar
    draw_rect(x + size * 6 + 20, y, size, size * 7, color); // O - right vertical bar
}

void countdown() {
    int size = 50; // Size of each rectangle
    int offset = 100;
    uint32_t color = 0xFF9146FF; // White color
    static uint32_t start_time = 0; // Get the start time

    if (start_time == 0) {
        // If this is the first call to countdown, initialize start_time
        start_time = SDL_GetTicks();
    }

    uint32_t elapsed_time = SDL_GetTicks() - start_time;
    clear_color_buffer(0xFF000000);
    if(elapsed_time <= 1000) {
        draw_three(window_width / 2 - offset, window_height / 2 - offset, size, color);
    }
    else if(elapsed_time <= 2000) {
        draw_two(window_width / 2 - offset, window_height / 2 - offset, size, color);
    }
    else if(elapsed_time <= 3000) {
        draw_one(window_width / 2 - offset, window_height / 2 - offset, size, color);
    }
    else if(elapsed_time <= 4000) {
        draw_go(window_width / 2 - offset, window_height / 2 - offset, size, color);
    }
}

// Generates a random number within the specified range
int generateRandomNumber(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void draw_circle_frame(int x0, int y0, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        draw_pixel(x0 - y, y0 + x, color);
        draw_pixel(x0 + y, y0 + x, color);
        draw_pixel(x0 - x, y0 + y, color);
        draw_pixel(x0 + x, y0 + y, color);
        draw_pixel(x0 - x, y0 - y, color);
        draw_pixel(x0 + x, y0 - y, color);
        draw_pixel(x0 - y, y0 - x, color);
        draw_pixel(x0 + y, y0 - x, color);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void draw_circle(int x0, int y0, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        bres_line(x0 - y, y0 + x, x0 + y, y0 + x, color);
        bres_line(x0 - x, y0 + y, x0 + x, y0 + y, color);
        bres_line(x0 - x, y0 - y, x0 + x, y0 - y, color);
        bres_line(x0 - y, y0 - x, x0 + y, y0 - x, color);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void update_state() {
    stable_fps();
    rotation.x += 0.01;
    rotation.y += 0.01;
    rotation.z += 0.01;
    project_model();
    scale_shape();
    for(int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x + (window_width / 2) - (scale++), triangle.points[0].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[1].x + (window_width / 2) - scale++, triangle.points[1].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[2].x + (window_width / 2) - scale++, triangle.points[2].y+(window_height / 2), 5, 5, random_uint32());
        draw_triangle(triangle.points[0].x + (window_width / 2) - scale++, triangle.points[0].y+(window_height / 2),
                      triangle.points[1].x + (window_width / 2) - scale++, triangle.points[1].y+(window_height / 2),
                      triangle.points[2].x + (window_width / 2) - scale++, triangle.points[2].y+(window_height / 2), random_uint32());
    }
}

void update_state_two() {
    stable_fps();
    clear_color_buffer(0xFF000000);
    rotation.x += 0.01;
    rotation.y += 0.01;
    rotation.z += 0.01;
    project_model();
    scale_shape();
    for(int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x + (window_width / 2), triangle.points[0].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[1].x + (window_width / 2), triangle.points[1].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[2].x + (window_width / 2) , triangle.points[2].y+(window_height / 2), 5, 5, random_uint32());
        draw_triangle(triangle.points[0].x + (window_width / 2) , triangle.points[0].y+(window_height / 2),
                      triangle.points[1].x + (window_width / 2) , triangle.points[1].y+(window_height / 2),
                      triangle.points[2].x + (window_width / 2) , triangle.points[2].y+(window_height / 2), random_uint32());
    }
}

void update_state_two_static() {
    stable_fps();
    rotation.x += 0.01;
    rotation.y += 0.01;
    rotation.z += 0.01;
    project_model();
    scale_shape();
    for(int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x + (window_width / 2), triangle.points[0].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[1].x + (window_width / 2), triangle.points[1].y+(window_height / 2), 5, 5, random_uint32());
        draw_rect(triangle.points[2].x + (window_width / 2) , triangle.points[2].y+(window_height / 2), 5, 5, random_uint32());
        draw_triangle(triangle.points[0].x + (window_width / 2) , triangle.points[0].y+(window_height / 2),
                      triangle.points[1].x + (window_width / 2) , triangle.points[1].y+(window_height / 2),
                      triangle.points[2].x + (window_width / 2) , triangle.points[2].y+(window_height / 2), random_uint32());
    }
}

void animate_border(int timing, uint32_t color) {
    if(timing < (window_height/2)) {
        draw_rect(timing, timing, window_width-5-(timing*2), 5, color); //Top Border
        draw_rect(timing, window_height-5 - timing, window_width-(timing*2), 5, color); //Bottom Border
        draw_rect(timing, timing, 5, window_height - timing*2, color); //Left Border
        draw_rect(window_width-5 - timing, timing, 5, window_height-timing*2, color); //Right Border
    }
}

void fade_color_buffer(uint32_t color) {
    uint32_t faded_color = ((color & 0xFEFEFEFE) >> 1) & 0x7F7F7F7F; // This will reduce the brightness by half.
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = faded_color;
        }
    }
}


void project_model() {
    for(int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        triangle_t projected_triangle;
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];
        for(int j = 0; j < 3; j++) {
            vec3_t transformed_vertices = face_vertices[j];
            transformed_vertices = vec3_rotate_x(transformed_vertices, rotation.x);
            transformed_vertices = vec3_rotate_y(transformed_vertices, rotation.y);
            transformed_vertices = vec3_rotate_z(transformed_vertices, rotation.z);
            transformed_vertices.z -= camera_position.z;
            vec2_t vertex = perspective_project_point(transformed_vertices);
            projected_triangle.points[j] = vertex;
        }
        triangles_to_render[i] = projected_triangle;
    }
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
        //printf("Error: Rectangle is out of bounds.\n");
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

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); //go to Real Fullscreen
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