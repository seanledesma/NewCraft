#include "include.h"


void draw_cube_basic(Vector3 position, Color color, Texture* texture);


// TO DO NEXT
// load (and unload) chunks as the player moves around based on player position
// face culling

int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, -4.0f, 0.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, -5.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Texture texture = LoadTexture("assets/tex_atlas_2.png");
    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    Vector3 current_chunk_pos = { 0.0f, 0.0f, 0.0f };
    Vector3 closest_chunk_positions[NUM_RENDERED_CHUNKS];

    // current_chunk_pos.x = floor((camera.position.x + HALF_CHUNK) / CHUNK_SIZE);
    // current_chunk_pos.y = floor((camera.position.y + HALF_CHUNK) / CHUNK_SIZE);
    // current_chunk_pos.z = floor((camera.position.z + HALF_CHUNK) / CHUNK_SIZE);

    // initialize first 27 chunks around player
    int chunk_counter = 0;
    for (int x = -1; x < 2; x++) {
        for (int y = -1; y < 2; y++) {
            for (int z = -1; z < 2; z++) {
                closest_chunk_positions[chunk_counter] = (Vector3) { 
                    current_chunk_pos.x + (x*CHUNK_SIZE),
                    current_chunk_pos.y + (y*CHUNK_SIZE),
                    current_chunk_pos.z + (z*CHUNK_SIZE)
                };
                // TraceLog(LOG_WARNING, "closest chunk position %d X position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].x);
                // TraceLog(LOG_WARNING, "closest chunk position %d Y position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].y);
                // TraceLog(LOG_WARNING, "closest chunk position %d Z position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].z);
                chunk_counter += 1;
            }
        }
    }

    HashTable* hash_table = InitializeTable(TABLE_CAPACITY);

    ChunkMesh* chunkmeshes[NUM_RENDERED_CHUNKS];
    for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
        chunkmeshes[i] = FetchChunkEntry(closest_chunk_positions[i], hash_table);
        UploadMesh(chunkmeshes[i]->mesh, false); 
    }



    // Vector3 player_base;
    // player_base.x = floor((camera.position.x / CHUNK_SIZE) + HALF_CHUNK);
    // player_base.y = floor((camera.position.y / CHUNK_SIZE) + HALF_CHUNK);
    // player_base.z = floor((camera.position.z / CHUNK_SIZE) + HALF_CHUNK);
    // Vector3 old_base = player_base;

    int test_counter = 0;
    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);

        // player_base.x = floor((camera.position.x / CHUNK_SIZE) + HALF_CHUNK);
        // player_base.y = floor((camera.position.y / CHUNK_SIZE) + HALF_CHUNK);
        // player_base.z = floor((camera.position.z / CHUNK_SIZE) + HALF_CHUNK);
        // if(old_base.x != player_base.x || old_base.y != player_base.y || old_base.z != player_base.z) {
        //     old_base = player_base;
        //     int chunk_counter = 0;
        //     for (int x = -1; x < 2; x++) {
        //         for (int y = -1; y < 2; y++) {
        //             for (int z = -1; z < 2; z++) {
        //                 closest_chunk_positions[chunk_counter++] = (Vector3) { 
        //                     player_base.x + (x*CHUNK_SIZE),
        //                     player_base.y + (y*CHUNK_SIZE),
        //                     player_base.z + (z*CHUNK_SIZE)
        //                 };
        //             }
        //         }
        //     }
        //     for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
        //         chunkmeshes[i] = FetchChunkEntry(closest_chunk_positions[i], hash_table);
        //         UploadMesh(chunkmeshes[i]->mesh, false); 
        //     }
        //     //old_base = player_base;
        // }
        // Vector3 current_chunk_pos;
        // current_chunk_pos.x = floor((camera.position.x + HALF_CHUNK) / CHUNK_SIZE);
        // current_chunk_pos.y = floor((camera.position.y + HALF_CHUNK) / CHUNK_SIZE);
        // current_chunk_pos.z = floor((camera.position.z + HALF_CHUNK) / CHUNK_SIZE);

        BeginDrawing();
            
            ClearBackground(LIGHTGRAY);

            BeginMode3D(camera);

                for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
                    DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                }

            EndMode3D();
            
            DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", camera.position.x, camera.position.y, camera.position.z), 
                        140, 10, 20, YELLOW);
            DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
                                current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
                        640, 10, 20, YELLOW);
            DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            test_counter++;

            DrawFPS(10, 10);
        EndDrawing();
    }

    for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
        UnloadMesh(*chunkmeshes[i]->mesh);
    }
    UnloadTexture(texture);
    DestroyTable(hash_table);
    CloseWindow();

    return 0;       
}


void draw_cube_basic(Vector3 pos, Color color, Texture* texture) {
    float size = 0.5f;

    rlPushMatrix();

        rlSetTexture(texture->id);
         rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            // for whatever reason, RL_QUADS goes bottom left, bottom right, top right, top left... counter clockwise
            // and, again for whatever reason, png format has origin at top left, opengl at bottom left, so must swap tex coords here (not intuitive)
            // Z-POSITIVE face
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z+size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z+size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z+size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z+size);
            
            // Z-NEGATIVE FACE
            rlNormal3f(0.0f, 0.0f, -1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z-size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z-size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z-size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z-size);


            // Y-POSITIVE FACE (TOP)
            rlNormal3f(0.0f, 1.0f, 0.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z+size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z+size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z-size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z-size);


            // Y-NEGATIVE (BOTTOM FACE)
            rlNormal3f(0.0f, 1.0f, 0.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z+size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z+size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z-size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z-size);
            

            // X-NEGATIVE FACE (LEFT)
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z-size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x-size, pos.y-size, pos.z+size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z+size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x-size, pos.y+size, pos.z-size);
            

            // X-POSITIVE FACE (RIGHT)
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z+size);

            // Vertex 3: Bottom right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x+size, pos.y-size, pos.z-size);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z-size);            

            // Vertex 1: Top left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x+size, pos.y+size, pos.z+size);

        rlEnd();

        rlSetTexture(0);
    rlPopMatrix();
}