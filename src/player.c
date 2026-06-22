#include "include.h"
#include "rcamera.h"

void InitPlayer(Player* player, Camera* camera) {

    player->position = (Vector3) {
        camera->position.x,
        camera->position.y - PLAYER_HEIGHT,
        camera->position.z
    };

    player->bounding_box.min = (Vector3) {
        player->position.x - (PLAYER_WIDTH / 2),
        player->position.y,
        player->position.z - (PLAYER_DEPTH / 2)
    };

    player->bounding_box.max = (Vector3) {
        player->position.x + (PLAYER_WIDTH / 2),
        player->position.y + PLAYER_HEIGHT,
        player->position.z + (PLAYER_WIDTH / 2)
    };

    player->on_ground = false;
    player->flying = true;
    player->target_offset = 0.0f;
}

// to do: update camera only after updating player position, checking collisions
void UpdatePlayer(Player* player, Camera* camera, BoundingBox* boxes, int nearby_boxes_count) {
    float cameraMoveSpeed = CAMERA_MOVE_SPEED*GetFrameTime();
    float cameraRotationSpeed = CAMERA_ROTATION_SPEED*GetFrameTime();
    float cameraPanSpeed = CAMERA_PAN_SPEED*GetFrameTime();
    float cameraOrbitalSpeed = CAMERA_ORBITAL_SPEED*GetFrameTime();
    float deltatime = GetFrameTime();

    Vector2 mousePositionDelta = GetMouseDelta();

    bool moveInWorldPlane = true;
    bool rotateAroundTarget = false;
    bool lockView = true;
    bool rotateUp = false;

    // Mouse support
    CameraYaw(camera, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
    CameraPitch(camera, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

    // Keyboard support
    if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);

    // handle jumping
    if (IsKeyPressed(KEY_SPACE) && player->on_ground == true && player->flying == false) {
        player->on_ground = false;
        CameraMoveUp(camera, 0.1f);
        player->velocity.y = 5;
    }

    // handle flying
    if (IsKeyPressed(KEY_F)) player->flying = true;
    if (IsKeyPressed(KEY_G)) player->flying = false;

    if (player->flying) {
        player->on_ground = false;
        if (IsKeyDown(KEY_SPACE)) {
            camera->position.y += 0.1f;
            //camera->target.y += 0.1f;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            camera->position.y -= 0.1f;
            //camera->target.y -= 0.1f;
        }
    }

    //handle falling
    if(player->flying == false) {
        //gravity, affects camera then player position
        player->velocity.y += GRAVITY * (deltatime / 1); 
        camera->position.y += player->velocity.y * (deltatime / 1);
        player->target_offset += player->velocity.y * deltatime;
    }

    //must update bounding box with camera's position to check if bounding box will hit
    //if bounding box hits, need to roll both camera position and bounding box position back
    player->bounding_box.min = (Vector3) {
        camera->position.x - (PLAYER_WIDTH / 2),
        camera->position.y - PLAYER_HEIGHT,
        camera->position.z - (PLAYER_DEPTH / 2)
    };

    player->bounding_box.max = (Vector3) {
        camera->position.x + (PLAYER_WIDTH / 2),
        camera->position.y,
        camera->position.z + (PLAYER_WIDTH / 2)
    }; 
    
    //check if player hits the bounding box under player, if so, reset camera position
    //else update player position to align with camera
    if(CheckCollisionBoxes(player->bounding_box, boxes[0]) && player->flying == false) {
        TraceLog(LOG_DEBUG, TextFormat("hit %.5f", deltatime));
        camera->position.y = ceil(boxes[0].max.y) + PLAYER_HEIGHT + 0.00001f;
        player->target_offset = 0;
        player->velocity.y = 0;
        player->on_ground = true;
    }

    // for (int i = 1; i < nearby_boxes_count; i++) {
    //     if (CheckCollisionBoxes(player->bounding_box, boxes[i]) && player->flying == false) {
    //         camera->position.x = player->position.x;
    //         camera->position.z = player->position.z;
    //     }
    // }

    /*
        thinking.. the player needs to slide along the borders / blocks he's collided into instead of just stopping in all
        directions. Could somehow check which face of the block the player hit so i only stop x or z, not both... how?

        I could take the box that the player hit, take the position, find the chunk location then the block index, and then
        do some basic math to figure out where the cube is in relation to the player. or... i just do that math using the 
        bounding box and the player's location, like we know we hit, so if the box.pos.x > player.pos.x but box.pos.z (floor)
        is not then we know the player is hitting a box in the positive x direction.
    */
    for (int i = 1; i < nearby_boxes_count; i++) {
        if (boxes[i].min.y > camera->position.y - PLAYER_HEIGHT) {
            if (CheckCollisionBoxes(player->bounding_box, boxes[i]) && player->flying == false) {
                if (floor(boxes[i].min.z) < floor(player->position.z)) {
                    camera->position.z = player->position.z;
                } else if (floor(boxes[i].min.z) > floor(player->position.z)) {
                    camera->position.z = player->position.z;
                }
                if (floor(boxes[i].min.x) < floor(player->position.x)) {
                    camera->position.x = player->position.x;
                } else if (floor(boxes[i].min.x) > floor(player->position.x)) {
                    camera->position.x = player->position.x;
                }
                
                //camera->position.z = player->position.z;
                //DrawBoundingBox(boxes[i], BLUE);
            }
        }
    }
}
