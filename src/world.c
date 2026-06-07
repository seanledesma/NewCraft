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

// traverse in a 2D spiral along XZ axis to generate Vector3 array of positions.
// may be called multiple times to reach desired Y height.
// returns an int signifying number of elements actually recorded in the passed-in coords array
int SpiralTraversal2D(Vector3* coords, int coords_index, Vector3 pos, int depth) {
    // this does relative spiraling, need to add to acutal position at the end
    int posX = 0;
    int posZ = 0;
    int dx = 0;
    int dz = -1;
    int temp = 0;
    int maxI = depth * depth;

    for(int i = 0; i < maxI; i++) {

        if ((-depth/2 <= posX) && (posX <= depth/2) && (-depth/2 <= posZ) && (posZ <= depth/2)) {
            // for(int j = -1; j < 1; j++) {
            //     coords[coord_counter++] = (Vector3) {
            //         pos.x + posX,
            //         pos.y + j,
            //         pos.z + posZ
            //     };
            // }

            // for(int j = -depth/2; j < -1; j++) {
            //     coords[coord_counter++] = (Vector3) {
            //         pos.x + posX,
            //         pos.y + j,
            //         pos.z + posZ
            //     };
            // }

            // for(int j = 1; j < depth/2; j++) {
            //     coords[coord_counter++] = (Vector3) {
            //         pos.x + posX,
            //         pos.y + j,
            //         pos.z + posZ
            //     };
            // }

            coords[coords_index++] = (Vector3) {
                pos.x + posX,
                pos.y,
                pos.z + posZ
            };

            // coords[coord_counter++] = (Vector3) {
            //     pos.x + posX,
            //     pos.y-1,
            //     pos.z + posZ
            // };

            // coords[coord_counter++] = (Vector3) {
            //     pos.x + posX,
            //     pos.y+1,
            //     pos.z + posZ
            // };
        }

        if ( (posX == posZ) || ((posX < 0) && (posX == -posZ)) || ((posX > 0) && (posX == 1-posZ)) ) {
            temp = dx;
            dx = -dz;
            dz = temp;
        }
        posX += dx;
        posZ += dz;
    }
    return coords_index;
}
// return an integer representing number of nearby blocks
int GetNearbyBlocks(BoundingBox* boxes, Vector3 player_pos, HashTable* hash_table) {
    /*
    this is dumb.. do i even want to use a bunch of bounding boxes around the player? 
    it might be a better idea to .. i mean, the goal is to have ray collisions hit the right
    block the player is looking at. 
    i'd be making a lot of unneccesary bounding boxes in the air, only need non air blocks 
    to detect collision.
    i don't think i'll get around having to make a bunch of bounding boxes around the player, however
    why don't we instead skip them if they are air, since we have the chunk data..
    i mean, we should be able to say, hey, grab everything from neg. N from player and go until pos N
    */

    Vector3 base_block_world = (Vector3) {
        floor(player_pos.x),
        floor(player_pos.y-2.0),
        floor(player_pos.z)
    };

    //Vector3 base_block_index = ConvertWorldBlockPosToChunkIndex(base_block_world, hash_table);

    int depth = 8;
    // make sure to make coords array as big as it may ever possibly get
    Vector3* coords = (Vector3*)MemAlloc((depth*depth*5) * sizeof(Vector3));
    int coords_counter = 0;
    coords_counter = SpiralTraversal2D(coords, coords_counter, base_block_world, depth);
    //then get coords for y-1
    // coords_counter = SpiralTraversal2D(coords, coords_counter, (Vector3) {
    //     base_block_world.x,
    //     base_block_world.y - 1,
    //     base_block_world.z
    // }, depth);
    // //then get coords for y+1
    // coords_counter = SpiralTraversal2D(coords, coords_counter, (Vector3) {
    //     base_block_world.x,
    //     base_block_world.y + 1,
    //     base_block_world.z
    // }, depth);
    // //then get coords for y+2
    // coords_counter = SpiralTraversal2D(coords, coords_counter, (Vector3) {
    //     base_block_world.x,
    //     base_block_world.y + 2,
    //     base_block_world.z
    // }, depth);
    // //then get coords for y+3
    // coords_counter = SpiralTraversal2D(coords, coords_counter, (Vector3) {
    //     base_block_world.x,
    //     base_block_world.y + 3,
    //     base_block_world.z
    // }, depth);


    for(int i = 0; i < coords_counter; i++) {

        // Vector3 index = ConvertWorldBlockPosToChunkIndex(coords[i], hash_table);
        /*
            I WAS THINKING... i might do this instead of is block air then skip, so it's just
            skipping all the blocks the player can't see..
            BUT that may require a big refactor in IsBlockVisible...
        */
        // if(!IsBlockVisible())

        if(IsBlockAir(coords[i], hash_table) == true) {
            continue;
        }

        boxes[i].min = coords[i];
        boxes[i].max = (Vector3) { 
            coords[i].x + 1.0f,
            coords[i].y + 1.0f,
            coords[i].z + 1.0f
        };
    }
    // TraceLog(LOG_WARNING, TextFormat("block type under player: %d", DecideBlockType(coords[0])));
    // TraceLog(LOG_WARNING, TextFormat("base block x:%.2f, y:%.2f, z:%.2f", 
    //             base_block_world.x, base_block_world.y, base_block_world.z));
    //TraceLog(LOG_WARNING, TextFormat("is block air under player: %d", IsBlockAir(coords[0], hash_table)));





    // if(IsBlockAir(coords[0], hash_table) == true) {
    //     return;
    // }

    // // if(DecideBlockType(coords[i]) == BLOCK_AIR) {
    // //     continue;
    // // }

    // boxes[0].min = coords[0];
    // boxes[0].max = (Vector3) { 
    //     coords[0].x + 1.0f,
    //     coords[0].y + 1.0f,
    //     coords[0].z + 1.0f
    // };



    free(coords);
    return coords_counter;
}

