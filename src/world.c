#include "include.h"

int DecideBlockType(Vector3 block_pos) {
    if(block_pos.y > 0.0f) return BLOCK_AIR;

    if(block_pos.y == 0.0f) return BLOCK_GRASS;

    if(block_pos.y > -10.0f && block_pos.y < 0.0f) return BLOCK_DIRT;

    if(block_pos.y <= -10.0f) return BLOCK_STONE;

    return BLOCK_UNKNOWN;
}

bool IsBlockVisible(Vector3 block_pos, int blockX, int blockY, int blockZ) {

    //some basic culling
    if(blockX == 0 || blockY == 0 || blockZ == 0) {
        return true;
    }else if (blockX == CHUNK_SIZE-1 || blockY == CHUNK_SIZE-1 || blockZ == CHUNK_SIZE-1) {
        return true;
    }
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

    return false;
}

ChunkMesh* GenWorld(HashTable* hash_table) {
    
}
