#include "raylib.h" 
#include "rlgl.h"
#include "raymath.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#ifndef INCLUDE_H
#define INCLUDE_H

#define RENDER_DISTANCE 3

#define CHUNK_SIZE 16
#define HALF_CHUNK CHUNK_SIZE/2
#define CHUNK_CUBED (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)
#define RENDER_DISTANCE_X 3
#define RENDER_DISTANCE_Y 1
#define RENDER_DISTANCE_Z 3
#define NUM_RENDERED_CHUNKS (RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE)
#define TABLE_CAPACITY 100000

#define MEGA_CHUNKS_MAX 27
#define MEGA_CHUNK_SIZE 27
#define DISTANCE_BETWEEN_MEGA_CHUNKS 48

#define PLAYER_HEIGHT 1.8f

#define BLOCK_AIR (int8_t)0
#define BLOCK_GRASS (int8_t)1
#define BLOCK_DIRT (int8_t)2
#define BLOCK_STONE (int8_t)3
#define BLOCK_LAVA (int8_t)4
#define BLOCK_MAGMA (int8_t)5
#define BLOCK_UNKNOWN (int8_t)6

#define DIRT_TEX_COORD_U_MIN 0.05f
#define DIRT_TEX_COORD_U_MAX 0.18f
#define DIRT_TEX_COORD_V_MIN 0.82f
#define DIRT_TEX_COORD_V_MAX 0.98f
#define DIRT_LIGHT_TEX_COORD_U_MIN 0.22f
#define DIRT_LIGHT_TEX_COORD_U_MAX 0.38f
#define DIRT_LIGHT_TEX_COORD_V_MIN 0.22f
#define DIRT_LIGHT_TEX_COORD_V_MAX 0.38f

#define DIRT_DARK_TEX_COORD_U_MIN 0.62f
#define DIRT_DARK_TEX_COORD_U_MAX 0.78f
#define DIRT_DARK_TEX_COORD_V_MIN 0.82f
#define DIRT_DARK_TEX_COORD_V_MAX 0.98f

#define GRASS_TEX_COORD_U_MIN 0.22f
#define GRASS_TEX_COORD_U_MAX 0.38f
#define GRASS_TEX_COORD_V_MIN 0.42f
#define GRASS_TEX_COORD_V_MAX 0.58f
#define GRASS_LIGHT_TEX_COORD_U_MIN 0.42f
#define GRASS_LIGHT_TEX_COORD_U_MAX 0.58f
#define GRASS_LIGHT_TEX_COORD_V_MIN 0.42f
#define GRASS_LIGHT_TEX_COORD_V_MAX 0.58f

// #define STONE_TEX_COORD_U_MIN 0.22f
// #define STONE_TEX_COORD_U_MAX 0.38f
// #define STONE_TEX_COORD_V_MIN 0.82f
// #define STONE_TEX_COORD_V_MAX 0.98f

#define STONE_TEX_COORD_U_MIN 0.22f
#define STONE_TEX_COORD_U_MAX 0.38f
#define STONE_TEX_COORD_V_MIN 0.82f
#define STONE_TEX_COORD_V_MAX 0.98f

#define LAVA_TEX_COORD_U_MIN 0.82f
#define LAVA_TEX_COORD_U_MAX 0.98f
#define LAVA_TEX_COORD_V_MIN 0.42f
#define LAVA_TEX_COORD_V_MAX 0.58f

#define MAGMA_TEX_COORD_U_MIN 0.42f
#define MAGMA_TEX_COORD_U_MAX 0.58f
#define MAGMA_TEX_COORD_V_MIN 0.82f
#define MAGMA_TEX_COORD_V_MAX 0.98f

extern Vector3 relative_positions[];

typedef struct Block {
    int8_t block_type;
} Block;

typedef struct Chunk {
    Vector3 world_pos;
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

typedef struct ChunkMesh {
    Mesh* mesh;
    Chunk* chunk;
    bool new;
} ChunkMesh;

typedef struct TableEntry {
    bool empty;
    Vector3 key;
    int32_t value;
    ChunkMesh* chunk_mesh;
}TableEntry;

typedef struct HashTable {
    TableEntry** entries;
    int32_t capacity; // size of array
    int32_t length; // how many entries
}HashTable;

typedef struct MegaChunk {
    ChunkMesh* chunkmeshes[3*3*3];
    Vector3 center_pos;
}MegaChunk;

// chunk.c
ChunkMesh* gen_chunk_mesh(Vector3 world_pos, HashTable* hash_table);
Chunk* gen_chunk(Vector3 world_pos, HashTable* hash_table);
Block gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ, int counter, HashTable* hash_table);

// hash.c
HashTable* InitializeTable(uint32_t capacity);
void DestroyTable(HashTable* hash_table);
int32_t Hash(int32_t x, int32_t y, int32_t z, int32_t size);
ChunkMesh* CreateChunkEntry(Vector3 pos, HashTable* hash_table);
ChunkMesh* FetchChunkEntry(Vector3 pos, HashTable* hash_table);
bool DoesChunkEntryExist(Vector3 pos, HashTable* hash_table);

//world.c
void InitWorld();
int8_t DecideBlockType(Vector3 block_world_pos);
void SpiralTraversal3D(Vector3* coords, Vector3 pos, int depth);
Chunk* GetCurrentChunk(Vector3 player_pos, HashTable* hash_table);
Vector3 DeriveChunkPosition(Vector3 starting_pos, HashTable* hash_table);
Chunk* DeriveChunk(Vector3 starting_pos, HashTable* hash_table);
BoundingBox* GetNearbyBlocks(Vector3 player_pos, HashTable* hash_table);
Vector3 ConvertWorldBlockPosToChunkIndex(Vector3 block_world_pos, HashTable* hash_table);
Vector3 ConvertChunkIndexToWorldBlockPos(Vector3 chunk_index, Vector3 chunk_world_pos, HashTable* hash_table);
bool IsBlockVisibleImproved(Vector3 block_world_pos, HashTable* hash_table);
bool IsBlockAir(Vector3 block_world_pos, HashTable* hash_table);

//mesh.c
void GenMeshChunk(Mesh* mesh, Chunk* chunk, HashTable* hash_table);
bool IsBlockVisible(Vector3 chunk_pos, Vector3 block_pos, int blockX, int blockY, int blockZ, HashTable* hash_table);

//player.c
void UpdatePlayer(Camera* camera);

#endif
