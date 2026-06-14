#include "include.h"

//fnl_state noise;


void draw_cube_basic(Vector3 position, Color color, Texture* texture);


// TO DO NEXT
// load (and unload) chunks as the player moves around based on player position 
// i'd like to have an initial array of uploaded chunks when player loads in, 1 in every direction
// then as the player goes about, we (slowly) add in mroe chunks
// i do NOT want to load chunks when player steps over a certain boundary
/*
*   thinking out loud.. the "current chunk" will be updating as the player traverses the map. I use SpiralTraversal2D
*   to get all the chunk coords arround the player. Those coords aren't intrinsically tied to anything, it's okay
*   if I delete them and get a new set. From the coords I can fetch all the chunk meshes. The actual problem is what
*   to do with my chunkmeshes array. I use my array of coords to gen all my chunkmeshes in sequential order. Once a player
*   moves to another chunk, base chunk is no longer chunkmeshes[0]. In fact, when we go to a new chunk, we will eventually also
*   have to remove some chunks that are too far away and gen new chunks that are within sight. 
*   I could do something tricky with making a new array and copying all the old elements over in the right order. 
*   I could just assume that I'll need a whole new array every time the player changes chunks, I don't think it's too costly
*   to make a new array. The only real costly part is the chunkmesh generation.
something like:
if (player gets to new chunk) {
    new_coords* = spiralTraversal2D();
    for i in coords:
        chunkmeshes[i] = fetchchunkentry    // this will create new chunks if they don't exist. Old ones will stay in hashmap
    //then i'll want to gen new meshes / free old meshes
    //i won't draw them because they aren't in new_coords, but I'm thinking of having a second
    //array of chunkmeshes on the outskirts, not immediate to the player, that I slowly load / deload
    //for the outer array, i just need to request greater depth from spiraltraversal and then use my 
    //previous coords_counter as the starting position for my outer_coords
}
*/

