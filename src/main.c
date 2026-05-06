#include "include.h"


void draw_cube_basic(Vector3 position, Color color, Texture* texture);
Mesh gen_block_mesh(int x, int y, int z);
//Mesh gen_chunk_mesh(Chunk* chunk);
//Chunk gen_chunk();
Mesh do_it_all(void);


int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "NewCraft");

    Camera camera = { 0 };
    camera.position = (Vector3) { 0.0f, 1.8f, 20.0f };
    camera.target = (Vector3) { 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    //Texture texture = LoadTexture("assets/dirt.png");
    Texture texture = LoadTexture("assets/tex_atlas_2.png");
    //texture.id = 1;
    //Mesh cube = gen_block_mesh();
    //UploadMesh(&cube, false);



    //Chunk chunk = { 0 };
    //chunk = gen_chunk();
    //Mesh mesh = { 0 };
    //mesh = gen_chunk_mesh(&chunk);
    //mesh = do_it_all();
    //mesh = gen_block_mesh(0.0f, 0.0f, 0.0f);

    ChunkMesh* chunkmesh = gen_chunk_mesh((Vector3) { 0.0f, 0.0f, 0.0f });

    //Mesh mesh = *chunkmesh->mesh;
    //ChunkMesh* chunkmesh = (ChunkMesh*)calloc(1, sizeof(ChunkMesh));
    //chunkmesh = gen_chunk_mesh((Vector3) { 0.0f, 0.0f, 0.0f });

    //UploadMesh(chunkmesh->mesh, false);
    //UploadMesh(&mesh, false);
    UploadMesh(chunkmesh->mesh, false);

    Material material = LoadMaterialDefault();
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix matrix = MatrixTranslate(0.0f, 0.0f, 0.0f);

    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);

        

        BeginDrawing();
            
            ClearBackground(LIGHTGRAY);
            //DrawTexture(texture, 0.0f, 0.0f, WHITE);
            BeginMode3D(camera);

                //DrawCube( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, RAYWHITE);
                //DrawCubeWires( (Vector3) { 0.0f, 1.0f, 0.0f }, 1.0f, 1.0f, 1.0f, BLACK);

                //draw_cube_basic((Vector3) { 0.0f, 1.0f, 0.0f }, WHITE, &texture);
                //draw_cube_basic((Vector3) { 1.0f, 0.0f, 0.0f }, WHITE, &texture);
                //draw_cube_basic((Vector3) { -2.0f, 1.0f, 0.0f }, WHITE, &texture);

                //DrawMesh(*chunkmesh->mesh, material, matrix);
                //DrawMesh(mesh, material, matrix);
                DrawMesh(*(chunkmesh->mesh), material, matrix);

            EndMode3D();
            
            DrawFPS(10, 10);
        EndDrawing();
    }

    //UnloadMesh(*chunkmesh->mesh);
    UnloadMesh(*(chunkmesh->mesh));
    //UnloadMesh(mesh);
    UnloadTexture(texture);
    CloseWindow();

    return 0;       
}

