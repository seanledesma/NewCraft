#include "include.h"

// TO DO NEXT
// better collisions between player / nearby blocks, prevent wall clipping / getting stuck in walls
// more performant terrain generation
// take a look at world gen up and down y axis (why does everything dissapear when i go up)
// realllyy need to lower memory consumption
// put some clouds in the sky
// caves!
// three threads or more for each y axis terrain gen?
// seperate thread for rebuilding current chunk on block break?
// ideas for lowering memory usage: do not allocate mem for all-air chunks, put two textures in one byte, frustrum culling,
//                                  deallocating memory when player leaves chunks visibility

int main(void) {
    //SetTraceLogLevel(LOG_DEBUG);
    bool debugging = false;

    bool all_chunkmeshes_generated = false;

    pthread_t thread_1, thread_2;

    int screenWidth = 1920;
    int screenHeight = 1080;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, PLAYER_HEIGHT, 0.0f };
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
    
    ChunkMesh** chunkmeshes = (ChunkMesh**)MemAlloc(hash_table->capacity * sizeof(ChunkMesh));

    int depth = 10;
    Vector3* coords = (Vector3*)MemAlloc(hash_table->capacity * sizeof(Vector3));
    int total_coords = 0;
    total_coords = SpiralTraversal2DChunks(coords, total_coords, starting_position, depth);
    float timer = 0.0f;

    total_coords = SpiralTraversal2DChunks(coords, total_coords, 
        (Vector3) {
            starting_position.x,
            starting_position.y + CHUNK_SIZE,
            starting_position.z
        }, depth);

    total_coords = SpiralTraversal2DChunks(coords, total_coords, 
        (Vector3) {
            starting_position.x,
            starting_position.y - CHUNK_SIZE,
            starting_position.z
        }, depth);

    int number_of_chunkmeshes = total_coords;

    Vector3* new_coords = (Vector3*)MemAlloc(hash_table->capacity * sizeof(Vector3));
    int new_total_coords = 0;

    for(int i = 0; i < total_coords; i++) {
        chunkmeshes[i] = FetchChunkEntry((Vector3) { 
            coords[i].x,
            coords[i].y,
            coords[i].z
        }, hash_table);
    }

    //create all chunks closet to player
    // for (int i = 0; i < number_of_chunkmeshes; i++) {
    //     chunkmeshes[i] = FetchChunkEntry((Vector3) { 
    //         coords[i].x,
    //         coords[i].y,
    //         coords[i].z
    //      }, hash_table);
    // }

    //then create all meshes
    // for (int i = 0; i < number_of_chunkmeshes; i++) {
    //     GenMeshChunkRework(chunkmeshes[i], hash_table);
    //     chunkmeshes[i]->uploaded = true;
    //     UploadMesh(chunkmeshes[i]->mesh, false);
    // }

    //Vector3 current_chunk_pos = {-1000.0f, -1000.0f, -1000.0f};
    Vector3 current_chunk_pos = DeriveChunkPosition(player.position);

    //chunkmeshes[0] = FetchChunkEntry(starting_position, hash_table);
    // GenMeshChunk(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    // GenMeshChunkRework(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    // UploadMesh(chunkmeshes[0]->mesh, false);

    Ray ray = {0};
    RayCollision collision = {0};
    RayCollision collision_test = {0};
    bool hit = false;
    // make boxes as big as it may ever possibly get
    BoundingBox* boxes = (BoundingBox*)MemAlloc(sizeof(BoundingBox) * MAX_NEARBY_BOXES + 1);
    int nearby_bounding_box_counter = 0;
    BoundingBox target_box = {0};
    BoundingBox target_box_test = {0};

    DisableCursor();
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        if (IsWindowResized()) {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
        }
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

            new_total_coords = 0;
            new_total_coords = SpiralTraversal2DChunks(new_coords, new_total_coords, current_chunk_pos, depth);
            new_total_coords = SpiralTraversal2DChunks(new_coords, new_total_coords, 
                (Vector3) {
                    current_chunk_pos.x,
                    current_chunk_pos.y + CHUNK_SIZE,
                    current_chunk_pos.z
                }, depth);

            new_total_coords = SpiralTraversal2DChunks(new_coords, new_total_coords, 
                (Vector3) {
                    current_chunk_pos.x,
                    current_chunk_pos.y - CHUNK_SIZE,
                    current_chunk_pos.z
                }, depth);
            bool does_chunkmesh_exist_in_new_arr = false;
            for(int i = 0; i < total_coords; i++) { //could have issue with newtotalcoords != totalcoords here
                does_chunkmesh_exist_in_new_arr = false;
                for(int j = 0; j < new_total_coords; j++) {
                    if(coords[i].x == new_coords[j].x &&
                        coords[i].y == new_coords[j].y && 
                        coords[i].z == new_coords[j].z) {
                            does_chunkmesh_exist_in_new_arr = true;
                            continue;
                            //chunkmeshes[i].
                    }

                    // if(coords[i].x == chunkmeshes[j]->id) {
                    //     does_chunkmesh_exist_in_new_arr = true;
                    //     TraceLog(LOG_WARNING, TextFormat("found id %d", chunkmeshes[j]->id));
                    //     continue;
                    // }
                    //does_chunkmesh_exist_in_new_arr = false;
                }

                if(does_chunkmesh_exist_in_new_arr == false) {
                    RemoveChunkEntry(coords[i], hash_table);
                    TraceLog(LOG_WARNING, TextFormat("removing chunkmesh %d", i));
                }
            }
            
            number_of_chunkmeshes = new_total_coords;
            total_coords = new_total_coords;

            // is this fast enough i can just run through it real quick with no noticeable penalty?
            for (int i = 0; i < number_of_chunkmeshes; i++) {
                // copy over new coords
                coords[i] = new_coords[i];

                // if the entry exists, we must suppose the mesh has been created... need to track that
                chunkmeshes[i] = FetchChunkEntry((Vector3) { 
                    coords[i].x,
                    coords[i].y,
                    coords[i].z
                }, hash_table);
            }

        }

        for (int i = 0; i < number_of_chunkmeshes; i++) {
            if(chunkmeshes[i]->dirty && !chunkmeshes[i]->generating) {
                ThreadStruct* thread_struct = (ThreadStruct*)MemAlloc(sizeof(ThreadStruct));
                thread_struct->chunkmesh = chunkmeshes[i];
                thread_struct->hashtable = hash_table;
                pthread_create(&thread_1, NULL, GenMeshChunkReworkVoid, thread_struct);
                break;
            }
            // if we get here then theoretically all chunkmeshes have been created
            // if (i == number_of_chunkmeshes - 1) {
            //     all_chunkmeshes_generated = true;
            //     //pthread_join(thread_1, NULL);
            // }
        }

        //maybe we seperate uploading mesh as well?
        for (int i = 0; i < number_of_chunkmeshes; i++) {
            if(!chunkmeshes[i]->uploaded && !chunkmeshes[i]->dirty && !chunkmeshes[i]->generating) {
                UploadMesh(chunkmeshes[i]->mesh, false);
                chunkmeshes[i]->uploaded = true;
                break;
            }
        }

        // if(chunkmeshes[total_coords-1]->uploaded) {
        //     all_chunkmeshes_generated = true;
        // }


        // if(all_chunkmeshes_generated) {
        //     for (int i = 0; i < number_of_chunkmeshes; i++) {
        //         if(DoesChunkEntryExist((Vector3){coords[i].x,coords[i].y,coords[i].z},hash_table)) {
        //             if(chunkmeshes[i]->generating || !chunkmeshes[i]->uploaded) {
        //                 all_chunkmeshes_generated = false;
        //                 break;
        //             }
        //             chunkmeshes[i]->uploaded = false;
        //             chunkmeshes[i]->dirty = true;
        //         }

        //     }
        // }

        // every once in a while, run through already loaded chunks and reload them to reduce faces drawn
        // this whole thing is stupid
        // timer += GetFrameTime();
        // if(timer >= 1.0f) {
        //     timer = 0.0f;
        //     for (int i = 0; i < number_of_chunkmeshes; i++) {
        //         if(DoesChunkEntryExist((Vector3) { coords[i].x, coords[i].y, coords[i].z }, hash_table)) {
        //             // if there is a single dirty chunk, don't do anything yet
        //             if(chunkmeshes[i]->dirty) break;
        //             if(i == number_of_chunkmeshes-1) {
        //                 for (int j = 0; j < number_of_chunkmeshes; j++) {
        //                     chunkmeshes[j]->dirty = true;
        //                 }
        //             }
        //         }
                
        //     }
        // }

        nearby_bounding_box_counter = GetNearbyBlocks(boxes, camera.position, camera.position, hash_table);

        // constantly check which block the player is looking at
        ray = GetScreenToWorldRay((Vector2) { screenWidth/2, screenHeight / 2 }, camera);
        ray.direction = Vector3Normalize(ray.direction);
        box_counter = 0;
        //run through all nearby boxes real quick
        //must make sure we only keep the closest box that was hit
        hit = false;
        collision_test.distance = 100.0f;
        for (int i = 0; i < nearby_bounding_box_counter-1; i++) {
            collision = GetRayCollisionBox(ray, boxes[box_counter]);
            // need to save collision every time it hits, then only save the shortest distance collision
            if (collision.hit) {
                hit = true;
                if (collision.distance < collision_test.distance) {
                    collision_test = collision;
                    target_box = boxes[box_counter];
                }
            }
            box_counter++;
        }
        collision = collision_test;

        if (hit) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                //BreakBlock(boxes[box_counter].min, hash_table);
                BreakBlock(target_box.min, hash_table);
                collision_test = collision;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                //PlaceBlock(collision.point, player.block_type, hash_table);
                PlaceBlock(&collision, player.block_type, hash_table);
                // TraceLog(LOG_WARNING, TextFormat("collision normal x: %.2f, y: %.2f, z: %.2f", collision.normal.x, collision.normal.y, collision.normal.z));
                // TraceLog(LOG_WARNING, TextFormat("ray direction x: %.2f, y: %.2f, z: %.2f", ray.direction.x, ray.direction.y, ray.direction.z));
                // TraceLog(LOG_WARNING, TextFormat("collision distance: %.2f", collision.distance));
                //collision.distance
                collision_test = collision;
            }
            //collision_test = collision;
            //target_box = boxes[box_counter];
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
            //DrawGrid(20, 1);

                for(int i = 0; i < number_of_chunkmeshes; i++) {
                    if(DoesChunkEntryExist((Vector3){coords[i].x,coords[i].y,coords[i].z},hash_table)) {
                        if(!chunkmeshes[i]->dirty && chunkmeshes[i]->uploaded) {
                            DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                        }
                    }
                }
                //DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                //DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                if (debugging) {
                    for(int i = 0; i < nearby_bounding_box_counter-1; i++) {
                        DrawBoundingBox(boxes[i], WHITE);
                    }
                }
                
                
                DrawBoundingBox(target_box, WHITE);
                // DrawBoundingBox(boxes[2], PURPLE);
                // DrawBoundingBox(boxes[3], PURPLE);
                // DrawBoundingBox(boxes[4], PURPLE);
                // DrawBoundingBox(boxes[5], PURPLE);
                // DrawBoundingBox(boxes[6], PURPLE);
                // DrawBoundingBox(boxes[7], PURPLE);
                // DrawBoundingBox(boxes[8], PURPLE);
                // DrawBoundingBox(boxes[9], PURPLE);

                //DrawBoundingBox(boxes[0], PINK);

                for (int i = 1; i < nearby_bounding_box_counter-1; i++) {
                    if (boxes[i].min.y > player.position.y) {
                        if (CheckCollisionBoxes(player.bounding_box, boxes[i]) && player.flying == false) {
                            // camera.position.x = player.position.x;
                            // camera.position.z = player.position.z;
                            DrawBoundingBox(boxes[i], BLUE);
                        }
                    }
                }

                //if (collision_test.hit) DrawCube(collision_test.point, 0.1f, 10.0f, 0.1f, RED);

                if (debugging) {
                    DrawLine3D(target_offset, (Vector3) { target_offset.x + 0.05f, target_offset.y, target_offset.z }, RED);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y + 0.05f, target_offset.z }, GREEN);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y, target_offset.z + 0.05f }, BLUE);
                }


            EndMode3D();

            if (debugging) {
                DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", player.position.x, player.position.y, player.position.z), 
                        140, 10, 20, YELLOW);
            }
            

            // DrawText(TextFormat("box position x:%.2f, y:%.2f, z:%.2f", box.min.x, box.min.y, box.min.z), 
            //             540, 10, 20, YELLOW);
            // DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
            //                     current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
            //             640, 10, 20, YELLOW);
            //DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            //test_counter++;

            //if(collision.hit) DrawText("hit!", 900, 10, 50, YELLOW);

            if (debugging == false) {
                DrawRectangle((screenWidth/2) - 20, (screenHeight / 2), 45, 5, GRAY);
                DrawRectangle((screenWidth/2), (screenHeight / 2) - 20, 5, 45, GRAY);
            }

            DrawFPS(10, 10);
        EndDrawing();
        //debugging
        if (IsKeyPressed(KEY_GRAVE)) debugging = !debugging;
    }

    for (int i = 0; i < number_of_chunkmeshes; i++) {
        if(DoesChunkEntryExist((Vector3){coords[i].x,coords[i].y,coords[i].z},hash_table)) {
            if(chunkmeshes[i]->uploaded) {
                UnloadMesh(*chunkmeshes[i]->mesh);
            }
        }
    }

    //UnloadMesh(*chunkmeshes[0]->mesh);

    // for(int i = 0; i < number_of_chunkmeshes; i++) {
    //     free(chunkmeshes[i]->mesh);
    //     free(chunkmeshes[i]->chunk);
    //     free(chunkmeshes[i]);
    // }

    pthread_cancel(thread_1);   // to stop use after free errors?
    free(coords);
    free(new_coords);
    free(chunkmeshes);
    free(boxes);
    UnloadTexture(texture);
    DestroyTable(hash_table);

    CloseWindow();

    return 0;       
}
