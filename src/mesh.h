#include "vector.h"
#include "triangle.h"

#ifndef MESH_H
#define MESH_H

// Define a constant for The number of vertices for our MESH
#define N_MESH_VERTICES 3 * 12 //TODO ?
//6 cube faces, 2 triangles per face
#define N_MESH_FACES (6 * 2)

// in mesh.h we declare our array of vertices we use
// extern keyword becuase we will initialize and define
// them in mesh.c
extern vec3_t mesh_vertices[N_MESH_VERTICES];

//declare an array of faces
extern face_t mesh_faces[N_MESH_FACES];

#endif