#include "include.h"

ChunkMesh* gen_chunk_mesh(Vector3 world_pos) {

    ChunkMesh* chunk_mesh = calloc(1, sizeof(ChunkMesh));
    
    chunk_mesh->chunk = (Chunk*)calloc(1,sizeof(Chunk));
    chunk_mesh->chunk = gen_chunk(world_pos.x, world_pos.y, world_pos.z);

    int num_blocks_in_chunk = CHUNK_CUBED;
    int num_block_vertices = sizeof(chunk_mesh->chunk->blocks[0]->vertices) / sizeof(chunk_mesh->chunk->blocks[0]->vertices[0]);
    int num_block_texcoords = sizeof(chunk_mesh->chunk->blocks[0]->texcoords) / sizeof(chunk_mesh->chunk->blocks[0]->texcoords[0]);
    int num_block_normals = sizeof(chunk_mesh->chunk->blocks[0]->normals) / sizeof(chunk_mesh->chunk->blocks[0]->normals[0]);

    int Vcounter = 0;
    int Tcounter = 0;
    int Ncounter = 0;

    for (int i = 0; i < CHUNK_CUBED; i++) {
        //add up all the arrays
        //adding up vertices
        for (int k = 0; k < num_block_vertices; k++) {
            chunk_mesh->chunk->total_vertices[Vcounter++] = chunk_mesh->chunk->blocks[i]->vertices[k];
        }
        for (int k = 0; k < num_block_texcoords; k++) {
            chunk_mesh->chunk->total_texcoords[Tcounter++] = chunk_mesh->chunk->blocks[i]->texcoords[k];
        }
        for (int k = 0; k < num_block_normals; k++) {
            chunk_mesh->chunk->total_normals[Ncounter++] = chunk_mesh->chunk->blocks[i]->normals[k];
        }

    }

    int num_chunk_vertices = num_block_vertices * num_blocks_in_chunk;
    int num_chunk_texcoords = num_block_texcoords * num_blocks_in_chunk;
    int num_chunk_normals = num_block_normals * num_blocks_in_chunk;

    //put it all together in a chunk mesh
    chunk_mesh->mesh = (Mesh*)calloc(1,sizeof(Mesh));

    chunk_mesh->mesh->vertices = (float *)calloc(1, num_chunk_vertices * sizeof(float));
    memcpy(chunk_mesh->mesh->vertices, chunk_mesh->chunk->total_vertices, num_chunk_vertices * sizeof(float));
    
    chunk_mesh->mesh->texcoords = (float *)calloc(1, num_chunk_texcoords * sizeof(float));
    memcpy(chunk_mesh->mesh->texcoords, chunk_mesh->chunk->total_texcoords, num_chunk_texcoords * sizeof(float));

    chunk_mesh->mesh->normals = (float *)calloc(1, num_chunk_normals * sizeof(float));
    memcpy(chunk_mesh->mesh->normals, chunk_mesh->chunk->total_normals, num_chunk_normals * sizeof(float));

    chunk_mesh->mesh->vertexCount = chunk_mesh->chunk->total_vertex_count;
    chunk_mesh->mesh->triangleCount = chunk_mesh->chunk->total_triangle_count;

    return chunk_mesh;
}

Chunk* gen_chunk(int worldX, int worldY, int worldZ) {
    //Chunk chunk = { 0 };
    Chunk* chunk = (Chunk*)calloc(1,sizeof(Chunk));

    chunk->world_pos = (Vector3) { worldX, worldY, worldZ };

    int block_counter = 0;
    chunk->total_vertex_count = 0;
    chunk->total_triangle_count = 0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        //create all the conceptual blocks
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                //Block* block = gen_block(chunk.world_pos, i, j, k);
                Block* block = (Block*)calloc(1, sizeof(Block));
                block = gen_block(chunk->world_pos, i, j, k);

                chunk->total_vertex_count += 36;
                chunk->total_triangle_count += 12;

                chunk->blocks[block_counter] = block;      
                block_counter++;

            }
        }
    }

    return chunk;
}

Block* gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ) {
    Block* block = (Block*)calloc(1, sizeof(Block));
    block->pos.x = world_pos.x - HALF_CHUNK + blockX;
    block->pos.y = world_pos.y - HALF_CHUNK + blockY;
    block->pos.z = world_pos.z - HALF_CHUNK + blockZ;

    //some basic culling
    if(blockX == 0 || blockY == 0 || blockZ == 0) {
        block->block_type = BLOCK_MAGMA;
    }else if (blockX == CHUNK_SIZE-1 || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1) {
        block->block_type = BLOCK_MAGMA;
    } else {
        block->block_type = BLOCK_AIR;
        return block;
    }

    float x = block->pos.x;
    float y = block->pos.y;
    float z = block->pos.z;

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

    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;
    // float u_min = DIRT_LIGHT_TEX_COORD_U_MIN;
    // float u_max = DIRT_LIGHT_TEX_COORD_U_MAX;
    // float v_min = DIRT_LIGHT_TEX_COORD_V_MIN;
    // float v_max = DIRT_LIGHT_TEX_COORD_V_MAX;

    float texcoords[] = {
        // u_min, v_max,
        // u_max, v_max,
        // u_max, v_min,
        // u_min, v_min,

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

    memcpy(block->vertices, vertices, 36*3*sizeof(float));
    
    memcpy(block->texcoords, texcoords, 36*2*sizeof(float));

    memcpy(block->normals, normals, 36*3*sizeof(float));

    block->vertexCount = 36;
    block->triangleCount = 12;

    return block;
}