Chunk* GetCurrentChunk(Vector3 player_pos, HashTable* hash_table) {
    ChunkMesh* curr_chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((player_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor(((player_pos.y - PLAYER_HEIGHT) + 8) / 16) * 16;
    int chunkZ = (int)floor((player_pos.z + 8) / 16) * 16;
    curr_chunkmesh = FetchChunkEntry((Vector3) {chunkX,chunkY,chunkZ}, hash_table);

    if(chunkX % CHUNK_SIZE != 0) {
        TraceLog(LOG_ERROR, "Incorrect chunk position, check world.c");
    }

    return curr_chunkmesh->chunk;
}

Vector3 DeriveChunkPosition(Vector3 starting_pos, HashTable* hash_table) {
    //ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((starting_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor((starting_pos.y + 8) / 16) * 16;
    int chunkZ = (int)floor((starting_pos.z + 8) / 16) * 16;
    TraceLog(LOG_DEBUG, TextFormat("1starting y: %.2f", starting_pos.y));
    TraceLog(LOG_DEBUG, TextFormat("2chunk pos y: %d", chunkY));

    if(chunkX % CHUNK_SIZE != 0) {
        TraceLog(LOG_ERROR, "Incorrect chunk position, check world.c");
    }

    return (Vector3) { (float)chunkX, (float)chunkY, (float)chunkZ };
}


ChunkMesh* DeriveChunkMesh(Vector3 starting_pos, HashTable* hash_table) {
    ChunkMesh* chunkmesh = {0};

    int chunkX = (int)floor((starting_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor((starting_pos.y + 8) / 16) * 16;
    int chunkZ = (int)floor((starting_pos.z + 8) / 16) * 16;
    if(DoesChunkEntryExist((Vector3) { chunkX, chunkY, chunkZ }, hash_table)){
        chunkmesh = FetchChunkEntry((Vector3) { chunkX, chunkY, chunkZ }, hash_table);
    } else {
        TraceLog(LOG_ERROR, "DERIVE CHUNK DID NOT FIND CHUNK");
    }

    if(chunkX % CHUNK_SIZE != 0) {
        TraceLog(LOG_ERROR, "Incorrect chunk position, check world.c");
    }
    return chunkmesh;
}

// ChunkMesh* DeriveChunkMesh(Vector3 starting_pos, HashTable* hash_table) {
//     ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

//     int chunkX = (int)floor((starting_pos.x + 8) / 16) * 16;
//     int chunkY = (int)floor((starting_pos.y + 8) / 16) * 16;
//     int chunkZ = (int)floor((starting_pos.z + 8) / 16) * 16;
//     if(DoesChunkEntryExist((Vector3) { chunkX, chunkY, chunkZ }, hash_table)){
//         chunkmesh = FetchChunkEntry((Vector3) { chunkX, chunkY, chunkZ }, hash_table);
//     } else {
//         TraceLog(LOG_ERROR, "DERIVE CHUNK DID NOT FIND CHUNK");
//     }

//     if(chunkX % CHUNK_SIZE != 0) {
//         TraceLog(LOG_ERROR, "Incorrect chunk position, check world.c");
//     }

//     return chunkmesh;
// }


bool IsBlockAir(Vector3 block_world_pos, HashTable* hash_table) {
    // TraceLog(LOG_WARNING, TextFormat("0-isblockair blockX: %.2f", block_world_pos.x));
    //allll we're gonna do is see if this block, at the given coords, is air, or no.
    Vector3 index = ConvertWorldBlockPosToChunkIndex(block_world_pos, hash_table);
    ChunkMesh* chunkmesh = DeriveChunkMesh(block_world_pos, hash_table);
    // TraceLog(LOG_WARNING, TextFormat("index x: %d", (int)index.x));
    // TraceLog(LOG_WARNING, TextFormat("index y: %d", (int)index.y));
    // TraceLog(LOG_WARNING, TextFormat("index z: %d", (int)index.z));
    // TraceLog(LOG_WARNING, TextFormat("of chunk x:%.2f, y:%.2f, z:%.2f", chunk->world_pos.x, 
    //             chunk->world_pos.y, chunk->world_pos.z));
    if(chunkmesh->chunk->blocks[(int)index.x][(int)index.y][(int)index.z].block_type == BLOCK_AIR) {
        //TraceLog(LOG_WARNING, "hit true");
        //TraceLog(LOG_WARNING, TextFormat("index y when hitting block air: %d", (int)index.y));
        chunkmesh = NULL;
        return true;
    }else{
        //TraceLog(LOG_WARNING, "hit false");
        chunkmesh = NULL;
        return false;
    }
}


Vector3 ConvertWorldBlockPosToChunkIndex(Vector3 block_world_pos, HashTable* hash_table) {
    Vector3 chunk_index = {0};

    // first need to get the chunk the block pertains to
    // Chunk* curr_chunk = (Chunk*)MemAlloc(sizeof(Chunk));
    // curr_chunk = DeriveChunk(block_world_pos, hash_table);

    // I actually don't need that chunk, we are just converting to an index
    Vector3 world_pos = DeriveChunkPosition(block_world_pos, hash_table);

    // then figure out the block index

    float blockX = block_world_pos.x;
    float blockY = block_world_pos.y;
    float blockZ = block_world_pos.z;

    // if(world_pos.x < 0 || blockX < 0) {
    //     if(world_pos.x < 0 && blockX < 0) {
    //         if(world_pos.x < blockX) {
    //             chunk_index.x = floor((world_pos.x - blockX) + 8);
    //         } else {
    //             chunk_index.x = floor((world_pos.x + blockX) + 8);
    //         }
    //     }
    //     if(world_pos.x < 0 && blockX >= 0) {
    //         // literally impossible?
    //     }
    //     if(blockX < 0 && world_pos.x >= 0) {
    //         chunk_index.x = floor((world_pos.x + blockX) + 8);
    //     }
    // }

    if(world_pos.x < 0 || blockX < 0) {
        if(world_pos.x < 0 && blockX < 0) {
            if(world_pos.x < blockX) {
                chunk_index.x = floor((blockX - world_pos.x) + 8);  //GOOD
            } else if(world_pos.x >= blockX) {
                chunk_index.x = floor((blockX - world_pos.x) + 8);
            }
        } else {
            chunk_index.x = floor((blockX + world_pos.x) + 8);
        }
    }else {
        chunk_index.x = floor((blockX - world_pos.x) + 8);
    }

    if(world_pos.y < 0 || blockY < 0) {
        if(world_pos.y < 0 && blockY < 0) {
            if(world_pos.y < blockY) {
                chunk_index.y = floor((blockY - world_pos.y) + 8);  //GOOD
            } else if(world_pos.y >= blockY) {
                chunk_index.y = floor((blockY - world_pos.y) + 8);
            }
        } else {
            chunk_index.y = floor((blockY + world_pos.y) + 8);
        }
    }else {
        chunk_index.y = floor((blockY - world_pos.y) + 8);
    }
    TraceLog(LOG_DEBUG, TextFormat("world y: %.2f", world_pos.y));
    TraceLog(LOG_DEBUG, TextFormat("blockY: %d", blockY));
    TraceLog(LOG_DEBUG, TextFormat("chunk index y: %.2f", chunk_index.y));
    TraceLog(LOG_DEBUG, TextFormat("chunk index x: %.2f", chunk_index.x));
    
    if(world_pos.z < 0 || blockZ < 0) {
        if(world_pos.z < 0 && blockZ < 0) {
            if(world_pos.z < blockZ) {
                chunk_index.z = floor((blockZ - world_pos.z) + 8);  //GOOD
            } else if(world_pos.z >= blockZ) {
                chunk_index.z = floor((blockZ - world_pos.z) + 8);
            }
        } else {
            chunk_index.z = floor((blockZ + world_pos.z) + 8);
        }
    }else {
        chunk_index.z = floor((blockZ - world_pos.z) + 8);
    }

    //throw an error if any index is below zero or above chunksize - 1
    if(chunk_index.x < 0 || chunk_index.y < 0 || chunk_index.z < 0 ||
    chunk_index.x > CHUNK_SIZE-1 || chunk_index.y > CHUNK_SIZE-1 || chunk_index.z > CHUNK_SIZE-1) {
        
        TraceLog(LOG_ERROR, "Index out of bounds, check ConvertWorldBlockPosToChunkIndex");
    }


    return chunk_index;
}


Vector3 ConvertChunkIndexToWorldBlockPos(Vector3 chunk_index, Vector3 chunk_world_pos, HashTable* hash_table) {
    Vector3 block_world_pos = {0};

    block_world_pos.x = chunk_world_pos.x + (chunk_index.x - HALF_CHUNK);
    block_world_pos.y = chunk_world_pos.y + (chunk_index.y - HALF_CHUNK);
    block_world_pos.z = chunk_world_pos.z + (chunk_index.z - HALF_CHUNK);

    return block_world_pos;
}

//i'm coming back to this later.. some bad assumptions when making this
bool IsBlockVisibleImproved(Vector3 block_world_pos, HashTable* hash_table) {
    // first check if that block is in a chunk that even exists
    Vector3 chunk_world_pos = DeriveChunkPosition(block_world_pos, hash_table);
    if(!DoesChunkEntryExist(chunk_world_pos, hash_table)) return false;

    //now we know it exists, just grab the index of the block and check its neighbors
    Vector3 base_index = ConvertWorldBlockPosToChunkIndex(block_world_pos, hash_table);
    //not going to use decide block type here, because player may have added blocks 
    //but first just see if we are on the border of unexplored chunks
    //DID THIS WRONG! AM CHECKING BLOCK POS, NOT CHUNK WORLD POS :((k ---- FIXED
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x+1, block_world_pos.y, block_world_pos.z }, hash_table), hash_table)) return true;
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x-1, block_world_pos.y, block_world_pos.z }, hash_table), hash_table)) return true;
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x, block_world_pos.y+1, block_world_pos.z }, hash_table), hash_table)) return true;
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x, block_world_pos.y-1, block_world_pos.z }, hash_table), hash_table)) return true;
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z+1 }, hash_table), hash_table)) return true;
    if(!DoesChunkEntryExist(DeriveChunkPosition((Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z-1 }, hash_table), hash_table)) return true;

    //so by now we know the block exists, and it's not on the end of the world.
    //next we gotta decide if this is when we are first setting up a chunk and the player hasn't had the chance
    //to add blocks, or if this is after. because if it's the first time, then we just poll DecideBlockType. But that
    //method doesn't work anymore once player has been around and may have manipulated terrain.
    //wait... if we know the chunk exists, even if the player has had time to mess with it, we can just check the surrounding
    //neighbors type. We already know we aren't on the edge of the world, but we may still be on a border.
    //it shouldn't matter if we are on a border or not, just check each neighbor using world coords
    //get the chunk for this block, then the index
    Chunk* temp_chunk = (Chunk*)MemAlloc(sizeof(Chunk));
    Vector3 temp_pos = (Vector3) { block_world_pos.x + 1, block_world_pos.y, block_world_pos.z };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // Vector3 temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    // //gonna do this a few more time
    // temp_pos = (Vector3) { block_world_pos.x - 1, block_world_pos.y, block_world_pos.z };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    // temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y+1, block_world_pos.z };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    // temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y-1, block_world_pos.z };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    // temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z+1 };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    // temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z-1 };
    // temp_chunk = DeriveChunk(temp_pos, hash_table);
    // temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    // if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    //if all else fails, it's not visible
    return false;

}
