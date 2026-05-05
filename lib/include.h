#include "raylib.h" 
#include "rlgl.h"
#include "raymath.h"

#include <stdlib.h>
#include <string.h>


#ifndef INCLUDE_H
#define INCLUDE_H

#define CHUNK_SIZE 16
#define HALF_CHUNK CHUNK_SIZE/2
#define CHUNK_CUBED (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)
#define BLOCK_AIR 0
#define BLOCK_GRASS 1
#define BLOCK_DIRT 2
#define BLOCK_STONE 3
#define BLOCK_LAVA 4
#define BLOCK_MAGMA 5

#define DIRT_TEX_COORD_U_MIN 0.05f
#define DIRT_TEX_COORD_U_MAX 0.18f
#define DIRT_TEX_COORD_V_MIN 0.82f
#define DIRT_TEX_COORD_V_MAX 0.98f
#define DIRT_LIGHT_TEX_COORD_U_MIN 0.22f
#define DIRT_LIGHT_TEX_COORD_U_MAX 0.38f
#define DIRT_LIGHT_TEX_COORD_V_MIN 0.22f
#define DIRT_LIGHT_TEX_COORD_V_MAX 0.38f
#define GRASS_TEX_COORD_U_MIN 0.22f
#define GRASS_TEX_COORD_U_MAX 0.38f
#define GRASS_TEX_COORD_V_MIN 0.42f
#define GRASS_TEX_COORD_V_MAX 0.58f
#define GRASS_LIGHT_TEX_COORD_U_MIN 0.42f
#define GRASS_LIGHT_TEX_COORD_U_MAX 0.58f
#define GRASS_LIGHT_TEX_COORD_V_MIN 0.42f
#define GRASS_LIGHT_TEX_COORD_V_MAX 0.58f
#define LAVA_TEX_COORD_U_MIN 0.82f
#define LAVA_TEX_COORD_U_MAX 0.98f
#define LAVA_TEX_COORD_V_MIN 0.42f
#define LAVA_TEX_COORD_V_MAX 0.58f

#define MAGMA_TEX_COORD_U_MIN 0.42f
#define MAGMA_TEX_COORD_U_MAX 0.58f
#define MAGMA_TEX_COORD_V_MIN 0.82f
#define MAGMA_TEX_COORD_V_MAX 0.98f


/*
*   TO DO
*   - Get texture atlas to work on one block
*   - Get meshing to work for entire chunk
*   - Atlas working for entire chunk
*   - Then chunking system
*/



typedef struct Block {
    int block_type;
    Vector3 pos;
    float tex_coord_u_min;
    float tex_coord_u_max;
    float tex_coord_v_min;
    float tex_coord__v_max;
    float vertices[36*3];
    float texcoords[36*2];
    float normals[36*3];
    float vertexCount;
    float triangleCount;

} Block;

typedef struct Chunk {
    Vector3 world_pos;
    float total_vertices[CHUNK_CUBED * 36 * 3];
    float total_texcoords[CHUNK_CUBED * 36 * 2];
    float total_normals[CHUNK_CUBED * 36 * 3];
    int total_vertex_count;
    int total_triangle_count;
    Block blocks[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

} Chunk;

typedef struct ChunkMesh {
    Mesh* mesh;
    Chunk* chunk;
} ChunkMesh;


ChunkMesh* gen_chunk_mesh(Vector3 world_pos);
Chunk* gen_chunk(int worldX, int worldY, int worldZ);
Block* gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ);


#endif
