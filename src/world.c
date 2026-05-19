#include "include.h"
#define FNL_IMPL
#include "FastNoiseLite.h"
fnl_state noise;

Vector3 relative_positions[] = {
    // home mega chunk
    (Vector3){0.0f, 0.0f, 0.0f},
    // left and right mega chunk (facing towards -Z)
    (Vector3){-1.0f, 0.0f, 0.0f},
    (Vector3){1.0f, 0.0f, 0.0f},
    //front and back
    (Vector3){0.0f, 0.0f, -1.0f},
    (Vector3){0.0f, 0.0f, 1.0f},
    //top and bottom
    (Vector3){0.0f, 1.0f, 0.0f},
    (Vector3){0.0f, -1.0f, 0.0f},

    // up left and right mega chunk (facing towards -Z)
    (Vector3){-1.0f, 1.0f, 0.0f},
    (Vector3){1.0f, 1.0f, 0.0f},
    // down left and right mega chunk (facing towards -Z)
    (Vector3){-1.0f, -1.0f, 0.0f},
    (Vector3){1.0f, -1.0f, 0.0f},
    //up front and back
    (Vector3){0.0f, 1.0f, -1.0f},
    (Vector3){0.0f, 1.0f, 1.0f},
    //down front and back
    (Vector3){0.0f, -1.0f, -1.0f},
    (Vector3){0.0f, -1.0f, 1.0f},

    //continues diagonally..
    (Vector3){-1.0f, 0.0f, -1.0f},
    (Vector3){1.0f, 0.0f, -1.0f},

    (Vector3){-1.0f, 0.0f, 1.0f},
    (Vector3){1.0f, 0.0f, 1.0f},

    (Vector3){-1.0f, 1.0f, -1.0f},
    (Vector3){1.0f, 1.0f, -1.0f},

    (Vector3){-1.0f, 1.0f, 1.0f},
    (Vector3){1.0f, 1.0f, 1.0f},

    (Vector3){-1.0f, -1.0f, -1.0f},
    (Vector3){1.0f, -1.0f, -1.0f},

    (Vector3){-1.0f, -1.0f, 1.0f},
    (Vector3){1.0f, -1.0f, 1.0f},
};

//fnl_state noise;

void InitWorld() {
    //setup noise

    noise = fnlCreateState();
    noise.seed = 1234;
    //noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    //noise.noise_type = FNL_NOISE_PERLIN;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    noise.frequency = 0.03f;
}

int8_t DecideBlockType(Vector3 block_world_pos) {
    //will return a value between -1 and 1 !!
    float height = fnlGetNoise2D(&noise, block_world_pos.x, block_world_pos.z);
    height *= 5;
    height = floor(height);

    if (block_world_pos.y > height) return BLOCK_AIR;

    if (block_world_pos.y == height) return BLOCK_GRASS;

    if (block_world_pos.y < height && block_world_pos.y >= -5.0f) return BLOCK_DIRT;

    if (block_world_pos.y < -5.0) return BLOCK_STONE;

    //something went wrong
    return BLOCK_MAGMA;
}

bool IsBlockVisible(Vector3 chunk_pos, Vector3 block_pos, int blockX, int blockY, int blockZ, HashTable* hash_table) {
    //return true;
    //some basic culling around chunk border
    // if(blockX == 0 || blockY == 0 || blockZ == 0) {
    //     return true;
    // }else if (blockX == CHUNK_SIZE-1 + 0.5f || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1+0.5f) {
    //     return true;
    // }

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
            float posX = temp_chunk_mesh->chunk->world_pos.x + HALF_CHUNK + 0.5f; 
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                return false;
            }

            // if(temp_chunk_mesh->chunk->blocks[CHUNK_SIZE-1][blockY][blockZ].block_type == BLOCK_AIR) {
            //     //TraceLog(LOG_WARNING, "returning true for IsBlockVisible, found other chunk / block is air");
            //     return true;
            // } else {
            //     return false;
            // }
        } else {
            //TraceLog(LOG_WARNING, "hi");
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
                return false;
            }
            
            
            
            
            
            // if(temp_chunk_mesh->chunk->blocks[0][blockY][blockZ].block_type == BLOCK_AIR) {
            //     return true;
            // } else {
            //     return false;
            // }
        } else {
            return true;
        }
    }

    if(blockZ == 0) {
        // check z - 1
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table) == true) {
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z - CHUNK_SIZE }, hash_table);


            float posX = temp_chunk_mesh->chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f; //have to add 0.5 so it lines up.. for reasons..
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                return false;
            }




            // if(temp_chunk_mesh->chunk->blocks[blockX][blockY][CHUNK_SIZE-1].block_type == BLOCK_AIR) {
            //     return true;
            // } else {
            //     return false;
            // }
        } else {
            return true;
        }
    }

    if (blockZ == CHUNK_SIZE-1) {
        if (DoesChunkEntryExist((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table) == true) {
            ChunkMesh* temp_chunk_mesh = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            temp_chunk_mesh = FetchChunkEntry((Vector3) { chunk_pos.x, chunk_pos.y, chunk_pos.z + CHUNK_SIZE }, hash_table);


            float posX = temp_chunk_mesh->chunk->world_pos.x - HALF_CHUNK + blockX + 0.5f; //have to add 0.5 so it lines up.. for reasons..
            float posY = temp_chunk_mesh->chunk->world_pos.y - HALF_CHUNK + blockY;
            float posZ = temp_chunk_mesh->chunk->world_pos.z - HALF_CHUNK + blockZ + 0.5f;

            if (DecideBlockType((Vector3) { posX, posY, posZ }) == BLOCK_AIR) {
                return true;
            } else {
                return false;
            }




            // if(temp_chunk_mesh->chunk->blocks[blockX][blockY][0].block_type == BLOCK_AIR) {
            //     return true;
            // } else {
            //     return false;
            // }
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

MegaChunk* GenMegaChunk(Vector3 megachunk_relative_pos, HashTable* hash_table) {
    MegaChunk* mega_chunk = (MegaChunk*)MemAlloc(sizeof(MegaChunk));
    int megachunk_counter = 0;

    Vector3 megachunk_world_pos = (Vector3) { megachunk_relative_pos.x * 48, megachunk_relative_pos.y * 48, megachunk_relative_pos.z * 48 };

    float chunk_world_X = 0.0f;
    float chunk_world_Y = 0.0f;
    float chunk_world_Z = 0.0f;
    for (int x = 0; x < MEGA_CHUNK_SIZE; x++) {
        //create all the conceptual chunks in mega chunk
        for (int y = 0; y < MEGA_CHUNK_SIZE; y++) {
            for (int z = 0; z < MEGA_CHUNK_SIZE; z++) {

                ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1, sizeof(ChunkMesh));
                // float chunk_world_X = megachunk_world_pos.x + (relative_pos.x * 16);
                // float chunk_world_Y = megachunk_world_pos.y + (relative_pos.y * 16);
                // float chunk_world_Z = megachunk_world_pos.z + (relative_pos.z * 16);

                chunk_world_X = megachunk_world_pos.x + (relative_positions[x].x * 16);
                chunk_world_Y = megachunk_world_pos.y + (relative_positions[x].y * 16);
                chunk_world_Z = megachunk_world_pos.z + (relative_positions[x].z * 16);

                chunkmesh = FetchChunkEntry((Vector3){ chunk_world_X, chunk_world_Y, chunk_world_Z }, hash_table);
                mega_chunk->chunkmeshes[megachunk_counter] = chunkmesh;
                
            }
        }
        megachunk_counter++;
    }

    return mega_chunk;
}

