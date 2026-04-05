//basic window
#include "raylib.h" 
#include "rlgl.h"

void DrawTriangleBasic(Vector3 position, float width, float height, float length, Color color);

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




    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);

                //DrawCube( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, RAYWHITE);
                //DrawCubeWires( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, BLACK);
                DrawTriangleBasic((Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, WHITE);

            EndMode3D();
            

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}

void DrawTriangleBasic(Vector3 position, float width, float height, float length, Color color) {
    float x = 0.0f;
    float y = 0.0f; 
    float z = 0.0f;

    rlPushMatrix();


        rlBegin(RL_TRIANGLES);
            rlColor4ub(color.r, color.g, color.b, color.a);

            rlNormal3f(0.0f, 0.0f, 1.0f);
            rlVertex3f(x - width/2, y - height/2, z + length/2);
            rlVertex3f(x + width/2, y - height/2, z + length/2);
            rlVertex3f(x - width/2, y + height/2, z + length/2);


        rlEnd();
    rlPopMatrix();
}