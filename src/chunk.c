#include "include.h"

// here we create and return a chunk_mesh pointer which has two nested pointers,
// one to what is returned by gen_chunk and the other to the mesh we create in this 
// function using the pointer to the chunk returned by gen_chunk.
ChunkMesh* gen_chunk_mesh(Vector3 world_pos, HashTable* hash_table) {

    ChunkMesh* chunk_mesh = calloc(1, sizeof(ChunkMesh));
    
    chunk_mesh->chunk = (Chunk*)calloc(1,sizeof(Chunk));
    // int32_t Xpos = (int32_t)floor(world_pos.x);
    // int32_t Ypos = (int32_t)floor(world_pos.y);
    // int32_t Zpos = (int32_t)floor(world_pos.z);
    //chunk_mesh->chunk = gen_chunk(Xpos, Ypos, Zpos);

    // i should make sure all new chunks are on multiples of sixteen
    // int Xpos = floor((world_pos.x + 8) / 16) * 16;
    // int Ypos = floor(((world_pos.y - PLAYER_HEIGHT) + 8) / 16) * 16;
    // int Zpos = floor((world_pos.z + 8) / 16) * 16;

    TraceLog(LOG_WARNING, "gen chunk with world y: %.2f", world_pos.y);
    //int32_t Ypos = (int32_t)floor(world_pos.y);
    //TraceLog(LOG_WARNING, "gen chunk with world y after floor(): %d", Ypos);
    
    //chunk_mesh->chunk = gen_chunk(world_pos);

    int num_blocks_in_chunk = CHUNK_CUBED;

    int num_block_vertices = 36 * 3;
    int num_block_texcoords = 36 * 2;
    int num_block_normals = 36 * 3;

    int num_chunk_vertices = num_block_vertices * num_blocks_in_chunk;
    int num_chunk_texcoords = num_block_texcoords * num_blocks_in_chunk;
    int num_chunk_normals = num_block_normals * num_blocks_in_chunk;

    //put it all together in a chunk mesh
    chunk_mesh->mesh = (Mesh*)MemAlloc(sizeof(Mesh));

    chunk_mesh->mesh->vertices = (float *)MemAlloc(num_chunk_vertices * sizeof(float));
    //memcpy(chunk_mesh->mesh->vertices, chunk_mesh->chunk->total_vertices, num_chunk_vertices * sizeof(float));
    
    chunk_mesh->mesh->texcoords = (float *)MemAlloc(num_chunk_texcoords * sizeof(float));
    //memcpy(chunk_mesh->mesh->texcoords, chunk_mesh->chunk->total_texcoords, num_chunk_texcoords * sizeof(float));

    chunk_mesh->mesh->normals = (float *)MemAlloc(num_chunk_normals * sizeof(float));
    //memcpy(chunk_mesh->mesh->normals, chunk_mesh->chunk->total_normals, num_chunk_normals * sizeof(float));

    // chunk_mesh->mesh->vertexCount = chunk_mesh->chunk->total_vertex_count;
    // chunk_mesh->mesh->triangleCount = chunk_mesh->chunk->total_triangle_count;

    chunk_mesh->chunk = gen_chunk(world_pos, chunk_mesh->mesh, hash_table);

    Mesh* temp_mesh = (Mesh*)MemAlloc(sizeof(Mesh));

    int total_vertex_count = chunk_mesh->mesh->vertexCount * 3;
    int total_tex_coords = (total_vertex_count / 3) * 2;
    int total_normal_count = total_vertex_count;

    temp_mesh->vertices = (float *)MemRealloc(chunk_mesh->mesh->vertices, total_vertex_count * sizeof(float));
    
    if (temp_mesh->vertices == NULL) {
        TraceLog(LOG_ERROR, "temp mesh in gen_chunk_mesh is NULL");
    } else {
        chunk_mesh->mesh->vertices = temp_mesh->vertices;
    }

    // now tex coords
    
    temp_mesh->texcoords = (float *)MemRealloc(chunk_mesh->mesh->texcoords, total_tex_coords * sizeof(float));
    
    if (temp_mesh->texcoords == NULL) {
        TraceLog(LOG_ERROR, "temp mesh in gen_chunk_mesh is NULL");
    } else {
        chunk_mesh->mesh->texcoords = temp_mesh->texcoords;
    }

    temp_mesh->normals = (float *)MemRealloc(chunk_mesh->mesh->normals, total_normal_count * sizeof(float));
    
    if (temp_mesh->normals == NULL) {
        TraceLog(LOG_ERROR, "temp mesh in gen_chunk_mesh is NULL");
    } else {
        chunk_mesh->mesh->normals = temp_mesh->normals;
    }


    return chunk_mesh;
}

