#include "include.h"

bool IsBlockVisibleRework(Vector3 block_world_position, HashTable* hash_table) {
    //first make sure the chunk exists
    Vector3 chunk_pos = DeriveChunkPosition(block_world_position);
    if (!DoesChunkEntryExist(chunk_pos, hash_table)) {
        // TraceLog(LOG_WARNING, TextFormat("chunk x: %d", (int)chunk_pos.x));
        // TraceLog(LOG_WARNING, TextFormat("chunk y: %d", (int)chunk_pos.y));
        // TraceLog(LOG_WARNING, TextFormat("chunk z: %d", (int)chunk_pos.z));
        return true;
    }

    //we have the block in question. all we need to do is get the chunk, then then block index, then check
    ChunkMesh* chunkmesh = DeriveChunkMesh(block_world_position, hash_table);

    Vector3 block_index = ConvertWorldBlockPosToChunkIndex(block_world_position);

    TraceLog(LOG_DEBUG, TextFormat("index x: %d", (int)block_index.x));
    if (chunkmesh->chunk->blocks[(int)block_index.x][(int)block_index.y][(int)block_index.z].block_type == BLOCK_AIR) {
        TraceLog(LOG_DEBUG, "got here");
        chunkmesh = NULL;
        return true;
    } else {
        chunkmesh = NULL;
        return false;
    }
}

