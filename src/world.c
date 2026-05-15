#include "include.h"

int8_t DecideBlockType(Vector3 block_pos) {
    if(block_pos.y > 0.0f) return BLOCK_AIR;

    if(block_pos.y == 0.0f) return BLOCK_GRASS;

    if(block_pos.y > -10.0f && block_pos.y < 0.0f) return BLOCK_DIRT;

    if(block_pos.y <= -10.0f) return BLOCK_STONE;

    //return BLOCK_STONE;

    return BLOCK_UNKNOWN;
}

bool IsBlockVisible(Vector3 block_pos, int blockX, int blockY, int blockZ) {

    //some basic culling
    if(blockX == 0 || blockY == 0 || blockZ == 0) {
        return true;
    }else if (blockX == CHUNK_SIZE-1 + 0.5f || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1+0.5f) {
        return true;
    }
    //SOMETHING HERE WRONG

    // check to see if neighbors are air
    // i could keep a record... of the entire world.. an array of bools...
    // that way i could determine block visibility even if the blocks neighbor is outside the chunk...
    // but that would be impossible to procedurally generate. could not be infinite
    // so i'll check if neighbors are air, and if it's an edge block.
    // i'll face cull the edge blocks later
    // but i need access the the rest of the blocks in the chunk to check...
    // so i'll run the neighbor blocks through DecideBlockType
    if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_AIR) return true;
    if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_AIR) return true;

    // if (DecideBlockType((Vector3) { block_pos.x + 1, block_pos.y, block_pos.z }) == BLOCK_UNKNOWN) return true;
    // if (DecideBlockType((Vector3) { block_pos.x - 1, block_pos.y, block_pos.z }) == BLOCK_UNKNOWN) return true;
    // if (DecideBlockType((Vector3) { block_pos.x, block_pos.y + 1, block_pos.z }) == BLOCK_UNKNOWN) return true;
    // if (DecideBlockType((Vector3) { block_pos.x, block_pos.y - 1, block_pos.z }) == BLOCK_UNKNOWN) return true;
    // if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z + 1 }) == BLOCK_UNKNOWN) return true;
    // if (DecideBlockType((Vector3) { block_pos.x, block_pos.y, block_pos.z - 1}) == BLOCK_UNKNOWN) return true;

    return true;
    //return false;
}

MegaChunk* GenMegaChunk(Vector3 megachunk_world_pos, HashTable* hash_table) {
    MegaChunk* mega_chunk = (MegaChunk*)calloc(1, sizeof(MegaChunk));
    int megachunk_counter = 0;
    Vector3 chunk_relative_positions[] = {
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

                chunk_world_X = megachunk_world_pos.x + (chunk_relative_positions[x].x * 16);
                chunk_world_Y = megachunk_world_pos.y + (chunk_relative_positions[x].y * 16);
                chunk_world_Z = megachunk_world_pos.z + (chunk_relative_positions[x].z * 16);

                chunkmesh = FetchChunkEntry((Vector3){ chunk_world_X, chunk_world_Y, chunk_world_Z }, hash_table);
                mega_chunk->chunkmeshes[megachunk_counter] = chunkmesh;
                
            }
        }
        megachunk_counter++;
    }

    return mega_chunk;
}

void InitializeWorld() {

}
