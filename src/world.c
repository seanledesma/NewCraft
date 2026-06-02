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
            TraceLog(LOG_WARNING, TextFormat("posZ = %d, posZ + pos.z = %d", posZ, posZ+(int)pos.z));
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

    // do it again for y-1
    posX = 0;
    posZ = 0;
    dx = 0;
    dz = -1;
    temp = 0;
    
    for(int i = 0; i < maxI; i++) {

        if ((-depth/2 <= posX) && (posX <= depth/2) && (-depth/2 <= posZ) && (posZ <= depth/2)) {
            TraceLog(LOG_WARNING, TextFormat("posZ = %d, posZ + pos.z = %d", posZ, posZ+(int)pos.z));
            coords[coord_counter++] = (Vector3) {
                pos.x + posX,
                pos.y - 1.0f,
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

    posX = 0;
    posZ = 0;
    dx = 0;
    dz = -1;
    temp = 0;
    
    for(int i = 0; i < maxI; i++) {

        if ((-depth/2 <= posX) && (posX <= depth/2) && (-depth/2 <= posZ) && (posZ <= depth/2)) {
            TraceLog(LOG_WARNING, TextFormat("posZ = %d, posZ + pos.z = %d", posZ, posZ+(int)pos.z));
            coords[coord_counter++] = (Vector3) {
                pos.x + posX,
                pos.y + 1.0f,
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

