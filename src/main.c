#include "include.h"


void draw_cube_basic(Vector3 position, Color color, Texture* texture);


// TO DO NEXT
// load (and unload) chunks as the player moves around based on player position DONE
// i'd like to have an initial array of uploaded chunks when player loads in, 1 in every direction
// then as the player goes about, we (slowly) add in mroe chunks
// i do NOT want to load chunks when player steps over a certain boundary

int main(void) {
    const int screenWidth = 2560;
    const int screenHeight = 1440;
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, 1.8f, 0.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, -5.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Texture texture = LoadTexture("assets/tex_atlas_2.png");
    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    // Vector3 current_chunk_pos = { 0.0f, 0.0f, 0.0f };
    // Vector3 closest_chunk_positions[NUM_RENDERED_CHUNKS];

    // // current_chunk_pos.x = floor((camera.position.x + HALF_CHUNK) / CHUNK_SIZE);
    // // current_chunk_pos.y = floor((camera.position.y + HALF_CHUNK) / CHUNK_SIZE);
    // // current_chunk_pos.z = floor((camera.position.z + HALF_CHUNK) / CHUNK_SIZE);

    // // initialize first 27 chunks around player
    // int chunk_counter = 0;
    // for (int x = -1; x < 2; x++) {
    //     for (int y = -1; y < 2; y++) {
    //         for (int z = -1; z < 2; z++) {
    //             closest_chunk_positions[chunk_counter] = (Vector3) { 
    //                 current_chunk_pos.x + (x*CHUNK_SIZE),
    //                 current_chunk_pos.y + (y*CHUNK_SIZE),
    //                 current_chunk_pos.z + (z*CHUNK_SIZE)
    //             };
    //             // TraceLog(LOG_WARNING, "closest chunk position %d X position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].x);
    //             // TraceLog(LOG_WARNING, "closest chunk position %d Y position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].y);
    //             // TraceLog(LOG_WARNING, "closest chunk position %d Z position: %.2f", chunk_counter, closest_chunk_positions[chunk_counter].z);
    //             chunk_counter += 1;
    //         }
    //     }
    // }

    HashTable* hash_table = InitializeTable(TABLE_CAPACITY);
    // int Xpos = floor((camera.position.x + 8) / 16) * 16;
    // int Ypos = floor(((camera.position.y - PLAYER_HEIGHT) + 8) / 16) * 16;
    // int Zpos = floor((camera.position.z + 8) / 16) * 16;
    // ChunkMesh* current_chunk = FetchChunkEntry((Vector3) { (float)Xpos, (float)Ypos, (float)Zpos }, hash_table); 
    // UploadMesh(current_chunk->mesh, false);

    // float oldX = (float) Xpos;
    // float oldY = (float) Ypos;
    // float oldZ = (float) Zpos;

    // ChunkMesh* closest_meshes[NUM_RENDERED_CHUNKS];
    // for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
    //     closest_meshes[i] = FetchChunkEntry(closest_chunk_positions[i], hash_table);
    //     UploadMesh(closest_meshes[i]->mesh, false); 
    // }



    // Vector3 player_base;
    // player_base.x = floor((camera.position.x / CHUNK_SIZE) + HALF_CHUNK);
    // player_base.y = floor((camera.position.y / CHUNK_SIZE) + HALF_CHUNK);
    // player_base.z = floor((camera.position.z / CHUNK_SIZE) + HALF_CHUNK);
    // Vector3 old_base = player_base;

    ChunkMesh* chunkmeshes[hash_table->capacity];
    //chunkmeshes[0] = current_chunk;
    int chunkcounter = 1;
    int test_counter = 0;
    int renderX = 1;
    int renderY = 1;
    int renderZ = 1;
    int mega_chunk_counter = 0;

    Vector3 mega_chunk_relative_positions[] = {
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

    // i don't need this right now but will want to do this later
    //mega_chunk_relative_positions[0] = current_chunk->chunk->world_pos; 

    MegaChunk* megachunks[MEGA_CHUNKS_MAX];

    megachunks[0] = GenMegaChunk(mega_chunk_relative_positions[0], hash_table);
    //megachunks[1] = GenMegaChunk(mega_chunk_relative_positions[1], hash_table);
    // for (int i = 0; i < MEGA_CHUNK_SIZE; i++) {
    //     UploadMesh(megachunks[0]->chunkmeshes[i]->mesh, false);
    // }
    UploadMesh(megachunks[0]->chunkmeshes[0]->mesh, false);
    UploadMesh(megachunks[0]->chunkmeshes[1]->mesh, false);

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
        //         closest_meshes[i] = FetchChunkEntry(closest_chunk_positions[i], hash_table);
        //         UploadMesh(closest_meshes[i]->mesh, false); 
        //     }
        //     //old_base = player_base;
        // }
        // Vector3 current_chunk_pos;
        // current_chunk_pos.x = floor((camera.position.x + HALF_CHUNK) / CHUNK_SIZE);
        // current_chunk_pos.y = floor((camera.position.y + HALF_CHUNK) / CHUNK_SIZE);
        // current_chunk_pos.z = floor((camera.position.z + HALF_CHUNK) / CHUNK_SIZE);

        // this should be JUST for tracking current chunk...
        // if(camera.position.x > oldX + HALF_CHUNK || camera.position.x <= oldX - HALF_CHUNK
        //     || camera.position.y > oldY + HALF_CHUNK || camera.position.y <= oldY - HALF_CHUNK
        //     || camera.position.z > oldZ + HALF_CHUNK || camera.position.z <= oldZ - HALF_CHUNK) {
        //     //UnloadMesh(*current_chunk->mesh);
        //     Xpos = floor((camera.position.x + 8) / 16) * 16;
        //     Ypos = floor(((camera.position.y - PLAYER_HEIGHT) + 8) / 16) * 16;
        //     Zpos = floor((camera.position.z + 8) / 16) * 16;
        //     current_chunk = FetchChunkEntry((Vector3) { (float)Xpos, (float)Ypos, (float)Zpos }, hash_table); 
        //     if (current_chunk->new == true) {
        //         //TraceLog(LOG_WARNING, "uploading new mesh");
        //         //UploadMesh(current_chunk->mesh, false);
        //         // if(chunkcounter <= hash_table->length) {
        //         //     chunkmeshes[chunkcounter] = current_chunk;
        //         //     chunkcounter++;
        //         // }
        //     }
        //     oldX = (float) Xpos;
        //     oldY = (float) Ypos;
        //     oldZ = (float) Zpos;

        //     //also... set up renderX,Y,Z
        //     renderX = 1;
        //     renderY = 1;
        //     renderZ = 1;
        // }

        // if(mega_chunk_counter < MEGA_CHUNKS_MAX) {
        //     //working on mega-chunk idea
        //     if(test_counter % 1000 == 0) {
        //         megachunks[mega_chunk_counter] = GenWorld(mega_chunk_relative_positions[mega_chunk_counter], hash_table);
        //         for (int i = 0; i < MEGA_CHUNK_SIZE; i++) {
        //             UploadMesh(megachunks[mega_chunk_counter]->chunkmeshes[i]->mesh, false);
        //         }
        //         mega_chunk_counter++;
        //     }
        // }


        BeginDrawing();
            
            ClearBackground(LIGHTGRAY);

            BeginMode3D(camera);

                // for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
                //     DrawMesh(*closest_meshes[i]->mesh, material, matrix);
                // }
                //DrawMesh(*current_chunk->mesh, material, matrix);

                
                // for(int i = 0; i < chunkcounter; i++) {
                //     if(chunkmeshes[chunkcounter]->chunk->world_pos == current_chunk->chunk->world_pos) {
                //         DrawMesh(*chunkmeshes[chunkcounter]->mesh, material, matrix);
                //     }
                // }

                // for (int i = 0; i < chunkcounter; i++) {
                //     DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                // }

                // for (int i = 0; i < MEGA_CHUNKS_MAX; i++) {
                //     for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
                //         for (int k = 0; k < CHUNK_SIZE; k++) {
                //             DrawMesh()
                //         }
                //     }
                // }

                // for (int i = 0; i < MEGA_CHUNKS_MAX; i++) {
                //     for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
                //         DrawMesh(*megachunks[i]->chunkmeshes[j]->mesh, material, matrix);
                //     }
                // }

                // for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
                //     DrawMesh(*megachunks[0]->chunkmeshes[j]->mesh, material, matrix);
                // }

                DrawMesh(*megachunks[0]->chunkmeshes[0]->mesh, material, matrix);
                DrawMesh(*megachunks[0]->chunkmeshes[1]->mesh, material, matrix);

            EndMode3D();
            
            DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", camera.position.x, camera.position.y, camera.position.z), 
                        140, 10, 20, YELLOW);
            // DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
            //                     current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
            //             640, 10, 20, YELLOW);
            DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            test_counter++;

            DrawFPS(10, 10);
        EndDrawing();
    }

    // for(int i = 0; i < NUM_RENDERED_CHUNKS; i++) {
    //     UnloadMesh(*closest_meshes[i]->mesh);
    // }
    //UnloadMesh(*current_chunk->mesh);
    UnloadTexture(texture);
    //free(chunkmeshes);
    //free(megachunks);
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