Mesh do_it_all(void) {
    // generate a conceptual chunk
    // account for each vertex in each cube seperately
    // combine all vertices and normals and texcoords into mega arrays
    // return as mesh to draw

    //NOTE: if i'm not going to use indices, i need to treat vertices as triangles not quads

    Chunk chunk = { 0 };
    chunk.world_pos = (Vector3) { 0.0f, 0.0f, 0.0f };
    int block_counter = 0;
    int total_vertex_count = 0;
    int total_triangle_count = 0;
    //int total_index_count = 0;
    float total_vertices[CHUNK_CUBED * 36 * 3];
    float total_texcoords[CHUNK_CUBED * 36 * 2];
    float total_normals[CHUNK_CUBED * 36 * 3];
    //float total_indices[CHUNK_CUBED * 36];

    for (int i = 0; i < CHUNK_SIZE; i++) {
        //create all the conceptual blocks
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                Block block = { 0 };
                block.pos.x = chunk.world_pos.x - HALF_CHUNK + i;
                block.pos.y = chunk.world_pos.y - HALF_CHUNK + j;
                block.pos.z = chunk.world_pos.z - HALF_CHUNK + k;

                //some basic culling
                if(i == 0 || j == 0 || k == 0) {
                    block.block_type = BLOCK_MAGMA;
                }else if (i == CHUNK_SIZE-1 || j == CHUNK_SIZE-1 || k == CHUNK_SIZE-1) {
                    block.block_type = BLOCK_MAGMA;
                }else{
                    block.block_type = BLOCK_AIR;
                    continue;
                }

                float x = block.pos.x;
                float y = block.pos.y;
                float z = block.pos.z;

                float size = 0.5f;

                float vertices[] = {
                    //first front triangle
                    //(counter clock wise starting with bottom left)
                    x-size, y-size, z+size,
                    x+size, y+size, z+size,
                    x-size, y+size, z+size,
                    //second front triangle
                    x-size, y-size, z+size,
                    x+size, y-size, z+size,
                    x+size, y+size, z+size,

                    //first back triangle
                    x+size, y-size, z-size,
                    x-size, y+size, z-size,
                    x+size, y+size, z-size,

                    //second back triangle
                    x+size, y-size, z-size,
                    x-size, y-size, z-size, 
                    x-size, y+size, z-size,

                    //top triangles
                    x-size, y+size, z+size,
                    x+size, y+size, z-size,
                    x-size, y+size, z-size,

                    x-size, y+size, z+size,
                    x+size, y+size, z+size,
                    x+size, y+size, z-size,

                    //bottom
                    x-size, y-size, z-size,
                    x+size, y-size, z+size,
                    x-size, y-size, z+size,

                    x-size, y-size, z-size,
                    x+size, y-size, z-size, 
                    x+size, y-size, z+size,

                    //left
                    x-size, y-size, z-size,
                    x-size, y+size, z+size,
                    x-size, y+size, z-size,

                    x-size, y-size, z-size,
                    x-size, y-size, z+size,
                    x-size, y+size, z+size,

                    //right
                    x+size, y-size, z+size,
                    x+size, y+size, z-size,
                    x+size, y+size, z+size,

                    x+size, y-size, z+size,
                    x+size, y-size, z-size,
                    x+size, y+size, z-size,
                };

                float u_min = MAGMA_TEX_COORD_U_MIN;
                float u_max = MAGMA_TEX_COORD_U_MAX;
                float v_min = MAGMA_TEX_COORD_V_MIN;
                float v_max = MAGMA_TEX_COORD_V_MAX;
                // float u_min = DIRT_LIGHT_TEX_COORD_U_MIN;
                // float u_max = DIRT_LIGHT_TEX_COORD_U_MAX;
                // float v_min = DIRT_LIGHT_TEX_COORD_V_MIN;
                // float v_max = DIRT_LIGHT_TEX_COORD_V_MAX;

                float texcoords[] = {
                    // u_min, v_max,
                    // u_max, v_max,
                    // u_max, v_min,
                    // u_min, v_min,

                    //front
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //back
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //top
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //bottom
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //left
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,
                    //right
                    u_min, v_max,
                    u_max, v_min,
                    u_min, v_min,

                    u_min, v_max,
                    u_max, v_max,
                    u_max, v_min,


                };
                float zero = 0.0f;
                float one = 1.0f;
                float normals[] = { 
                    //front
                    zero, zero, one,
                    zero, zero, one,
                    zero, zero, one,

                    zero, zero, one,
                    zero, zero, one,
                    zero, zero, one,
                    //back
                    zero, zero, -one,
                    zero, zero, -one,
                    zero, zero, -one,

                    zero, zero, -one,
                    zero, zero, -one,
                    zero, zero, -one,
                    //top
                    zero, one, zero,
                    zero, one, zero,
                    zero, one, zero,

                    zero, one, zero,
                    zero, one, zero,
                    zero, one, zero,
                    //bottom
                    zero, -one, zero,
                    zero, -one, zero,
                    zero, -one, zero,

                    zero, -one, zero,
                    zero, -one, zero,
                    zero, -one, zero,
                    //left
                    -one, zero, zero,
                    -one, zero, zero,
                    -one, zero, zero,

                    -one, zero, zero,
                    -one, zero, zero,
                    -one, zero, zero,
                    //right
                    one, zero, zero,
                    one, zero, zero,
                    one, zero, zero,

                    one, zero, zero,
                    one, zero, zero,
                    one, zero, zero,
                    
                };

                memcpy(block.vertices, vertices, 36*3*sizeof(float));
                
                memcpy(block.texcoords, texcoords, 36*2*sizeof(float));

                memcpy(block.normals, normals, 36*3*sizeof(float));

                block.vertexCount = 36;
                block.triangleCount = 12;

                total_vertex_count += 36;
                total_triangle_count += 12;
                //total_index_count += 36;

                chunk.blocks[block_counter] = &block;
                block_counter++;
            }
        }
    }
    int num_blocks_in_chunk = CHUNK_CUBED;
    int num_block_vertices = sizeof(chunk.blocks[0]->vertices) / sizeof(chunk.blocks[0]->vertices[0]);
    int num_block_texcoords = sizeof(chunk.blocks[0]->texcoords) / sizeof(chunk.blocks[0]->texcoords[0]);
    int num_block_normals = sizeof(chunk.blocks[0]->normals) / sizeof(chunk.blocks[0]->normals[0]);
    //int num_block_indices = 36;
    int Vcounter = 0;
    int Tcounter = 0;
    int Ncounter = 0;
    //int Icounter = 0;
    for (int i = 0; i < CHUNK_CUBED; i++) {
        //add up all the arrays
        //adding up vertices
        for (int k = 0; k < num_block_vertices; k++) {
            total_vertices[Vcounter++] = chunk.blocks[i]->vertices[k];
        }
        for (int k = 0; k < num_block_texcoords; k++) {
            total_texcoords[Tcounter++] = chunk.blocks[i]->texcoords[k];
        }
        for (int k = 0; k < num_block_normals; k++) {
            total_normals[Ncounter++] = chunk.blocks[i]->normals[k];
        }
        // for (int k = 0; k < num_block_indices; k++) {
        //     total_indices[Icounter++] = chunk.blocks[i].indices[k];
        // }
    }

    int num_chunk_vertices = num_block_vertices * num_blocks_in_chunk;
    int num_chunk_texcoords = num_block_texcoords * num_blocks_in_chunk;
    int num_chunk_normals = num_block_normals * num_blocks_in_chunk;
    //nt num_chunk_indices = num_block_indices * num_blocks_in_chunk;

    //put it all together in a chunk mesh

    Mesh chunk_mesh = { 0 };
    chunk_mesh.vertices = (float *)malloc(num_chunk_vertices * sizeof(float));
    memcpy(chunk_mesh.vertices, total_vertices, num_chunk_vertices * sizeof(float));
    
    chunk_mesh.texcoords = (float *)malloc(num_chunk_texcoords * sizeof(float));
    memcpy(chunk_mesh.texcoords, total_texcoords, num_chunk_texcoords * sizeof(float));

    chunk_mesh.normals = (float *)malloc(num_chunk_normals * sizeof(float));
    memcpy(chunk_mesh.normals, total_normals, num_chunk_normals * sizeof(float));

    // chunk_mesh.indices = (float *)malloc(num_chunk_indices * sizeof(float));
    // memcpy(chunk_mesh.indices, total_indices, num_chunk_indices * sizeof(float));

    chunk_mesh.vertexCount = total_vertex_count;
    chunk_mesh.triangleCount = total_triangle_count;
    //chunk_mesh.

    return chunk_mesh;
}




















