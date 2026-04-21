//basic window
#include "raylib.h" 
#include "rlgl.h"

void draw_cube_basic(Vector3 position, Color color, Texture* texture);

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, 1.8f, 4.0f };
    camera.target = (Vector3) { 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    Texture texture = LoadTexture("assets/dirt.png");
    //texture.id = 1;

    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);

                //DrawCube( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, RAYWHITE);
                //DrawCubeWires( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, BLACK);
                draw_cube_basic((Vector3) { 0.0f, 3.0f, 0.0f }, WHITE, &texture);

            EndMode3D();
            

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;       
}

void draw_cube_basic(Vector3 pos, Color color, Texture* texture) {
    float x = 0.0f;
    float y = 0.0f; 
    float z = 0.0f;

    rlPushMatrix();

        rlSetTexture(texture->id);
         rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            // for whatever reason, RL_QUADS goes bottom left, bottom right, top right, top left... counter clockwise

            // Z-POSITIVE face
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x-1.0f, pos.y-1.0f, pos.z+1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x+1.0f, pos.y-1.0f, pos.z+1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(pos.x+1.0f, pos.y+1.0f, pos.z+1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(pos.x-1.0f, pos.z+1.0f, pos.z+1.0f);
            
            // Z-NEGATIVE FACE
            rlNormal3f(0.0f, 0.0f, -1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(pos.x+1.0f, pos.y-1.0f, pos.z-1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(pos.x-1.0f, -1.0f, -1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(-1.0f, 1.0f, -1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(1.0f, 1.0f, -1.0f);


            // Y-POSITIVE FACE (TOP)
            rlNormal3f(0.0f, 1.0f, 0.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-1.0f, 1.0f, 1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(1.0f, 1.0f, 1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(1.0f, 1.0f, -1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(-1.0f, 1.0f, -1.0f);


            // Y-NEGATIVE (BOTTOM FACE)
            rlNormal3f(0.0f, 1.0f, 0.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(1.0f, -1.0f, 1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-1.0f, -1.0f, 1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(-1.0f, -1.0f, -1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(1.0f, -1.0f, -1.0f);
            

            // X-NEGATIVE FACE (LEFT)
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-1.0f, -1.0f, -1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-1.0f, -1.0f, 1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(-1.0f, 1.0f, 1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(-1.0f, 1.0f, -1.0f);
            

            // X-POSITIVE FACE (RIGHT)
            rlNormal3f(0.0f, 0.0f, 1.0f);
            // Vertex 2: Bottom left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(1.0f, -1.0f, 1.0f);

            // Vertex 3: Bottom right
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(1.0f, -1.0f, -1.0f);

            // Vertex 3: Top right
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(1.0f, 1.0f, -1.0f);            

            // Vertex 1: Top left
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(1.0f, 1.0f, 1.0f);

            // rlNormal3f(0.0f, 0.0f, 1.0f);
            // rlTexCoord2f(0.5f, 0.0f);
            // rlVertex3f(x - width/2, y - height/2, z + length/2);
            // rlTexCoord2f(0.0f, 1.0f);
            // rlVertex3f(x + width/2, y - height/2, z + length/2);
            // rlTexCoord2f(1.0f, 1.0f);
            // rlVertex3f(x - width/2, y + height/2, z + length/2);
            
            // rlVertex3f(x + width/2, y - height/2, z + length/2);
            // rlVertex3f(x + width/2, y + height/2, z + length/2);
            // rlVertex3f(x - width/2, y + height/2, z + length/2);

            // rlNormal3f(0.0f, 0.0f, -1.0f);
            // rlVertex3f(x - width/2, y - height/2, z - length/2);
            // rlVertex3f(x - width/2, y + height/2, z - length/2);
            // rlVertex3f(x + width/2, y - height/2, z - length/2);

            // rlVertex3f(x + width/2, y + height/2, z - length/2);
            // rlVertex3f(x + width/2, y - height/2, z - length/2);
            // rlVertex3f(x - width/2, y + height/2, z - length/2);


        rlEnd();

        rlSetTexture(0);
    rlPopMatrix();
}