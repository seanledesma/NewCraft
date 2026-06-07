#include "include.h"

//fnl_state noise;


void draw_cube_basic(Vector3 position, Color color, Texture* texture);


// TO DO NEXT
// load (and unload) chunks as the player moves around based on player position DONE
// i'd like to have an initial array of uploaded chunks when player loads in, 1 in every direction
// then as the player goes about, we (slowly) add in mroe chunks
// i do NOT want to load chunks when player steps over a certain boundary

int main(void) {

    //SetTraceLogLevel(LOG_DEBUG);

    // Vector3* coords = (Vector3*)MemAlloc(270 * sizeof(Vector3));
    // Vector3 pos = (Vector3) { 10.0f, 0.0f, 10.0f };
    // SpiralTraversal3D(coords, pos, 6);
    // for(int i = 0; i < 270; i++) {
    //     TraceLog(LOG_WARNING, TextFormat("x: %.2f, y: %.2f, z: %.2f", coords[i].x, coords[i].y, coords[i].z));
    // }

    // free(coords);
    // return(0);

    const int screenWidth = 2560;
    const int screenHeight = 1440;
    // const int screenWidth = GetMonitorWidth(0);
    // const int screenHeight = GetMonitorHeight(0);
    // const int cross_hair_width = screenWidth * 0.01f;
    // const int cross_hair_height = screenHeight * 0.01f;
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, PLAYER_HEIGHT-10, 0.0f };
    camera.target = (Vector3) { camera.position.x, camera.position.y, camera.position.z - 1 };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Player player = {0};
    InitPlayer(&player, &camera);



    // fnl_state noise = fnlCreateState();
    // noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    InitWorld();

    Texture texture = LoadTexture("assets/my_tex_atlas.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    HashTable* hash_table = InitializeTable(TABLE_CAPACITY);

    ChunkMesh* chunkmeshes[hash_table->capacity];
    //chunkmeshes[0] = current_chunk;
    int chunkcounter = 1;
    int test_counter = 0;
    int renderX = 1;
    int renderY = 1;
    int renderZ = 1;



    // i don't need this right now but will want to do this later
    //mega_chunk_relative_positions[0] = current_chunk->chunk->world_pos; 

    //MegaChunk* megachunks[MEGA_CHUNKS_MAX];

    //load 27 megachunks in one go
    // while(mega_chunk_counter < MEGA_CHUNKS_MAX) {
    //     //working on mega-chunk idea
    //     megachunks[mega_chunk_counter] = GenMegaChunk(relative_positions[mega_chunk_counter], hash_table);
    //     for (int i = 0; i < MEGA_CHUNK_SIZE; i++) {
    //         UploadMesh(megachunks[mega_chunk_counter]->chunkmeshes[i]->mesh, false);
    //     }
    //     mega_chunk_counter++;
    // }

    // load one mega chunk
    // megachunks[0] = GenMegaChunk(relative_positions[0], hash_table);
    // for (int i = 0; i < MEGA_CHUNK_SIZE; i++) {
    //     UploadMesh(megachunks[0]->chunkmeshes[i]->mesh, false);
    // }


    // UploadMesh(megachunks[0]->chunkmeshes[0]->mesh, false);
    // UploadMesh(megachunks[0]->chunkmeshes[1]->mesh, false);

    //first, virtually create all chunkmeshes 
    // int rel_pos_counter = 0;
    // int mult_factor = 1;
    // for (int i = 0; i < 81; i++) {
    //     if(rel_pos_counter == 27) {
    //         mult_factor++;
    //         rel_pos_counter = 0;
    //     }
    //     Vector3 chunk_pos = (Vector3) { relative_positions[rel_pos_counter].x * (CHUNK_SIZE * mult_factor),
    //                                     relative_positions[rel_pos_counter].y * (CHUNK_SIZE * mult_factor),
    //                                     relative_positions[rel_pos_counter].z * (CHUNK_SIZE * mult_factor)};

    //     chunkmeshes[i] = FetchChunkEntry(chunk_pos, hash_table);

    //     rel_pos_counter++;
    // }

    // something temporary until i figure out chunk loading sequence + only showing chunks player can see
    // int count = 0;
    // for (int x = -RENDER_DISTANCE_X; x <= RENDER_DISTANCE_X; x++) {
    //     for (int y = -RENDER_DISTANCE_Y; y <= RENDER_DISTANCE_Y; y++) {
    //         for (int z = -RENDER_DISTANCE_Z; z <= RENDER_DISTANCE_Z; z++) {
    //             Vector3 chunk_pos = (Vector3) { x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE };

    //             chunkmeshes[count] = FetchChunkEntry(chunk_pos, hash_table);
    //             count += 1;
    //         }
    //     }
    // }

    //TraceLog(LOG_WARNING, "size of chunkmeshes: %d", (sizeof(chunkmeshes) / sizeof(chunkmeshes[0])));

    //next, create all the meshes for each chunkmesh
    // for (int i = 0; i < count; i++) {
    //     //TraceLog(LOG_WARNING, "index no.: %d", i);
    //     GenMeshChunk(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
    //     UploadMesh(chunkmeshes[i]->mesh, false);
    // }

    // Vector3* coords = (Vector3*)MemAlloc(270 * sizeof(Vector3));
    // Vector3 pos = (Vector3) { 10.0f, 0.0f, 10.0f };
    // SpiralTraversal3D(coords, pos, 6);
    // for(int i = 0; i < 270; i++) {
    //     TraceLog(LOG_WARNING, TextFormat("x: %.2f, y: %.2f, z: %.2f", coords[i].x, coords[i].y, coords[i].z));
    // }

    // generating chunks starting from center
    // Vector3* coords = (Vector3*)MemAlloc(1000 * sizeof(Vector3));
    // Vector3 starting_position = (Vector3) { 0.0f, 0.0f, 0.0f };
    int depth = 10;
    int count = 0;
    // int coords_counter = 0;
    // coords_counter = SpiralTraversal2D(coords, coords_counter, starting_position, depth);
    // coords_counter = SpiralTraversal2D(coords, coords_counter, 
    //     (Vector3) {
    //         starting_position.x,
    //         starting_position.y + 1,
    //         starting_position.z
    //     }, depth);

    // coords_counter = SpiralTraversal2D(coords, coords_counter, 
    //     (Vector3) {
    //         starting_position.x,
    //         starting_position.y - 1,
    //         starting_position.z
    //     }, depth);
    // create all chunks
    // for (int i = 0; i < coords_counter; i++) {
    //     chunkmeshes[i] = FetchChunkEntry((Vector3) { 
    //         coords[i].x * CHUNK_SIZE,
    //         coords[i].y * CHUNK_SIZE,
    //         coords[i].z * CHUNK_SIZE
    //      }, hash_table);
    //      count++;
    // }

    // then create all meshes
    // for (int i = 0; i < coords_counter; i++) {
    //     GenMeshChunkRework(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
    //     //GenMeshChunk(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
    //     UploadMesh(chunkmeshes[i]->mesh, false);
    // }

    chunkmeshes[0] = FetchChunkEntry(relative_positions[0], hash_table);
    // GenMeshChunk(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    GenMeshChunkRework(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    UploadMesh(chunkmeshes[0]->mesh, false);

    Model model = LoadModelFromMesh(*chunkmeshes[0]->mesh);
    Ray ray = {0};
    RayCollision collision = {0};
    // make boxes as big as it may ever possibly get
    BoundingBox* boxes = (BoundingBox*)MemAlloc(sizeof(BoundingBox) * depth*depth*5);
    int nearby_bounding_box_counter = 0;

    DisableCursor();
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);
        UpdatePlayer(&player, &camera, boxes);

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
        TraceLog(LOG_DEBUG, TextFormat("0starting pos in main y: %.2f", camera.position.y));
        //nearby_bounding_box_counter = GetNearbyBlocks(boxes, camera.position, hash_table);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // ray = GetScreenToWorldRay(GetMousePosition(), camera);
            ray = GetScreenToWorldRay((Vector2) { screenWidth/2, screenHeight / 2 }, camera);
            //collision = GetRayCollisionMesh(ray, model.meshes[0], model.transform);

            ray.direction = Vector3Normalize(ray.direction);
            //collision = GetRayCollisionMesh(ray, *chunkmeshes[0]->mesh, matrix);
            // collision = GetRayCollisionBox(ray, boxes[box_counter]);

            //run through all nearby boxes real quick
            int box_counter = 0;
            for (int i = 0; i < nearby_bounding_box_counter; i++) {
                collision = GetRayCollisionBox(ray, boxes[box_counter]);
                if (collision.hit) {
                    //break a block!
                    // BreakBlock(collision.point, hash_table);
                    //BreakBlock(boxes[box_counter].min, hash_table);
                    break;
                }
                box_counter++;
            }
        }

        // check for dirty chunks to re-make
        // for (int i = 0; i < coords_counter; i++) {
        //     if(chunkmeshes[i]->dirty == true) {

        //         chunkmeshes[i]->dirty = false;

        //         UnloadMesh(*chunkmeshes[i]->mesh);
        //         //do i need to free mesh or will that cause issues?
        //         MemFree(chunkmeshes[i]->mesh);
        //         Mesh* mesh = (Mesh*)calloc(1,sizeof(Mesh));
                
        //         int num_blocks_in_chunk = CHUNK_CUBED;

        //         int num_block_vertices = 36 * 3;
        //         int num_block_texcoords = 36 * 2;
        //         int num_block_normals = 36 * 3;

        //         int num_chunk_vertices = num_block_vertices * num_blocks_in_chunk;
        //         int num_chunk_texcoords = num_block_texcoords * num_blocks_in_chunk;
        //         int num_chunk_normals = num_block_normals * num_blocks_in_chunk;
        //         mesh->vertices = (float *)MemAlloc(num_chunk_vertices * sizeof(float));
    
        //         mesh->texcoords = (float *)MemAlloc(num_chunk_texcoords * sizeof(float));
            
        //         mesh->normals = (float *)MemAlloc(num_chunk_normals * sizeof(float));

        //         mesh->vertexCount = 0;
        //         mesh->triangleCount = 0;


        //         chunkmeshes[i]->mesh = mesh;
        //         GenMeshChunkSimplified(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
        //         UploadMesh(chunkmeshes[i]->mesh, false);
        //     }
        // }

        //TraceLog(LOG_WARNING, TextFormat("IN MAIN LOOP what is block type under player: %d", chunkmeshes[0]->chunk->blocks[8][8][8].block_type));

        BeginDrawing();
            
            ClearBackground(BLACK);

            BeginMode3D(camera);
            DrawGrid(20, 1);

                // for (int i = 0; i < MEGA_CHUNKS_MAX; i++) {
                //     for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
                //         DrawMesh(*megachunks[i]->chunkmeshes[j]->mesh, material, matrix);
                //     }
                // }

                // draw just one mega chunk
                // for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
                //     DrawMesh(*megachunks[0]->chunkmeshes[j]->mesh, material, matrix);
                // }

                // for(int i = 0; i < coords_counter; i++) {
                //     DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                // }

                DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                
                // for(int i = 0; i < nearby_bounding_box_counter; i++) {
                //     DrawBoundingBox(boxes[i], ORANGE);
                // }
                
                // DrawBoundingBox(boxes[1], PURPLE);
                // DrawBoundingBox(boxes[2], PURPLE);
                // DrawBoundingBox(boxes[3], PURPLE);
                // DrawBoundingBox(boxes[4], PURPLE);
                // DrawBoundingBox(boxes[5], PURPLE);
                // DrawBoundingBox(boxes[6], PURPLE);
                // DrawBoundingBox(boxes[7], PURPLE);
                // DrawBoundingBox(boxes[8], PURPLE);
                // DrawBoundingBox(boxes[9], PURPLE);

                //DrawBoundingBox(boxes[0], PINK);

                // DrawMesh(*megachunks[0]->chunkmeshes[0]->mesh, material, matrix);
                // DrawMesh(*megachunks[0]->chunkmeshes[1]->mesh, material, matrix);

                if (collision.hit) DrawCube(collision.point, 0.3f, 0.3f, 0.3f, RED);

                DrawLine3D((Vector3) { camera.target.x, camera.target.y, camera.target.z }, 
                            (Vector3) { camera.target.x + 0.1f, camera.target.y, camera.target.z }, RED);
                DrawLine3D((Vector3) { camera.target.x, camera.target.y, camera.target.z }, 
                            (Vector3) { camera.target.x, camera.target.y + 0.1f, camera.target.z }, GREEN);
                DrawLine3D((Vector3) { camera.target.x, camera.target.y, camera.target.z }, 
                            (Vector3) { camera.target.x, camera.target.y, camera.target.z + 0.1f }, BLUE);

            EndMode3D();

            
            DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", camera.position.x, camera.position.y-PLAYER_HEIGHT, camera.position.z), 
                        140, 10, 20, YELLOW);

            // DrawText(TextFormat("box position x:%.2f, y:%.2f, z:%.2f", box.min.x, box.min.y, box.min.z), 
            //             540, 10, 20, YELLOW);
            // DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
            //                     current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
            //             640, 10, 20, YELLOW);
            DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            test_counter++;

            if(collision.hit) DrawText("hit!", 900, 10, 50, YELLOW);

            // DrawRectangle((screenWidth/2) - 20, (screenHeight / 2), 45, 5, GRAY);
            // DrawRectangle((screenWidth/2), (screenHeight / 2) - 20, 5, 45, GRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    // with multiple megachunks
    // for (int i = 0; i < MEGA_CHUNKS_MAX; i++) {
    //     for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
    //         UnloadMesh(*megachunks[i]->chunkmeshes[j]->mesh);
    //     }
    // }

    // to unload just one mega chunk
    // for (int j = 0; j < MEGA_CHUNK_SIZE; j++) {
    //     UnloadMesh(*megachunks[0]->chunkmeshes[j]->mesh);
    // }

    // for (int i = 0; i < count; i++) {
    //     UnloadMesh(*chunkmeshes[i]->mesh);
    // }

    UnloadMesh(*chunkmeshes[0]->mesh);

    UnloadModel(model);

    UnloadTexture(texture);
    //free(chunkmeshes);
    //free(megachunks);
    DestroyTable(hash_table);
    CloseWindow();

    return 0;       
}