Mesh gen_block_mesh(int x, int y, int z) {
    Mesh mesh = {0};

    float size = 0.5f;

    float vertices[] = {
        x-size, y-size, z+size,
        x+size, y-size, z+size,
        x+size, y+size, z+size,
        x-size, y+size, z+size,

        x+size, y-size, z-size,
        x-size, y-size, z-size,
        x-size, y+size, z-size,
        x+size, y+size, z-size,

        x-size, y+size, z+size,
        x+size, y+size, z+size,
        x+size, y+size, z-size,
        x-size, y+size, z-size,

        x+size, y-size, z+size,
        x-size, y-size, z+size,
        x-size, y-size, z-size,
        x+size, y-size, z-size,

        x-size, y-size, z-size,
        x-size, y-size, z+size,
        x-size, y+size, z+size,
        x-size, y+size, z-size,

        x+size, y-size, z+size,
        x+size, y-size, z-size,
        x+size, y+size, z-size,
        x+size, y+size, z+size,
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
        -1.0f, 0.0f, 0.0f,
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

// Chunk gen_chunk() {
//     Chunk chunk = {0};
//     chunk.world_pos = (Vector3) { 0.0f, 0.0f, 0.0f };
//     for (int i = 0; i < CHUNK_SIZE; i++) {
//         for (int j = 0; j < CHUNK_SIZE; j++) {
//             for (int k = 0; k < CHUNK_SIZE; k++) {

//                 // Block block = {0};
//                 // block = gen_block_mesh(
//                 //     chunk.world_pos.x + i - HALF_CHUNK, 
//                 //     chunk.world_pos.y + j - HALF_CHUNK, 
//                 //     chunk.world_pos.z + k - HALF_CHUNK);
//                 // block.pos = (Vector3) { (float) i, (float) j, (float) k };
//                 // block.block_type = BLOCK_MAGMA; //hard coded for now
//                 // chunk.blocks[i+j+k] = block;
//             }
//         }
//     }
//     Block block = {0};
//     // block = gen_block_mesh(
//     //     chunk.world_pos.x  - HALF_CHUNK, 
//     //     chunk.world_pos.y - HALF_CHUNK, 
//     //     chunk.world_pos.z - HALF_CHUNK);
//     block.block_type = BLOCK_MAGMA; //hard coded for now
//     chunk.blocks[0] = block;
    
//     return chunk;
// }



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