void GenMeshChunkRework(Mesh* mesh, Chunk* chunk, HashTable* hash_table) {
    int face_counter = 0;
    // if you see all magma, something went wrong
    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;
    int shade = 215;

    for (int blockX = 0; blockX < CHUNK_SIZE; blockX++) {
        for (int blockY = 0; blockY < CHUNK_SIZE; blockY++) {
            for (int blockZ = 0; blockZ < CHUNK_SIZE; blockZ++) {
                float x = chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f;
                float y = chunk->world_pos.y - HALF_CHUNK + blockY + 0.5f;
                float z = chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;
                Vector3 block_world_pos = { x, y, z };
                float size = 0.5f;

                int block_type = chunk->blocks[blockX][blockY][blockZ].block_type;

                if(block_type == BLOCK_AIR) {
                    mesh->vertexCount += 36;
                    mesh->triangleCount += 12;
                    continue;
                }

                //TraceLog(LOG_WARNING, TextFormat("block type: %d", chunk->blocks[blockX][blockY][blockZ].block_type));
                // we will know if block is visible or not by checking all 6 sides
                //front side (Z+)
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z + 1 }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = DIRTGRASS_TEX_COORD_U_MIN;
                            u_max = DIRTGRASS_TEX_COORD_U_MAX;
                            v_min = DIRTGRASS_TEX_COORD_V_MIN;
                            v_max = DIRTGRASS_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }
                    //this seems like a lot, but we really need to add each face individually

                    //first front triangle
                    //(counter clock wise starting with bottom left)
                    float vertices[] = {
                        x-size, y-size, z+size,
                        x+size, y+size, z+size,
                        x-size, y+size, z+size,
                        //second front triangle
                        x-size, y-size, z+size,
                        x+size, y-size, z+size,
                        x+size, y+size, z+size,
                    };

                    float texcoords[] = {
                        //front
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
                    };
                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    // if the block above is NOT air, we need to alter colors for ambient occlusion
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y + 1, block_world_pos.z + 1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    // y - 1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y - 1, block_world_pos.z + 1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //x+1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y, block_world_pos.z + 1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;  
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //x-1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y, block_world_pos.z + 1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }


                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;
                }

                //back side (Z-)
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z - 1 }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = DIRTGRASS_TEX_COORD_U_MIN;
                            u_max = DIRTGRASS_TEX_COORD_U_MAX;
                            v_min = DIRTGRASS_TEX_COORD_V_MIN;
                            v_max = DIRTGRASS_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }

                    float vertices[] = {
                        //first back triangle
                        x+size, y-size, z-size,
                        x-size, y+size, z-size,
                        x+size, y+size, z-size,

                        //second back triangle
                        x+size, y-size, z-size,
                        x-size, y-size, z-size, 
                        x-size, y+size, z-size,
                    };

                    float texcoords[] = {
                        //back
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
                        //back
                        zero, zero, -one,
                        zero, zero, -one,
                        zero, zero, -one,

                        zero, zero, -one,
                        zero, zero, -one,
                        zero, zero, -one,
                    };

                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y + 1, block_world_pos.z - 1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    // y - 1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y - 1, block_world_pos.z - 1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //x+1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y, block_world_pos.z - 1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;  
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //x-1
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y, block_world_pos.z - 1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }

                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;
                    face_counter++;
                }

                //top Y+
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y + 1, block_world_pos.z }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = GRASS_LIGHT_TEX_COORD_U_MIN;
                            u_max = GRASS_LIGHT_TEX_COORD_U_MAX;
                            v_min = GRASS_LIGHT_TEX_COORD_V_MIN;
                            v_max = GRASS_LIGHT_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }

                    float vertices[] = {
                        //top triangles
                        x-size, y+size, z+size,
                        x+size, y+size, z-size,
                        x-size, y+size, z-size,

                        x-size, y+size, z+size,
                        x+size, y+size, z+size,
                        x+size, y+size, z-size,
                    };

                    float texcoords[] = {
                        //top
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
                        //top
                        zero, one, zero,
                        zero, one, zero,
                        zero, one, zero,

                        zero, one, zero,
                        zero, one, zero,
                        zero, one, zero,
                    };

                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y + 1, block_world_pos.z }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    // 
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y + 1, block_world_pos.z }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y+1, block_world_pos.z + 1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y+1, block_world_pos.z - 1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;   
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }

                    // going to need to check all four corners as well
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y + 1, block_world_pos.z+1 }, hash_table)) {
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y + 1, block_world_pos.z+1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y+1, block_world_pos.z-1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y+1, block_world_pos.z-1 }, hash_table)) {
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                    }

                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;
                    face_counter++;
                }

                //bottom Y-
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y - 1, block_world_pos.z }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }

                    float vertices[] = {
                        //bottom
                        x-size, y-size, z-size,
                        x+size, y-size, z+size,
                        x-size, y-size, z+size,

                        x-size, y-size, z-size,
                        x+size, y-size, z-size, 
                        x+size, y-size, z+size,
                    };

                    float texcoords[] = {
                        //bottom
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
                        //bottom
                        zero, -one, zero,
                        zero, -one, zero,
                        zero, -one, zero,

                        zero, -one, zero,
                        zero, -one, zero,
                        zero, -one, zero,
                    };

                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y-1, block_world_pos.z }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y-1, block_world_pos.z }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y-1, block_world_pos.z+1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;  
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x, block_world_pos.y-1, block_world_pos.z-1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }

                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;
                    face_counter++;
                }

                // //left (X-)
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x - 1, block_world_pos.y, block_world_pos.z }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = DIRTGRASS_TEX_COORD_U_MIN;
                            u_max = DIRTGRASS_TEX_COORD_U_MAX;
                            v_min = DIRTGRASS_TEX_COORD_V_MIN;
                            v_max = DIRTGRASS_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }

                    float vertices[] = {
                        //left
                        x-size, y-size, z-size,
                        x-size, y+size, z+size,
                        x-size, y+size, z-size,

                        x-size, y-size, z-size,
                        x-size, y-size, z+size,
                        x-size, y+size, z+size,
                    };

                    float texcoords[] = {
                        //left
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
                        //left
                        -one, zero, zero,
                        -one, zero, zero,
                        -one, zero, zero,

                        -one, zero, zero,
                        -one, zero, zero,
                        -one, zero, zero,
                    };
                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y+1, block_world_pos.z }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y-1, block_world_pos.z }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y, block_world_pos.z+1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;  
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x-1, block_world_pos.y, block_world_pos.z-1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }

                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;
                    face_counter++;
                }


                // //right x+
                if (IsBlockVisibleRework((Vector3) { block_world_pos.x + 1, block_world_pos.y, block_world_pos.z }, hash_table)) {
                    switch (block_type) {
                        case BLOCK_GRASS:
                            u_min = DIRTGRASS_TEX_COORD_U_MIN;
                            u_max = DIRTGRASS_TEX_COORD_U_MAX;
                            v_min = DIRTGRASS_TEX_COORD_V_MIN;
                            v_max = DIRTGRASS_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_DIRT:
                            u_min = DIRT_DARK_TEX_COORD_U_MIN;
                            u_max = DIRT_DARK_TEX_COORD_U_MAX;
                            v_min = DIRT_DARK_TEX_COORD_V_MIN;
                            v_max = DIRT_DARK_TEX_COORD_V_MAX;
                            break;
                        case BLOCK_STONE:
                            u_min = STONE_TEX_COORD_U_MIN;
                            u_max = STONE_TEX_COORD_U_MAX;
                            v_min = STONE_TEX_COORD_V_MIN;
                            v_max = STONE_TEX_COORD_V_MAX;
                            break;
                        default:
                            // if you see all magma, something went wrong
                            u_min = MAGMA_TEX_COORD_U_MIN;
                            u_max = MAGMA_TEX_COORD_U_MAX;
                            v_min = MAGMA_TEX_COORD_V_MIN;
                            v_max = MAGMA_TEX_COORD_V_MAX;
                    }

                    float vertices[] = {
                        //right
                        x+size, y-size, z+size,
                        x+size, y+size, z-size,
                        x+size, y+size, z+size,

                        x+size, y-size, z+size,
                        x+size, y-size, z-size,
                        x+size, y+size, z-size,
                    };

                    float texcoords[] = {
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
                        //right
                        one, zero, zero,
                        one, zero, zero,
                        one, zero, zero,

                        one, zero, zero,
                        one, zero, zero,
                        one, zero, zero,
                    };

                    unsigned char colors[] = {
                        //2 triangles, 6 vertices, 4 rgba values per vertex
                        255, 255, 255, 255, //0 - 3     #1
                        255, 255, 255, 255, //4 - 7     #2
                        255, 255, 255, 255, //8 - 11    #3

                        255, 255, 255, 255, //12 - 15   #4
                        255, 255, 255, 255, //16 - 19   #5
                        255, 255, 255, 255, //20 - 23   #6
                    };
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y+1, block_world_pos.z }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade; 
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y-1, block_world_pos.z }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade; 
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y, block_world_pos.z-1 }, hash_table)) {
                        colors[4]=shade; colors[5]=shade; colors[6]=shade;  
                        colors[16]=shade; colors[17]=shade; colors[18]=shade;
                        colors[20]=shade; colors[21]=shade; colors[22]=shade;
                    }
                    //
                    if(!IsBlockVisibleRework((Vector3) { block_world_pos.x+1, block_world_pos.y, block_world_pos.z+1 }, hash_table)) {
                        colors[0]=shade; colors[1]=shade; colors[2]=shade;  
                        colors[8]=shade; colors[9]=shade; colors[10]=shade;
                        colors[12]=shade; colors[13]=shade; colors[14]=shade;
                    }

                    int vert_count = (face_counter * 6 * 3);
                    int tex_count = (face_counter * 6 * 2);
                    int norm_count = (face_counter * 6 * 3);
                    int color_count = face_counter * 6 * 4;

                    memcpy(mesh->vertices + vert_count, vertices, 6*3*sizeof(float));
                    
                    memcpy(mesh->texcoords + tex_count, texcoords, 6*2*sizeof(float));

                    memcpy(mesh->normals + norm_count, normals, 6*3*sizeof(float));

                    memcpy(mesh->colors + color_count, colors, 6*4*sizeof(unsigned char));

                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;

                    face_counter++;

                } else {
                    mesh->vertexCount += 6;
                    mesh->triangleCount += 2;
                    face_counter++;
                }
            }
        }
    }
}
