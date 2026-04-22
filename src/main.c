//basic window
#include "raylib.h" 
#include "rlgl.h"
#include "raymath.h"

#include <stdlib.h>
#include <string.h>


#define DIRT_TEX_COORD_U_MIN 0.05f;
#define DIRT_TEX_COORD_U_MAX 0.18f;
#define DIRT_TEX_COORD_V_MIN 0.82f;
#define DIRT_TEX_COORD_V_MAX 0.98f;
#define GRASS_TEX_COORD_U_MIN 0.22f;
#define GRASS_TEX_COORD_U_MAX 0.38f;
#define GRASS_TEX_COORD_V_MIN 0.42f;
#define GRASS_TEX_COORD_V_MAX 0.58f;
#define GRASS_LIGHT_TEX_COORD_U_MIN 0.42f;
#define GRASS_LIGHT_TEX_COORD_U_MAX 0.58f;
#define GRASS_LIGHT_TEX_COORD_V_MIN 0.42f;
#define GRASS_LIGHT_TEX_COORD_V_MAX 0.58f;
#define LAVA_TEX_COORD_U_MIN 0.82f;
#define LAVA_TEX_COORD_U_MAX 0.98f;
#define LAVA_TEX_COORD_V_MIN 0.42f;
#define LAVA_TEX_COORD_V_MAX 0.58f;

#define MAGMA_TEX_COORD_U_MIN 0.42f;
#define MAGMA_TEX_COORD_U_MAX 0.58f;
#define MAGMA_TEX_COORD_V_MIN 0.82f;
#define MAGMA_TEX_COORD_V_MAX 0.98f;


/*
*   TO DO
*   - Get texture atlas to work on one block
*   - Get meshing to work for entire chunk
*   - Atlas working for entire chunk
*   - Then chunking system
*/

void draw_cube_basic(Vector3 position, Color color, Texture* texture);
Mesh gen_cube_mesh();

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

    //Texture texture = LoadTexture("assets/dirt.png");
    Texture texture = LoadTexture("assets/tex_atlas_2.png");
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Texture could not be loaded! Check path or format.");
    }
    //texture.id = 1;
    Mesh cube = gen_cube_mesh();
    UploadMesh(&cube, false);

    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);

        

        BeginDrawing();
            DrawFPS(10, 10);
            ClearBackground(DARKGRAY);
            //DrawTexture(texture, 0.0f, 0.0f, WHITE);
            BeginMode3D(camera);

                //DrawCube( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, RAYWHITE);
                //DrawCubeWires( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, BLACK);

                //draw_cube_basic((Vector3) { 0.0f, 1.0f, 0.0f }, WHITE, &texture);
                //draw_cube_basic((Vector3) { 1.0f, 0.0f, 0.0f }, WHITE, &texture);
                //draw_cube_basic((Vector3) { -2.0f, 1.0f, 0.0f }, WHITE, &texture);

                DrawMesh(cube, material, matrix);

            EndMode3D();
            

        EndDrawing();
    }

    UnloadMesh(cube);
    UnloadTexture(texture);
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

Mesh gen_cube_mesh() {
    Mesh mesh = {0};

    float size = 0.5f;
    float zero = 0.0f;
    float one = 1.0f;

    float vertices[] = {
        -size, -size, size,
        size, -size, size,
        size, size, size,
        -size, size, size,

        size, -size, -size,
        -size, -size, -size,
        -size, size, -size,
        size, size, -size,

        -size, size, size,
        size, size, size,
        size, size, -size,
        -size, size, -size,

        size, -size, size,
        -size, -size, size,
        -size, -size, -size,
        size, -size, -size,

        -size, -size, -size,
        -size, -size, size,
        -size, size, size,
        -size, size, -size,

        size, -size, size,
        size, -size, -size,
        size, size, -size,
        size, size, size,
    };

    // float u_min = 5.0f/16.0f;
    // float u_max = 6.0f/16.0f;
    // float v_min = 4.0f/16.0f;
    // float v_max = 5.0f/16.0f;
    // float u_min = GRASS_LIGHT_TEX_COORD_U_MIN;
    // float u_max = GRASS_LIGHT_TEX_COORD_U_MAX;
    // float v_min = GRASS_LIGHT_TEX_COORD_V_MIN;
    // float v_max = GRASS_LIGHT_TEX_COORD_V_MAX;
    // float u_min = LAVA_TEX_COORD_U_MIN;
    // float u_max = LAVA_TEX_COORD_U_MAX;
    // float v_min = LAVA_TEX_COORD_V_MIN;
    // float v_max = LAVA_TEX_COORD_V_MAX;
    float u_min = MAGMA_TEX_COORD_U_MIN;
    float u_max = MAGMA_TEX_COORD_U_MAX;
    float v_min = MAGMA_TEX_COORD_V_MIN;
    float v_max = MAGMA_TEX_COORD_V_MAX;

    float texcoords[] = {
        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,

        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,

        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,

        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,

        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,

        u_min, v_max,
        u_max, v_max,
        u_max, v_min,
        u_min, v_min,
    };

    // float texcoords[] = {
    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,

    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,

    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,

    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,

    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,

    //     zero, one,
    //     one, one,
    //     one, zero,
    //     zero, zero,
    // };

    // float normals[] = {
    //     zero, zero, one,
    //     zero, zero, -one,
    //     zero, one, zero,
    //     zero, one, zero,//not neg one?
    //     zero, zero, one,
    //     zero, zero, one, //this does not look right
    // };

    float normals[] = { //yoinked from raylib
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    mesh.vertices = (float *)malloc(24*3*sizeof(float));
    memcpy(mesh.vertices, vertices, 24*3*sizeof(float));

    mesh.texcoords = (float *)malloc(24*2*sizeof(float));
    memcpy(mesh.texcoords, texcoords, 24*2*sizeof(float));

    mesh.normals = (float *)malloc(24*3*sizeof(float));
    memcpy(mesh.normals, normals, 24*3*sizeof(float));

    mesh.indices = (unsigned short *)RL_MALLOC(36*sizeof(unsigned short));

    int k = 0;

    // Indices can be initialized right now
    for (int i = 0; i < 36; i += 6)
    {
        mesh.indices[i] = 4*k;
        mesh.indices[i + 1] = 4*k + 1;
        mesh.indices[i + 2] = 4*k + 2;
        mesh.indices[i + 3] = 4*k;
        mesh.indices[i + 4] = 4*k + 2;
        mesh.indices[i + 5] = 4*k + 3;

        k++;
    }

    mesh.vertexCount = 24;
    mesh.triangleCount = 12;

    return mesh;
}