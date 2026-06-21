#include "raylib.h" 
#include "rlgl.h"
#include "raymath.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#ifndef INCLUDE_H
#define INCLUDE_H

#define PLAYER_HEIGHT 1.8f
#define GRAVITY -9.8f

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
#define PLAYER_WIDTH 0.8f
#define PLAYER_DEPTH 0.5f

#define NUM_BLOCK_VERTICES 36 * 3
#define NUM_BLOCK_TEXCOORDS 36 * 2
#define NUM_BLOCK_NORMALS 36 * 3
#define NUM_CHUNK_VERTICES NUM_BLOCK_VERTICES * CHUNK_CUBED
#define NUM_CHUNK_TEXCOORDS NUM_BLOCK_TEXCOORDS * CHUNK_CUBED
#define NUM_CHUNK_NORMALS NUM_BLOCK_NORMALS * CHUNK_CUBED

#define NEARBY_BOX_DEPTH 10
#define MAX_NEARBY_BOXES NEARBY_BOX_DEPTH*NEARBY_BOX_DEPTH*5

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

#define DIRT_DARK_TEX_COORD_U_MIN 0.415f
#define DIRT_DARK_TEX_COORD_U_MAX 0.585f
#define DIRT_DARK_TEX_COORD_V_MIN 0.023f
#define DIRT_DARK_TEX_COORD_V_MAX 0.195f

#define GRASS_TEX_COORD_U_MIN 0.22f
#define GRASS_TEX_COORD_U_MAX 0.38f
#define GRASS_TEX_COORD_V_MIN 0.42f
#define GRASS_TEX_COORD_V_MAX 0.58f

//about as close to perfect as i can get
#define GRASS_LIGHT_TEX_COORD_U_MIN 0.023f
#define GRASS_LIGHT_TEX_COORD_U_MAX 0.195f
#define GRASS_LIGHT_TEX_COORD_V_MIN 0.023f
#define GRASS_LIGHT_TEX_COORD_V_MAX 0.195f

// #define STONE_TEX_COORD_U_MIN 0.22f
// #define STONE_TEX_COORD_U_MAX 0.38f
// #define STONE_TEX_COORD_V_MIN 0.82f
// #define STONE_TEX_COORD_V_MAX 0.98f

#define STONE_TEX_COORD_U_MIN 0.609f
#define STONE_TEX_COORD_U_MAX 0.782f
#define STONE_TEX_COORD_V_MIN 0.023f
#define STONE_TEX_COORD_V_MAX 0.195f

#define LAVA_TEX_COORD_U_MIN 0.82f
#define LAVA_TEX_COORD_U_MAX 0.98f
#define LAVA_TEX_COORD_V_MIN 0.42f
#define LAVA_TEX_COORD_V_MAX 0.58f

#define MAGMA_TEX_COORD_U_MIN 0.789f
#define MAGMA_TEX_COORD_U_MAX 0.9f
#define MAGMA_TEX_COORD_V_MIN 0.023f
#define MAGMA_TEX_COORD_V_MAX 0.195f

#define DIRTGRASS_TEX_COORD_U_MIN 0.218f
#define DIRTGRASS_TEX_COORD_U_MAX 0.391f
#define DIRTGRASS_TEX_COORD_V_MIN 0.023f
#define DIRTGRASS_TEX_COORD_V_MAX 0.195f


extern Vector3 relative_positions[];

typedef struct Block {
    int8_t block_type;
} Block;

typedef struct Chunk {
    Vector3 world_pos;
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

typedef struct ChunkMesh {
    bool new;
    bool dirty;
    bool uploaded;
    Mesh* mesh;
    Chunk* chunk;
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

typedef struct Player {
    Vector3 position;
    BoundingBox bounding_box;
    Vector3 velocity;
    float acceleration; 
    bool on_ground;
    bool frozen;
    bool flying;
    float target_offset;
}Player;

// typedef struct MegaChunk {
//     ChunkMesh* chunkmeshes[3*3*3];
//     Vector3 center_pos;
// }MegaChunk;

// chunk.c
void gen_chunk_mesh(Vector3 world_pos, ChunkMesh* chunk_mesh);
void gen_chunk(Vector3 world_pos, Chunk* chunk);
Block gen_block(Vector3 world_pos, int blockX, int blockY, int blockZ);
void BreakBlock(Vector3 point, HashTable* hash_table);

// hash.c
HashTable* InitializeTable(int capacity);
void DestroyTable(HashTable* hash_table);
int32_t Hash(int32_t x, int32_t y, int32_t z, int32_t size);
ChunkMesh* CreateChunkEntry(Vector3 pos, HashTable* hash_table);
ChunkMesh* FetchChunkEntry(Vector3 pos, HashTable* hash_table);
bool DoesChunkEntryExist(Vector3 pos, HashTable* hash_table);

//world.c
void InitWorld();
int8_t DecideBlockType(Vector3 block_world_pos);
int SpiralTraversal2D(Vector3* coords, int coords_index, Vector3 pos, int depth);
int SpiralTraversal2DChunks(Vector3* coords, int coords_index, Vector3 pos, int depth);
Chunk* GetCurrentChunk(Vector3 player_pos, HashTable* hash_table);
Vector3 DeriveChunkPosition(Vector3 starting_pos);
ChunkMesh* DeriveChunkMesh(Vector3 starting_pos, HashTable* hash_table);
int GetNearbyBlocks(BoundingBox* boxes, Vector3 camera_pos, Vector3 player_pos, HashTable* hash_table);
Vector3 ConvertWorldBlockPosToChunkIndex(Vector3 block_world_pos);
Vector3 ConvertChunkIndexToWorldBlockPos(Vector3 chunk_index, Vector3 chunk_world_pos);
bool IsBlockVisibleImproved(Vector3 block_world_pos, HashTable* hash_table);
bool IsBlockAir(Vector3 block_world_pos, HashTable* hash_table);

//mesh.c
void GenMeshChunk(Mesh* mesh, Chunk* chunk, HashTable* hash_table);
void GenMeshChunkSimplified(Mesh* mesh, Chunk* chunk, HashTable* hash_table);
void GenMeshChunkRework(ChunkMesh* chunk_mesh, HashTable* hash_table);
bool IsBlockVisible(Vector3 chunk_pos, Vector3 block_pos, int blockX, int blockY, int blockZ, HashTable* hash_table);
bool IsBlockVisibleRework(Vector3 block_world_position, HashTable* hash_table);

//player.c
void InitPlayer(Player* player, Camera* camera);
void UpdatePlayer(Player* player, Camera* camera, BoundingBox* boxes, int nearby_boxes_count);

#endif
