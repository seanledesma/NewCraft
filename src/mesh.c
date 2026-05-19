#include "include.h"

void GenMeshChunk(Mesh* mesh, Chunk* chunk) {
    int block_counter = 0;
    // if you see all magma, something went wrong
    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;

    for (int blockX = 0; blockX < CHUNK_SIZE; blockX++) {
        for (int blockY = 0; blockY < CHUNK_SIZE; blockY++) {
            for (int blockZ = 0; blockZ < CHUNK_SIZE; blockZ++) {
                
                if(chunk->blocks[blockX][blockY][blockZ].block_type == BLOCK_AIR) {
                    mesh->vertexCount += 36;
                    mesh->triangleCount += 12;
                    continue;
                }
                if(chunk->blocks[blockX][blockY][blockZ].block_type == BLOCK_GRASS) {
                    u_min = GRASS_LIGHT_TEX_COORD_U_MIN;
                    u_max = GRASS_LIGHT_TEX_COORD_U_MAX;
                    v_min = GRASS_LIGHT_TEX_COORD_V_MIN;
                    v_max = GRASS_LIGHT_TEX_COORD_V_MAX;
                }
                if(chunk->blocks[blockX][blockY][blockZ].block_type == BLOCK_DIRT) {
                    u_min = DIRT_DARK_TEX_COORD_U_MIN;
                    u_max = DIRT_DARK_TEX_COORD_U_MAX;
                    v_min = DIRT_DARK_TEX_COORD_V_MIN;
                    v_max = DIRT_DARK_TEX_COORD_V_MAX;
                }
                if(chunk->blocks[blockX][blockY][blockZ].block_type == BLOCK_STONE) {
                    u_min = STONE_TEX_COORD_U_MIN;
                    u_max = STONE_TEX_COORD_U_MAX;
                    v_min = STONE_TEX_COORD_V_MIN;
                    v_max = STONE_TEX_COORD_V_MAX;
                }

                float x = chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f;
                float y = chunk->world_pos.y - HALF_CHUNK + blockY;
                float z = chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

                float size = 0.5f;

                float vertices[] = {
                    //first front triangle
                    //(counter clock wise starting with bottom left)
                    x-size, y-size, z+size,
                    x+size, y+size, z+size,
                    x-size, y+size, z+size,
                    //second front triangle
                    x-size, y-size, z+size,
                    x+size, y-size, z+size,
                    x+size, y+size, z+size,

                    //first back triangle
                    x+size, y-size, z-size,
                    x-size, y+size, z-size,
                    x+size, y+size, z-size,

                    //second back triangle
                    x+size, y-size, z-size,
                    x-size, y-size, z-size, 
                    x-size, y+size, z-size,

                    //top triangles
                    x-size, y+size, z+size,
                    x+size, y+size, z-size,
                    x-size, y+size, z-size,

                    x-size, y+size, z+size,
                    x+size, y+size, z+size,
                    x+size, y+size, z-size,

                    //bottom
                    x-size, y-size, z-size,
                    x+size, y-size, z+size,
                    x-size, y-size, z+size,

                    x-size, y-size, z-size,
                    x+size, y-size, z-size, 
                    x+size, y-size, z+size,

                    //left
                    x-size, y-size, z-size,
                    x-size, y+size, z+size,
                    x-size, y+size, z-size,

                    x-size, y-size, z-size,
                    x-size, y-size, z+size,
                    x-size, y+size, z+size,

                    //right
                    x+size, y-size, z+size,
                    x+size, y+size, z-size,
                    x+size, y+size, z+size,

                    x+size, y-size, z+size,
                    x+size, y-size, z-size,
                    x+size, y+size, z-size,
                };

                float texcoords[] = {
                    //front
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //back
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //top
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //bottom
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //left
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //right
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,


                };
                float zero = 0.0f;
                float one = 1.0f;
                float normals[] = { 
                    //front
                    zero, zero, one,
                    zero, zero, one,
                    zero, zero, one,

                    zero, zero, one,
                    zero, zero, one,
                    zero, zero, one,
                    //back
                    zero, zero, -one,
                    zero, zero, -one,
                    zero, zero, -one,

                    zero, zero, -one,
                    zero, zero, -one,
                    zero, zero, -one,
                    //top
                    zero, one, zero,
                    zero, one, zero,
                    zero, one, zero,

                    zero, one, zero,
                    zero, one, zero,
                    zero, one, zero,
                    //bottom
                    zero, -one, zero,
                    zero, -one, zero,
                    zero, -one, zero,

                    zero, -one, zero,
                    zero, -one, zero,
                    zero, -one, zero,
                    //left
                    -one, zero, zero,
                    -one, zero, zero,
                    -one, zero, zero,

                    -one, zero, zero,
                    -one, zero, zero,
                    -one, zero, zero,
                    //right
                    one, zero, zero,
                    one, zero, zero,
                    one, zero, zero,

                    one, zero, zero,
                    one, zero, zero,
                    one, zero, zero,
                    
                };

                int vert_count = block_counter * (36 * 3);
                int tex_count = block_counter * (36 * 2);
                int norm_count = block_counter * (36 * 3);

                memcpy(mesh->vertices + vert_count, vertices, 36*3*sizeof(float));
                
                memcpy(mesh->texcoords + tex_count, texcoords, 36*2*sizeof(float));

                memcpy(mesh->normals + norm_count, normals, 36*3*sizeof(float));

                mesh->vertexCount += 36;
                mesh->triangleCount += 12;

                block_counter++;
            }
        }
    }
}
