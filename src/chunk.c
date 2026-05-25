#include "include.h"

// here we create and return a chunk_mesh pointer which has two nested pointers,
// one to what is returned by gen_chunk and the other to the mesh we create in this 
// function using the pointer to the chunk returned by gen_chunk.
ChunkMesh* gen_chunk_mesh(Vector3 world_pos, HashTable* hash_table) {

    ChunkMesh* chunk_mesh = calloc(1, sizeof(ChunkMesh));
    
    chunk_mesh->chunk = (Chunk*)calloc(1,sizeof(Chunk));

    //TraceLog(LOG_WARNING, "gen chunk with world y: %.2f", world_pos.y);

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
    
    chunk_mesh->mesh->texcoords = (float *)MemAlloc(num_chunk_texcoords * sizeof(float));
   
    chunk_mesh->mesh->normals = (float *)MemAlloc(num_chunk_normals * sizeof(float));

    chunk_mesh->mesh->vertexCount = 0;
    chunk_mesh->mesh->triangleCount = 0;
   
    chunk_mesh->chunk = gen_chunk(world_pos, hash_table);

    return chunk_mesh;
}

// takes chunk world position to pass to block gen function. 
// we create and return a pointer to a chunk strut which now
// has blocks array full of pointers to blocks created in gen_block.
Chunk* gen_chunk(Vector3 world_pos, HashTable* hash_table) {
    Chunk* chunk = (Chunk*)calloc(1,sizeof(Chunk));

    chunk->world_pos = world_pos;

    int block_counter = 0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        //create all the conceptual blocks
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                chunk->blocks[i][j][k] = gen_block(world_pos, i, j, k, block_counter, hash_table);   
                block_counter++;

            }
        }
    }
    //TraceLog(LOG_WARNING, "hit gen chunk");
    return chunk;
}

// takes world position and three loop positions to get block position.
// culling should take place here since block textures are set here.
// vertices are set from top left triangle, then bottom right triangle,
// in a counter clockwise rotation starting from bottom left corner.
// tex coords were not too hard, just upside down due to difference between
// png and opengl. This is the lowest it goes, we return a pointer to the 
// data generated here.
Block gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ, int counter, HashTable* hash_table) {
    float posX = world_pos.x - HALF_CHUNK + blockX + 0.5f; //have to add 0.5 so it lines up.. for reasons..
    float posY = world_pos.y - HALF_CHUNK + blockY;
    float posZ = world_pos.z - HALF_CHUNK + blockZ + 0.5f;
    Vector3 blockpos = (Vector3){posX, posY, posZ};

    Block block = {0};
    
    block.block_type = DecideBlockType(blockpos);

    return block;
}

Chunk* GetCurrentChunk(Vector3 player_pos, HashTable* hash_table) {
    ChunkMesh* curr_chunkmesh = (ChunkMesh*)calloc(1,sizeof(ChunkMesh));

    int chunkX = (int)floor((player_pos.x + 8) / 16) * 16;
    int chunkY = (int)floor(((player_pos.y - PLAYER_HEIGHT) + 8) / 16) * 16;
    int chunkZ = (int)floor((player_pos.z + 8) / 16) * 16;
    curr_chunkmesh = FetchChunkEntry((Vector3) {chunkX,chunkY,chunkZ}, hash_table);

    return curr_chunkmesh->chunk;
}

BoundingBox GetNearbyBlocks(Vector3 player_pos, HashTable* hash_table) {
    //.. figuring this out
    // i probably want to figure out nearby blocks up to N, then
    // put some bounding boxes on them, return an array of those..

    Chunk* curr_chunk = (Chunk*)MemAlloc(sizeof(Chunk));
    curr_chunk = GetCurrentChunk(player_pos, hash_table);

    //next get the block at players feet
    // int relX = (int)floor((player_pos.x + 8) / 16);
    // int relY = (int)floor(((player_pos.y - PLAYER_HEIGHT) + 8) / 16);
    // int relZ = (int)floor((player_pos.z + 8) / 16);

    // if(player_pos.x != 0.0f && player_pos.y - PLAYER_HEIGHT != 0 && player_pos.z !=0) {

    // }

    // listen... best way to do this is to translate player pos / chunk world pos to a relative
    // block right below player, then translate that to real world coords, pass to box
        
    int relX = (int)floor(curr_chunk->world_pos.x / player_pos.x);
    int relY = floor(curr_chunk->world_pos.y / (player_pos.y - PLAYER_HEIGHT));
    int relZ = floor(curr_chunk->world_pos.z / player_pos.z);
    if(player_pos.x == 0.0f) relX = 8;
    if(player_pos.y - PLAYER_HEIGHT == 0.0f) relY = 8;
    if(player_pos.z == 0.0f) relZ = 8;

    Block* curr_block = (Block*)MemAlloc(sizeof(Block));
    curr_block = &curr_chunk->blocks[relX][relY][relZ];     //??


    //i would say... the chunk and the player pos have something in common. 
    //we know that 8,8,8 indices in chunk is physical 0,0,0 (relative) chunk position
    //ie, the center of the chunk. we just need to know how far away exactly the player
    //is from center of the chunk, then figuring out which block should be trivial
    int indexX = 0;
    int tempX = curr_chunk->world_pos.x - floor(player_pos.x);
    // if(tempX == -8) indexX = 0;
    // if(tempX == -7) indexX = 1;
    indexX = tempX + 8;

    int indexY = 0;
    int tempY = curr_chunk->world_pos.y - floor(player_pos.y - 2.0f); //2.0f to get block below player
    indexY = tempY + 8;

    int tempZ = curr_chunk->world_pos.z - floor(player_pos.z);
    int indexZ = tempZ + 8;

    float boxX = curr_chunk->world_pos.x - (indexX - 8); //seems redundant but we are just getting it working
    float boxY = curr_chunk->world_pos.y - (indexY - 8);
    float boxZ = curr_chunk->world_pos.z - (indexZ - 8);

    BoundingBox box = {
        (Vector3) {boxX, boxY, boxZ},
        (Vector3) {boxX + 1.0f, boxY + 1.0f, boxZ + 1.0f}
    };

    // just following players exact movements now.. we want to track block below player
    // BoundingBox box = {
    //     (Vector3) {player_pos.x, player_pos.y - PLAYER_HEIGHT + 0.5f, player_pos.z},
    //     (Vector3) {player_pos.x + 1.0f, player_pos.y - PLAYER_HEIGHT + 1.5f, player_pos.z + 1.0f}
    // };

    return box;
}
