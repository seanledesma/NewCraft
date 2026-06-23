#include "include.h"

// TO DO NEXT
// player collisions with floor / nearby blocks
// add / remove blocks

int main(void) {
    //SetTraceLogLevel(LOG_DEBUG);
    float debugging = true;

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 100.0f, PLAYER_HEIGHT, 100.0f };
    camera.target = (Vector3) { camera.position.x-1000, camera.position.y-1000, camera.position.z - 1000 };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Player player = {0};
    InitPlayer(&player, &camera);

    //debugging / xyz color lines
    Vector3 target_offset = {0};
    float travel = 0.0f;
    float distance = 0.0f;

    int box_counter = 0;


    InitWorld();

    Texture texture = LoadTexture("assets/my_tex_atlas.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    HashTable* hash_table = InitializeTable(TABLE_CAPACITY);

    //ChunkMesh* chunkmeshes[hash_table->capacity];

    Vector3 starting_position = DeriveChunkPosition(player.position);    
    int number_of_chunkmeshes = 100;
    ChunkMesh** chunkmeshes = (ChunkMesh**)MemAlloc(hash_table->capacity * sizeof(ChunkMesh));

    int depth = 10;
    Vector3* coords = (Vector3*)MemAlloc(hash_table->capacity * sizeof(Vector3));
    int total_coords = 0;
    total_coords = SpiralTraversal2DChunks(coords, total_coords, starting_position, depth);
    float timer_chunks = 0.0f;
    float timer_mesh = 0.0f;
    float timer_player = 0.0f;

    // // inner_coords_counter = SpiralTraversal2D(inner_coords, inner_coords_counter, 
    // //     (Vector3) {
    // //         starting_position.x,
    // //         starting_position.y + 1,
    // //         starting_position.z
    // //     }, depth);

    // // inner_coords_counter = SpiralTraversal2D(inner_coords, inner_coords_counter, 
    // //     (Vector3) {
    // //         starting_position.x,
    // //         starting_position.y - 1,
    // //         starting_position.z
    // //     }, depth);


    //create all chunks closet to player
    for (int i = 0; i < number_of_chunkmeshes; i++) {
        chunkmeshes[i] = FetchChunkEntry((Vector3) { 
            coords[i].x,
            coords[i].y,
            coords[i].z
         }, hash_table);
    }

    //then create all meshes
    for (int i = 0; i < number_of_chunkmeshes; i++) {
        GenMeshChunkRework(chunkmeshes[i], hash_table);
        chunkmeshes[i]->uploaded = true;
        UploadMesh(chunkmeshes[i]->mesh, false);
    }

    Vector3 current_chunk_pos = starting_position;

    //chunkmeshes[0] = FetchChunkEntry(starting_position, hash_table);
    // GenMeshChunk(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    // GenMeshChunkRework(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    // UploadMesh(chunkmeshes[0]->mesh, false);

    Ray ray = {0};
    RayCollision collision = {0};
    // make boxes as big as it may ever possibly get
    BoundingBox* boxes = (BoundingBox*)MemAlloc(sizeof(BoundingBox) * MAX_NEARBY_BOXES);
    int nearby_bounding_box_counter = 0;
    BoundingBox target_box = {0};

    DisableCursor();
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        //UpdateCameraCustom(&camera, cameraMode);
        UpdatePlayer(&player, &camera, boxes, nearby_bounding_box_counter, hash_table);

        // every tenth of a second update player location
        // timer_player += GetFrameTime();
        // if (timer_player >= 0.1f) {
        //     timer_player = 0.0f;
        //     player.position = camera.position;
        // }

        Vector3 temp_chunk_pos = DeriveChunkPosition(player.position);

        // the following creates chunks and meshes immediately surrounding player
        if ( temp_chunk_pos.x != current_chunk_pos.x 
            || temp_chunk_pos.y != current_chunk_pos.y 
            || temp_chunk_pos.z != current_chunk_pos.z ) 
        {
            current_chunk_pos = temp_chunk_pos;

            total_coords = 0;
            total_coords = SpiralTraversal2DChunks(coords, total_coords, current_chunk_pos, depth);

            //create all chunks closet to player
            //if they don't exist yet, DON'T create them all at once, just skip for now
            for (int i = 0; i < number_of_chunkmeshes; i++) {
                if(DoesChunkEntryExist((Vector3) { coords[i].x, coords[i].y, coords[i].z }, hash_table)) {                                                    
                    chunkmeshes[i] = FetchChunkEntry((Vector3) { 
                        coords[i].x,
                        coords[i].y,
                        coords[i].z
                    }, hash_table);
                }
            }

            //then create all meshes IF there are any new ones
            for (int i = 0; i < number_of_chunkmeshes; i++) {
                if(DoesChunkEntryExist((Vector3) { coords[i].x, coords[i].y, coords[i].z }, hash_table)) {
                    if (chunkmeshes[i]->dirty) {
                        GenMeshChunkRework(chunkmeshes[i], hash_table);
                        UploadMesh(chunkmeshes[i]->mesh, false);
                        chunkmeshes[i]->uploaded = true;
                    }
                }
            }
        }

        // in the background, slowly generate chunk data without generating meshes yet
        timer_chunks += GetFrameTime();
        timer_mesh += GetFrameTime();
        if (timer_chunks > 0.2) {
            timer_chunks = 0.0f;

            // go through coords up to number of chunkmeshes, if they HAVN'T been made, make one then break
            for (int i = 0; i < number_of_chunkmeshes; i++) {
                if(!DoesChunkEntryExist((Vector3) { coords[i].x, coords[i].y, coords[i].z }, hash_table)) {                                                    
                    chunkmeshes[i] = FetchChunkEntry((Vector3) { 
                        coords[i].x,
                        coords[i].y,
                        coords[i].z
                    }, hash_table);
                }
            }
            /*
                optional: i could then run through ALL coords, basically past total number of chunkmeshes to end of coords
                and unload meshes if they exist to save on memory space.
            */
        }

        if(timer_mesh >= 0.6) {
            timer_mesh = 0;
            for (int i = 0; i < number_of_chunkmeshes; i++) {
                //make sure we are checking a chunk that exists
                if(DoesChunkEntryExist((Vector3) { coords[i].x, coords[i].y, coords[i].z }, hash_table)) {
                    if(chunkmeshes[i]->dirty) {
                        GenMeshChunkRework(chunkmeshes[i], hash_table);
                        UploadMesh(chunkmeshes[i]->mesh, false);
                        chunkmeshes[i]->uploaded = true;
                        break;
                    }
                }
            }
        }

        nearby_bounding_box_counter = GetNearbyBlocks(boxes, camera.position, camera.position, hash_table);
        // Vector3 lower_upper_position = (Vector3) { camera.position.x, camera.position.y - 1, camera.position.z };
        // nearby_bounding_box_counter += GetNearbyBlocks(boxes, lower_upper_position, lower_upper_position, hash_table);
        // lower_upper_position = (Vector3) { camera.position.x, camera.position.y + 1, camera.position.z };
        // nearby_bounding_box_counter += GetNearbyBlocks(boxes, lower_upper_position, lower_upper_position, hash_table);

        // constantly check which block the player is looking at
        ray = GetScreenToWorldRay((Vector2) { screenWidth/2, screenHeight / 2 }, camera);
        ray.direction = Vector3Normalize(ray.direction);
        box_counter = 0;
        //run through all nearby boxes real quick
        for (int i = 0; i < nearby_bounding_box_counter; i++) {
            collision = GetRayCollisionBox(ray, boxes[box_counter]);


            if (collision.hit) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    BreakBlock(boxes[box_counter].min, hash_table);
                    break;
                }

                target_box = boxes[box_counter];
            } else {
                //target_box = 0;
            }

            box_counter++;
        }

        //trying to figure out debug lines
        target_offset = (Vector3) { camera.target.x - camera.position.x, camera.target.y - camera.position.y, 
                                    camera.target.z - camera.position.z };
        distance = sqrt((target_offset.x * target_offset.x) + (target_offset.y * target_offset.y) + (target_offset.z * target_offset.z));
        travel = 1 / distance; // to change how far away target is, change n / distance
        target_offset = (Vector3) { camera.position.x + travel * (camera.target.x - camera.position.x),
                                    camera.position.y + travel * (camera.target.y - camera.position.y),
                                    camera.position.z + travel * (camera.target.z - camera.position.z) };

        BeginDrawing();
            
            ClearBackground(SKYBLUE);

            BeginMode3D(camera);
            DrawGrid(20, 1);

                for(int i = 0; i < number_of_chunkmeshes; i++) {
                    if(DoesChunkEntryExist((Vector3){coords[i].x,coords[i].y,coords[i].z},hash_table)) {
                        if(!chunkmeshes[i]->dirty) {
                            DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                        }
                    }
                }
                //DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                //DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                
                for(int i = 0; i < nearby_bounding_box_counter; i++) {
                    DrawBoundingBox(boxes[i], WHITE);
                }
                
                //DrawBoundingBox(target_box, RED);
                // DrawBoundingBox(boxes[2], PURPLE);
                // DrawBoundingBox(boxes[3], PURPLE);
                // DrawBoundingBox(boxes[4], PURPLE);
                // DrawBoundingBox(boxes[5], PURPLE);
                // DrawBoundingBox(boxes[6], PURPLE);
                // DrawBoundingBox(boxes[7], PURPLE);
                // DrawBoundingBox(boxes[8], PURPLE);
                // DrawBoundingBox(boxes[9], PURPLE);

                //DrawBoundingBox(boxes[0], PINK);

                for (int i = 1; i < nearby_bounding_box_counter; i++) {
                    if (boxes[i].min.y > player.position.y) {
                        if (CheckCollisionBoxes(player.bounding_box, boxes[i]) && player.flying == false) {
                            // camera.position.x = player.position.x;
                            // camera.position.z = player.position.z;
                            DrawBoundingBox(boxes[i], BLUE);
                        }
                    }
                }

                //if (collision.hit) DrawCube(collision.point, 0.3f, 0.3f, 0.3f, RED);

                if (debugging) {
                    DrawLine3D(target_offset, (Vector3) { target_offset.x + 0.05f, target_offset.y, target_offset.z }, RED);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y + 0.05f, target_offset.z }, GREEN);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y, target_offset.z + 0.05f }, BLUE);
                }



            EndMode3D();

            
            DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", player.position.x, player.position.y, player.position.z), 
                        140, 10, 20, YELLOW);

            // DrawText(TextFormat("box position x:%.2f, y:%.2f, z:%.2f", box.min.x, box.min.y, box.min.z), 
            //             540, 10, 20, YELLOW);
            // DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
            //                     current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
            //             640, 10, 20, YELLOW);
            //DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            //test_counter++;

            //if(collision.hit) DrawText("hit!", 900, 10, 50, YELLOW);

            // DrawRectangle((screenWidth/2) - 20, (screenHeight / 2), 45, 5, GRAY);
            // DrawRectangle((screenWidth/2), (screenHeight / 2) - 20, 5, 45, GRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    for (int i = 0; i < number_of_chunkmeshes; i++) {
        if(DoesChunkEntryExist((Vector3){coords[i].x,coords[i].y,coords[i].z},hash_table)) {
            UnloadMesh(*chunkmeshes[i]->mesh);
        }
    }

    //UnloadMesh(*chunkmeshes[0]->mesh);

    // for(int i = 0; i < number_of_chunkmeshes; i++) {
    //     free(chunkmeshes[i]->mesh);
    //     free(chunkmeshes[i]->chunk);
    //     free(chunkmeshes[i]);
    // }

    free(coords);
    free(chunkmeshes);
    free(boxes);
    UnloadTexture(texture);
    DestroyTable(hash_table);
    CloseWindow();

    return 0;       
}
