#include "include.h"

void GenMeshChunk(Mesh* mesh, Chunk* chunk, HashTable* hash_table) {
    int block_counter = 0;
    // if you see all magma, something went wrong
    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;

    for (int blockX = 0; blockX < CHUNK_SIZE; blockX++) {
        for (int blockY = 0; blockY < CHUNK_SIZE; blockY++) {
            for (int blockZ = 0; blockZ < CHUNK_SIZE; blockZ++) {
                float x = chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f;
                float y = chunk->world_pos.y - HALF_CHUNK + blockY + 0.5f;
                float z = chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;
                Vector3 block_world_pos = { x, y, z };

                if(IsBlockVisible(chunk->world_pos, block_world_pos, blockX, blockY, blockZ, hash_table) == false) {
                    chunk->blocks[blockX][blockY][blockZ].block_type = BLOCK_AIR;
                }

                // if(IsBlockVisibleImproved(block_world_pos, hash_table) == false) {
                //     chunk->blocks[blockX][blockY][blockZ].block_type = BLOCK_AIR;
                // }
                
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


/*
DUH.. seperate the meshing logic from the chunk logic. generate all chunks, mark their block types, then generate meshes.
this shouldn't require a huge refactor, tip: use a bool to mark chunks "dirty" if they need remeshing
this will prove incredibly useful when implementing block deletion / addition.
*/
bool IsBlockVisible(Vector3 chunk_pos, Vector3 block_pos, int blockX, int blockY, int blockZ, HashTable* hash_table) {
    //return true;
    //some basic culling around chunk border
    // if(blockX == 0 || blockY == 0 || blockZ == 0) {
    //     return true;
    // }else if (blockX == CHUNK_SIZE-1 + 0.5f || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1+0.5f) {
    //     return true;
    // }

    if (blockX == 0 && blockZ == 0) {
        return true;
    }
    if (blockX == 0 && blockZ == CHUNK_SIZE-1) {
        return true;
    }
    if (blockX == CHUNK_SIZE-1 && blockZ == 0) {
        return true;
    }
    if (blockX == CHUNK_SIZE-1 && blockZ == CHUNK_SIZE-1) {
        return true;
    }

    /*
    the whole idea here is to see first if the adjacent chunk exists, and if it does, check 
    the specific block that is next to the block we are dealing with. 
    */
    if(blockX == 0) {
        // check x - 1
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x - CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table) == true) {
            //TraceLog(LOG_WARNING, "did we at least get here");
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x - CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table);
            
            // decide block type expects a world coord
            float posX = temp_chunk_mesh->chunk->world_pos.x + HALF_CHUNK - 0.5f; 
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                //we just checked if the block west of current block was air, and it wasn't. that doesn't mean
                //that this block isn't visible, we still need to check the other faces.
                if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                //if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;

                return false;
            }
        } else {
            return true;
        }
    }

    if (blockX == CHUNK_SIZE-1) {
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x + CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table) == true) {
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x + CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table);
            
            // decide block type expects a world coord
            float posX = temp_chunk_mesh->chunk->world_pos.x - HALF_CHUNK + 0.5f; 
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                //if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;
                return false;
            }
        } else {
            return true;
        }
    }

    if(blockZ == 0) {
        // check z - 1
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table) == true) {
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table);


            float posX = temp_chunk_mesh->chunk->world_pos.x + HALF_CHUNK + blockX + 0.5f;
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK - 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
                //if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;
                return false;
            }
        } else {
            return true;
        }
    }

    if (blockZ == CHUNK_SIZE-1) {
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table) == true) {
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table);


            float posX = temp_chunk_mesh->chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f; 
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + 0.05f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
                //if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
                if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;
                return false;
            }
        } else {
            return true;
        }
    }

    // if(blockY == 0) {
    //     // check y - 1
    //     if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y - CHUNK_SIZE, chunk_pos.z }, hash_table) == true) {
    //         ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //         temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y - CHUNK_SIZE, chunk_pos.z }, hash_table);
    //         if(temp_chunk_mesh->chunk->blocks[blockX][CHUNK_SIZE-1][blockZ].block_type == BLOCK_AIR) {
    //             return true;
    //         } else {
    //             return false;
    //         }
    //     } else {
    //         return true;
    //     }
    // }

    // if(blockZ == 0) {
    //     // check z - 1
    //     if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table) == true) {
    //         ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //         temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table);
    //         if(temp_chunk_mesh->chunk->blocks[blockX][blockY][CHUNK_SIZE-1].block_type == BLOCK_AIR) {
    //             return true;
    //         } else {
    //             return false;
    //         }
    //     } else {
    //         return true;
    //     }
    // }
    //     // need to check neighboring chunk here
    //     //ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    // //if (blockX == CHUNK_SIZE-1 || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1) {
    
    // if (blockX == CHUNK_SIZE-1) {
    //     if (DoesChunkEntryExist((Vector3) { chunk_pos.x - CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table) == true) {
    //         ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //         temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x - CHUNK_SIZE, chunk_pos.y, chunk_pos.z }, hash_table);
    //         if(temp_chunk_mesh->chunk->blocks[0][blockY][blockZ].block_type == BLOCK_AIR) {
    //             return true;
    //         } else {
    //             return false;
    //         }
    //     } else {
    //         return true;
    //     }
    // }

    // if (blockY == CHUNK_SIZE-1) {
    //     if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y + CHUNK_SIZE, chunk_pos.z }, hash_table) == true) {
    //         ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //         temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y + CHUNK_SIZE, chunk_pos.z }, hash_table);
    //         if(temp_chunk_mesh->chunk->blocks[blockX][0][blockZ].block_type == BLOCK_AIR) {
    //             return true;
    //         } else {
    //             return false;
    //         }
    //     } else {
    //         return true;
    //     }
    // }

    // if (blockZ == CHUNK_SIZE-1) {
    //     if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table) == true) {
    //         ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //         temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table);
    //         if(temp_chunk_mesh->chunk->blocks[blockX][blockY][0].block_type == BLOCK_AIR) {
    //             return true;
    //         } else {
    //             return false;
    //         }
    //     } else {
    //         return true;
    //     }
    // }

    

    // check to see if neighbors are air
    if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;

    /*
        one problem remains, what if the block I'm checking is in another chunk? I need to be able to tell
        if there is a chunk next door and if so is the neighbor block air or not.
    */

    return false;
}

