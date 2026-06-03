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

// given a 3D coord (Vector3) and depth (how many rows/columns to traverse) return array of coords
void SpiralTraversal3D(Vector3* coords, Vector3 pos, int depth) {
    // this does relative spiraling, need to add to acutal position at the end
    int posX = 0;
    int posZ = 0;
    int dx = 0;
    int dz = -1;
    int temp = 0;
    int maxI = depth * depth;
    int coord_counter = 0;

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

            coords[coord_counter++] = (Vector3) {
                pos.x + posX,
                pos.y,
                pos.z + posZ
            };
        }

        if ( (posX == posZ) || ((posX < 0) && (posX == -posZ)) || ((posX > 0) && (posX == 1-posZ)) ) {
            temp = dx;
            dx = -dz;
            dz = temp;
        }
        posX += dx;
        posZ += dz;
    }
}

BoundingBox* GetNearbyBlocks(Vector3 player_pos, HashTable* hash_table) {
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

    BoundingBox* boxes = (BoundingBox*)MemAlloc(sizeof(BoundingBox) * 729);

    Vector3 base_block_world = (Vector3) {
        floor(player_pos.x),
        floor(player_pos.y - 2),
        floor(player_pos.z)
    };

    //Vector3 base_block_index = ConvertWorldBlockPosToChunkIndex(base_block_world, hash_table);

    int depth = 7;
    Vector3* coords = (Vector3*)MemAlloc((depth*depth) * sizeof(Vector3));
    SpiralTraversal3D(coords, base_block_world, depth);

    for(int i = 0; i < (depth*depth); i++) {

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

        // if(DecideBlockType(coords[i]) == BLOCK_AIR) {
        //     continue;
        // }

        boxes[i].min = coords[i];
        boxes[i].max = (Vector3) { 
            coords[i].x + 1.0f,
            coords[i].y + 1.0f,
            coords[i].z + 1.0f
        };
    }

    free(coords);

    return boxes;
}

Chunk* GetCurrentChunk(Vector3 player_pos, HashTable* hash_table) {
    ChunkMesh* curr_chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((player_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor(((player_pos.y - PLAYER_HEIGHT) + 8) / 16) * 16;
    int chunkZ = (int)floor((player_pos.z + 8) / 16) * 16;
    curr_chunkmesh = FetchChunkEntry((Vector3) {chunkX,chunkY,chunkZ}, hash_table);

    return curr_chunkmesh->chunk;
}

Vector3 DeriveChunkPosition(Vector3 starting_pos, HashTable* hash_table) {
    //ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((starting_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor((starting_pos.y + 8) / 16) * 16;
    int chunkZ = (int)floor((starting_pos.z + 8) / 16) * 16;

    return (Vector3) { (float)chunkX, (float)chunkY, (float)chunkZ };
}

Chunk* DeriveChunk(Vector3 starting_pos, HashTable* hash_table) {
    ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((starting_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor((starting_pos.y + 8) / 16) * 16;
    int chunkZ = (int)floor((starting_pos.z + 8) / 16) * 16;
    if(DoesChunkEntryExist((Vector3) { chunkX, chunkY, chunkZ }, hash_table)){
        chunkmesh = FetchChunkEntry((Vector3) { chunkX, chunkY, chunkZ }, hash_table);
    } else {
        TraceLog(LOG_ERROR, "DERIVE CHUNK DID NOT FIND CHUNK");
    }

    return chunkmesh->chunk;
}


bool IsBlockAir(Vector3 block_world_pos, HashTable* hash_table) {
    //allll we're gonna do is see if this block, at the given coords, is air, or no.
    Vector3 index = ConvertWorldBlockPosToChunkIndex(block_world_pos, hash_table);
    Chunk* chunk = (Chunk*)MemAlloc(sizeof(Chunk));
    chunk = DeriveChunk(block_world_pos, hash_table);

    if(chunk->blocks[(int)index.x][(int)index.y][(int)index.z].block_type == BLOCK_AIR) {
        TraceLog(LOG_WARNING, "hit true");
        return true;
    }else{
        TraceLog(LOG_WARNING, "hit false");
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
    chunk_index.x = (world_pos.x - floor(block_world_pos.x)) + 7;       // 7?? or 8??

    chunk_index.y = (world_pos.y - floor(block_world_pos.y)) + 7;

    chunk_index.z = (world_pos.z - floor(block_world_pos.z)) + 7;


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
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    Vector3 temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    //gonna do this a few more time
    temp_pos = (Vector3) { block_world_pos.x - 1, block_world_pos.y, block_world_pos.z };
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y+1, block_world_pos.z };
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y-1, block_world_pos.z };
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z+1 };
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    temp_pos = (Vector3) { block_world_pos.x, block_world_pos.y, block_world_pos.z-1 };
    temp_chunk = DeriveChunk(temp_pos, hash_table);
    temp_index = ConvertWorldBlockPosToChunkIndex(temp_pos, hash_table);
    if(temp_chunk->blocks[(int)temp_index.x][(int)temp_index.y][(int)temp_index.z].block_type == BLOCK_AIR) return true;

    //if all else fails, it's not visible
    return false;

}