int main(void) {

    //SetTraceLogLevel(LOG_DEBUG);
    float debugging = true;

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
    //ChunkMesh* chunkmeshes = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    //chunkmeshes[0] = current_chunk;
    int chunkcounter = 1;
    int test_counter = 0;
    int renderX = 1;
    int renderY = 1;
    int renderZ = 1;


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
    Vector3* inner_coords = (Vector3*)MemAlloc(1000 * sizeof(Vector3));
    Vector3 starting_position = (Vector3) { 0.0f, 0.0f, 0.0f };
    int depth = 2;
    int count = 0;
    int coords_counter = 0;
    // coords_counter = SpiralTraversal2D(inner_coords, coords_counter, starting_position, depth);
    // // coords_counter = SpiralTraversal2D(inner_coords, coords_counter, 
    // //     (Vector3) {
    // //         starting_position.x,
    // //         starting_position.y + 1,
    // //         starting_position.z
    // //     }, depth);

    // // coords_counter = SpiralTraversal2D(inner_coords, coords_counter, 
    // //     (Vector3) {
    // //         starting_position.x,
    // //         starting_position.y - 1,
    // //         starting_position.z
    // //     }, depth);


    // //create all chunks closet to player
    // for (int i = 0; i < coords_counter; i++) {
    //     chunkmeshes[i] = FetchChunkEntry((Vector3) { 
    //         inner_coords[i].x * CHUNK_SIZE,
    //         inner_coords[i].y * CHUNK_SIZE,
    //         inner_coords[i].z * CHUNK_SIZE
    //      }, hash_table);
    //      count++;
    // }

    // //then create all meshes
    // for (int i = 0; i < coords_counter; i++) {
    //     GenMeshChunkRework(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
    //     //GenMeshChunk(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
    //     UploadMesh(chunkmeshes[i]->mesh, false);
    // }

    Vector3 current_chunk_pos = starting_position;

    chunkmeshes[0] = FetchChunkEntry(relative_positions[0], hash_table);
    // GenMeshChunk(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    GenMeshChunkRework(chunkmeshes[0]->mesh, chunkmeshes[0]->chunk, hash_table);
    UploadMesh(chunkmeshes[0]->mesh, false);

    ChunkMesh* newchunkmeshes = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
    newchunkmeshes[0] = *chunkmeshes[0];

    //Model model = LoadModelFromMesh(*chunkmeshes[0]->mesh);
    Ray ray = {0};
    RayCollision collision = {0};
    // make boxes as big as it may ever possibly get
    BoundingBox* boxes = (BoundingBox*)MemAlloc(sizeof(BoundingBox) * depth*depth*5);
    int nearby_bounding_box_counter = 0;
    BoundingBox target_box = {0};

    DisableCursor();
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);
        UpdatePlayer(&player, &camera, boxes);
        TraceLog(LOG_DEBUG, TextFormat("0starting pos in main y: %.2f", camera.position.y));
        //nearby_bounding_box_counter = GetNearbyBlocks(boxes, camera.position, camera.position, hash_table);
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
        
        Vector3 temp_chunk_pos = DeriveChunkPosition(camera.position, hash_table);
        //TraceLog(LOG_WARNING, TextFormat("temp chunk x: %.2f", temp_chunk_pos.x));
        if ( temp_chunk_pos.x != current_chunk_pos.x 
            || temp_chunk_pos.y != current_chunk_pos.y 
            || temp_chunk_pos.z != current_chunk_pos.z ) 
        {
            TraceLog(LOG_WARNING, "do we ever hit here");
            free(inner_coords);
            free(newchunkmeshes);
            inner_coords = (Vector3*)MemAlloc(sizeof(Vector3));
            newchunkmeshes = (ChunkMesh*)MemAlloc(sizeof(ChunkMesh));
            current_chunk_pos = temp_chunk_pos;
            int depth = 2;
            coords_counter = SpiralTraversal2D(inner_coords, coords_counter, current_chunk_pos, depth);

            for (int i = 0; i < coords_counter; i++) {
                newchunkmeshes[i] = *FetchChunkEntry((Vector3) {
                    inner_coords[i].x * CHUNK_SIZE,
                    inner_coords[i].y * CHUNK_SIZE, 
                    inner_coords[i].z * CHUNK_SIZE
                }, hash_table);
            }
            //then check for new meshes (subject to change pending outer_coords)
            for (int i = 0; i < coords_counter; i++) {
                if (newchunkmeshes[i].new) {
                    newchunkmeshes[i].new = false;
                    GenMeshChunkRework(newchunkmeshes[i].mesh, newchunkmeshes[i].chunk, hash_table);
                    UploadMesh(newchunkmeshes[i].mesh, false);
                }
            }
            
        }



        // check for dirty chunks to re-make
        for (int i = 0; i < coords_counter; i++) {
            if(chunkmeshes[i]->dirty == true) {

                chunkmeshes[i]->dirty = false;

                UnloadMesh(*chunkmeshes[i]->mesh);
                //do i need to free mesh or will that cause issues?
                MemFree(chunkmeshes[i]->mesh);
                Mesh* mesh = (Mesh*)calloc(1,sizeof(Mesh));
                
                int num_blocks_in_chunk = CHUNK_CUBED;

                int num_block_vertices = 36 * 3;
                int num_block_texcoords = 36 * 2;
                int num_block_normals = 36 * 3;

                int num_chunk_vertices = num_block_vertices * num_blocks_in_chunk;
                int num_chunk_texcoords = num_block_texcoords * num_blocks_in_chunk;
                int num_chunk_normals = num_block_normals * num_blocks_in_chunk;
                mesh->vertices = (float *)MemAlloc(num_chunk_vertices * sizeof(float));
    
                mesh->texcoords = (float *)MemAlloc(num_chunk_texcoords * sizeof(float));
            
                mesh->normals = (float *)MemAlloc(num_chunk_normals * sizeof(float));

                mesh->vertexCount = 0;
                mesh->triangleCount = 0;


                chunkmeshes[i]->mesh = mesh;
                GenMeshChunkRework(chunkmeshes[i]->mesh, chunkmeshes[i]->chunk, hash_table);
                UploadMesh(chunkmeshes[i]->mesh, false);
            }
        }

        //TraceLog(LOG_WARNING, TextFormat("IN MAIN LOOP what is block type under player: %d", chunkmeshes[0]->chunk->blocks[8][8][8].block_type));

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

                // for(int i = 0; i < coords_counter; i++) {
                //     DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                // }
                DrawMesh(*newchunkmeshes[0].mesh, material, matrix);

                //DrawMesh(*chunkmeshes[0]->mesh, material, matrix);

                
                // for(int i = 0; i < nearby_bounding_box_counter; i++) {
                //     DrawBoundingBox(boxes[i], ORANGE);
                // }
                
                DrawBoundingBox(target_box, BLACK);
                // DrawBoundingBox(boxes[2], PURPLE);
                // DrawBoundingBox(boxes[3], PURPLE);
                // DrawBoundingBox(boxes[4], PURPLE);
                // DrawBoundingBox(boxes[5], PURPLE);
                // DrawBoundingBox(boxes[6], PURPLE);
                // DrawBoundingBox(boxes[7], PURPLE);
                // DrawBoundingBox(boxes[8], PURPLE);
                // DrawBoundingBox(boxes[9], PURPLE);

                DrawBoundingBox(boxes[0], PINK);

                if (collision.hit) DrawCube(collision.point, 0.3f, 0.3f, 0.3f, RED);

                if (debugging) {
                    DrawLine3D(target_offset, (Vector3) { target_offset.x + 0.05f, target_offset.y, target_offset.z }, RED);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y + 0.05f, target_offset.z }, GREEN);
                    DrawLine3D(target_offset, (Vector3) { target_offset.x, target_offset.y, target_offset.z + 0.05f }, BLUE);
                }



            EndMode3D();

            
            DrawText(TextFormat("Player position x:%.2f, y:%.2f, z:%.2f", camera.position.x, camera.position.y-PLAYER_HEIGHT, camera.position.z), 
                        140, 10, 20, YELLOW);

            // DrawText(TextFormat("box position x:%.2f, y:%.2f, z:%.2f", box.min.x, box.min.y, box.min.z), 
            //             540, 10, 20, YELLOW);
            // DrawText(TextFormat("Current Chunk Position x:%.2f, y:%.2f, z:%.2f", 
            //                     current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z), 
            //             640, 10, 20, YELLOW);
            //DrawText(TextFormat("counter: %d", test_counter), 10, 50, 12, YELLOW);
            //test_counter++;

            if(collision.hit) DrawText("hit!", 900, 10, 50, YELLOW);

            // DrawRectangle((screenWidth/2) - 20, (screenHeight / 2), 45, 5, GRAY);
            // DrawRectangle((screenWidth/2), (screenHeight / 2) - 20, 5, 45, GRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    for (int i = 0; i < count; i++) {
        UnloadMesh(*chunkmeshes[i]->mesh);
    }

    //UnloadMesh(*chunkmeshes[0]->mesh);

    //UnloadModel(model);

    UnloadTexture(texture);
    //free(chunkmeshes);
    //free(megachunks);
    DestroyTable(hash_table);
    CloseWindow();

    return 0;       
}