// takes chunk world position to pass to block gen function. 
// we create and return a pointer to a chunk strut which now
// has blocks array full of pointers to blocks created in gen_block.
Chunk* gen_chunk(Vector3 world_pos, Mesh* mesh, HashTable* hash_table) {
    //Chunk chunk = { 0 };
    Chunk* chunk = (Chunk*)calloc(1,sizeof(Chunk));

    chunk->world_pos = world_pos;

    int block_counter = 0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        //create all the conceptual blocks
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                //Block* block = gen_block(chunk.world_pos, i, j, k);
                //Block block = (Block*)calloc(1, sizeof(Block));
                //chunk->blocks[block_counter] = gen_block(world_pos, i, j, k, mesh, block_counter, hash_table);
                chunk->blocks[i][j][k] = gen_block(world_pos, i, j, k, mesh, block_counter, hash_table);

                //chunk->total_vertex_count += 36;
                //chunk->total_triangle_count += 12;

                //chunk->blocks[block_counter] = block;      
                block_counter++;

            }
        }
    }
    TraceLog(LOG_WARNING, "hit gen chunk");
    return chunk;
}

// takes world position and three loop positions to get block position.
// culling should take place here since block textures are set here.
// vertices are set from top left triangle, then bottom right triangle,
// in a counter clockwise rotation starting from bottom left corner.
// tex coords were not too hard, just upside down due to difference between
// png and opengl. This is the lowest it goes, we return a pointer to the 
// data generated here.
Block gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ, Mesh* mesh, int counter, HashTable* hash_table) {
    //Block* block = (Block*)calloc(1, sizeof(Block));
    float posX = world_pos.x - HALF_CHUNK + blockX + 0.5f; //have to add 0.5 so it lines up.. for reasons..
    float posY = world_pos.y - HALF_CHUNK + blockY;
    float posZ = world_pos.z - HALF_CHUNK + blockZ + 0.5f;
    Vector3 blockpos = (Vector3){posX, posY, posZ};

    Block block = {0};
    block.block_type = DecideBlockType(blockpos);
    

    if(IsBlockVisible(world_pos, blockpos, blockX, blockY, blockZ, hash_table) == false) {
        block.block_type = BLOCK_AIR;
    }

    if (block.block_type == BLOCK_AIR) {
        //bruh
        mesh->vertexCount += 36;
        mesh->triangleCount += 12;
        return block;
    }

    // if you see all magma, something went wrong
    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;

    if(block.block_type == BLOCK_GRASS) {
        u_min = GRASS_LIGHT_TEX_COORD_U_MIN;
        u_max = GRASS_LIGHT_TEX_COORD_U_MAX;
        v_min = GRASS_LIGHT_TEX_COORD_V_MIN;
        v_max = GRASS_LIGHT_TEX_COORD_V_MAX;
    }
    if(block.block_type == BLOCK_DIRT) {
        u_min = DIRT_DARK_TEX_COORD_U_MIN;
        u_max = DIRT_DARK_TEX_COORD_U_MAX;
        v_min = DIRT_DARK_TEX_COORD_V_MIN;
        v_max = DIRT_DARK_TEX_COORD_V_MAX;
    }
    if(block.block_type == BLOCK_STONE) {
        u_min = STONE_TEX_COORD_U_MIN;
        u_max = STONE_TEX_COORD_U_MAX;
        v_min = STONE_TEX_COORD_V_MIN;
        v_max = STONE_TEX_COORD_V_MAX;
    }
    // if(block.block_type == BLOCK_AIR) {
    //     u_min = 0;
    //     u_max = 0;
    //     v_min = 0;
    //     v_max = 0;
    // }


    float x = blockpos.x;
    float y = blockpos.y;
    float z = blockpos.z;

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

    // float u_min = MAGMA_TEX_COORD_U_MIN;
    // float u_max = MAGMA_TEX_COORD_U_MAX;
    // float v_min = MAGMA_TEX_COORD_V_MIN;
    // float v_max = MAGMA_TEX_COORD_V_MAX;
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

    int vert_count = counter * (36 * 3);
    int tex_count = counter * (36 * 2);
    int norm_count = counter * (36 * 3);

    int num_vertices = (36 * 3);
    int num_texcoords = (36 * 2);
    int num_normals = (36 * 3);
        
    // memcpy(mesh->vertices + vert_count, vertices, num_vertices * sizeof(float));
    
    // memcpy(mesh->texcoords + tex_count, texcoords, num_texcoords * sizeof(float));

    // memcpy(mesh->normals + norm_count, normals, num_normals * sizeof(float));
    // memcpy(mesh->vertices + vert_count, vertices, sizeof(vertices));
    
    // memcpy(mesh->texcoords + tex_count, texcoords, sizeof(texcoords));

    // memcpy(mesh->normals + norm_count, normals, sizeof(normals));

    // for (int i = 0; i < num_vertices; i++) {
    //     mesh->vertices[vert_count + i] = vertices[i];
    // }
        
    // for (int i = 0; i < num_texcoords; i++) {
    //     mesh->texcoords[tex_count + i] = texcoords[i];
    // }
        
    // for (int i = 0; i < num_normals; i++) {
    //     mesh->normals[norm_count + i] = normals[i];
    // }

    memcpy(mesh->vertices + vert_count, vertices, 36*3*sizeof(float));
    
    memcpy(mesh->texcoords + tex_count, texcoords, 36*2*sizeof(float));

    memcpy(mesh->normals + norm_count, normals, 36*3*sizeof(float));

    mesh->vertexCount += 36;
    mesh->triangleCount += 12;

    return block;
}
