#include "include.h"


void draw_cube_basic(Vector3 position, Color color, Texture* texture);


int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, 1.8f + 8.0f, 20.0f };
    camera.target = (Vector3) { 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Texture texture = LoadTexture("assets/tex_atlas_2.png");

    HashTable* hash_table = InitializeTable(100000);
    //ChunkMesh* chunkmesh = CreateChunkEntry((Vector3) { 0.0f, 0.0f, 0.0f }, hash_table);
    //ChunkMesh* chunkmesh = CreateChunkEntry(camera.position, hash_table);
    //ChunkMesh* chunkmesh2 = CreateChunkEntry((Vector3) { 50.0f, 0.0f, 0.0f }, hash_table);

    //ChunkMesh* chunkmesh = gen_chunk_mesh((Vector3) { 0.0f, 0.0f, 0.0f });
    //ChunkMesh* chunkmesh = gen_chunk_mesh((Vector3) { 0.0f, 0.0f, 0.0f });

    //ChunkMesh* chunkmeshes = (ChunkMesh*)calloc(16, sizeof(ChunkMesh));
    Vector3 positions[9] = {
        (Vector3) {-16.0f, 0.0f, -16.0f},
        (Vector3) {-16.0f, 0.0f, 0.0f},
        (Vector3) {-16.0f, 0.0f, 16.0f},

        (Vector3) {0.0f, 0.0f, -16.0f},
        (Vector3) {0.0f, 0.0f, 0.0f},
        (Vector3) {0.0f, 0.0f, 16.0f},

        (Vector3) {16.0f, 0.0f, -16.0f},
        (Vector3) {16.0f, 0.0f, 0.0f},
        (Vector3) {16.0f, 0.0f, 16.0f},
    }; 
    // for(int i = 0; i < 3; i++) {
    //     for (int j = 0; j < 3; j++) {
    //         //positions[i] = (Vector3) { (CHUNK_SIZE * i) / 2, 0.0f, (CHUNK_SIZE * j) / 2 };
    //     }
    // }

    ChunkMesh* chunkmeshes[9];
    for(int i = 0; i < 9; i++) {
        chunkmeshes[i] = CreateChunkEntry(positions[i], hash_table);
        UploadMesh(chunkmeshes[i]->mesh, false); 
    }


    //UploadMesh(chunkmesh->mesh, false);
    //UploadMesh(chunkmesh2->mesh, false);

    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);



    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);

        

        BeginDrawing();
            
            ClearBackground(LIGHTGRAY);
            BeginMode3D(camera);

                //DrawMesh(*(chunkmesh->mesh), material, matrix);
                //DrawMesh(*(chunkmesh2->mesh), material, matrix);
                for(int i = 0; i < 9; i++) {
                    DrawMesh(*chunkmeshes[i]->mesh, material, matrix);
                }

            EndMode3D();
            
            DrawFPS(10, 10);
        EndDrawing();
    }

    //UnloadMesh(*(chunkmesh->mesh));
    //UnloadMesh(*(chunkmesh2->mesh));
    for(int i = 0; i < 9; i++) {
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