#include "include.h"
#include "rcamera.h"

void InitPlayer(Player* player, Camera* camera) {

    //camera must be initialized before player
    player->position = (Vector3) {
        camera->position.x,
        camera->position.y - PLAYER_HEIGHT,
        camera->position.z
    };
    player->up = camera->up;
    player->target = camera->target;

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
    player->collision_x = false;
    player->collision_z = false;
}

Vector3 GetPlayerForward(Player* player) {
    return Vector3Normalize(Vector3Subtract(player->target, player->position));
}

Vector3 GetPlayerUp(Player* player) {
    return Vector3Normalize(player->up);
}

Vector3 GetPlayerRight(Player* player) {
    Vector3 forward = GetPlayerForward(player);
    Vector3 up = GetPlayerUp(player);

    return Vector3Normalize(Vector3CrossProduct(forward, up));
}

void PlayerMoveForward(Player* player, float distance) {
    Vector3 forward = GetPlayerForward(player);

    // Project vector onto world plane (the plane defined by the up vector)
    if (fabsf(player->up.z) > 0.7071f) forward.z = 0;
    else if (fabsf(player->up.x) > 0.7071f) forward.x = 0;
    else forward.y = 0;

    forward = Vector3Normalize(forward);

    // Scale by distance
    forward = Vector3Scale(forward, distance);

    // Move position and target
    player->position = Vector3Add(player->position, forward);
    player->target = Vector3Add(player->target, forward);
}

void PlayerMoveRight(Player* player, float distance) {
    Vector3 right = GetPlayerRight(player);

    // Project vector onto world plane (the plane defined by the up vector)
    if (fabsf(player->up.z) > 0.7071f) right.z = 0;
    else if (fabsf(player->up.x) > 0.7071f) right.x = 0;
    else right.y = 0;

    right = Vector3Normalize(right);

    // Scale by distance
    right = Vector3Scale(right, distance);

    // Move position and target
    player->position = Vector3Add(player->position, right);
    player->target = Vector3Add(player->target, right);
}

void PlayerMoveUp(Player* player, float distance) {
    Vector3 up = GetPlayerUp(player);

    // Scale by distance
    up = Vector3Scale(up, distance);

    // Move position and target
    player->position = Vector3Add(player->position, up);
    player->target = Vector3Add(player->target, up);
}

// to do: update camera only after updating player position, checking collisions
void UpdatePlayer(Player* player, Camera* camera, BoundingBox* boxes, int nearby_boxes_count, HashTable* hash_table) {
    float playerMoveSpeed = CAMERA_MOVE_SPEED*GetFrameTime();
    float deltatime = GetFrameTime();

    Vector2 mousePositionDelta = GetMouseDelta();

    bool rotateAroundTarget = false;
    bool lockView = true;
    bool rotateUp = false;

    // Mouse support
    PlayerYaw(player, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY);
    PlayerPitch(player, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY);

    // Keyboard support
    if (IsKeyDown(KEY_W)) PlayerMoveForward(player, playerMoveSpeed);
    if (IsKeyDown(KEY_A)) PlayerMoveRight(player, -playerMoveSpeed);
    if (IsKeyDown(KEY_S)) PlayerMoveForward(player, -playerMoveSpeed);
    if (IsKeyDown(KEY_D)) PlayerMoveRight(player, playerMoveSpeed);

    // handle jumping
    if (IsKeyPressed(KEY_SPACE) && player->on_ground == true && player->flying == false) {
        player->on_ground = false;
        PlayerMoveUp(player, 0.1f);
        player->velocity.y = 5;
    }

    // handle flying
    if (IsKeyPressed(KEY_F)) player->flying = true;
    if (IsKeyPressed(KEY_G)) player->flying = false;

    if (player->flying) {
        player->on_ground = false;
        if (IsKeyDown(KEY_SPACE)) {
            player->position.y += 0.1f;
            //camera->target.y += 0.1f;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            player->position.y -= 0.1f;
            //camera->target.y -= 0.1f;
        }
    }

    //handle falling
    if(player->flying == false) {
        //gravity, affects camera then player position
        player->velocity.y += GRAVITY * (deltatime / 1); 
        player->position.y += player->velocity.y * (deltatime / 1);
        player->target_offset += player->velocity.y * deltatime;
    }

    //must update bounding box with camera's position to check if bounding box will hit
    //if bounding box hits, need to roll both camera position and bounding box position back
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
    
    //check if player hits the bounding box under player, if so, reset camera position
    //else update player position to align with camera
    if(CheckCollisionBoxes(player->bounding_box, boxes[0]) && player->flying == false) {
        TraceLog(LOG_DEBUG, TextFormat("hit %.5f", deltatime));
        //player->position.y = ceil(boxes[0].max.y) + 0.00001f;
        player->position.y = camera->position.y - PLAYER_HEIGHT;
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

        flash forward a bit, player now slides along z wall, but only slides a bit on x wall. because a soon as 
        the bounding box colliding with player is diagonal, z wall logic takes over and skips x wall logic.
        however, seems like there's no way ...
        well, why is it colliding diagonal on a straight wall? should always just be checking block right before, not diagonal..
        cuz player bounding box goes further out than player, so it crosses into the next block over before the base block updates.
        going to have to deal with diagonal cases

        what if.. what if we checked all the boxes for collision, and only proceed if the box has the same x or z pos 
    */
//    bool check_collision_x = false;
//    bool check_collision_z = false;
    for (int i = 1; i < nearby_boxes_count; i++) {
        if (boxes[i].min.y >= player->bounding_box.min.y) {
            if (CheckCollisionBoxes(player->bounding_box, boxes[i]) && player->flying == false) {
                // i could use boxes[0] if this sticks around 
                Vector3 base_block_world = (Vector3) {
                    floor(player->position.x),
                    floor(player->position.y),
                    floor(player->position.z)
                };
                float z_point = (boxes[i].min.z);  //find middle point
                float x_point = (boxes[i].min.x);

                // check if we have the box right in front of player
                if (z_point == base_block_world.z) {
                    if (x_point < base_block_world.x) {
                        TraceLog(LOG_WARNING, "hit x point");
                        TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                        player->collision_x = true;
                        player->position.x = camera->position.x;
                        //player->position.x = x_point + 0.0001;
                    }
                    if (x_point > base_block_world.x) { 
                        TraceLog(LOG_WARNING, "hit x point prt 2");
                        player->collision_x = true;
                        player->position.x = camera->position.x;
                        //player->position.x = x_point - 0.0001;
                    }
                }

                if (x_point ==base_block_world.x) {
                    if (z_point < base_block_world.z) {
                        TraceLog(LOG_WARNING, "hit z point");
                        TraceLog(LOG_WARNING, TextFormat("zPOint: %.2f", z_point));
                        TraceLog(LOG_WARNING, TextFormat("player z: %.2f", base_block_world.z));
                        TraceLog(LOG_WARNING, TextFormat("Xpoint: %.2f", x_point));
                        TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                        player->collision_z = true;
                        player->position.z = camera->position.z;    // idea here is to keep player from going into mesh
                    }
                    if (z_point > base_block_world.z) {
                        TraceLog(LOG_WARNING, "hit z point prt 2");
                        TraceLog(LOG_WARNING, TextFormat("zPOint: %.2f", z_point));
                        TraceLog(LOG_WARNING, TextFormat("player z: %.2f", base_block_world.z));
                        TraceLog(LOG_WARNING, TextFormat("Xpoint: %.2f", x_point));
                        TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                        player->collision_z = true;
                        player->position.z = camera->position.z;
                    }
                }


                // text for positive z face of bounding box
                // if (z_point < base_block_world.z && x_point == base_block_world.x) {
                //     TraceLog(LOG_WARNING, "hit z point");
                //     TraceLog(LOG_WARNING, TextFormat("zPOint: %.2f", z_point));
                //     TraceLog(LOG_WARNING, TextFormat("player z: %.2f", base_block_world.z));
                //     TraceLog(LOG_WARNING, TextFormat("Xpoint: %.2f", x_point));
                //     TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                //     player->collision_z = true;
                //     player->position.z = camera->position.z;    // idea here is to keep player from going into mesh
                // } else if (z_point > base_block_world.z) {
                //     TraceLog(LOG_WARNING, "hit z point prt 2");
                //     TraceLog(LOG_WARNING, TextFormat("zPOint: %.2f", z_point));
                //     TraceLog(LOG_WARNING, TextFormat("player z: %.2f", base_block_world.z));
                //     TraceLog(LOG_WARNING, TextFormat("Xpoint: %.2f", x_point));
                //     TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                //     player->collision_z = true;
                //     player->position.z = camera->position.z;
                // } else if (x_point < base_block_world.x) {
                //     TraceLog(LOG_WARNING, "hit x point");
                //     TraceLog(LOG_WARNING, TextFormat("player x: %.2f", base_block_world.x));
                //     player->collision_x = true;
                //     player->position.x = camera->position.x;
                // } else if (x_point > base_block_world.x) { 
                //     TraceLog(LOG_WARNING, "hit x point prt 2");
                //     player->collision_x = true;
                //     player->position.x = camera->position.x;
                // } else player->collision_x = false;
                







                // if (floor(boxes[i].min.z) < floor(player->position.z)) {
                //     //camera->position.z = player->position.z;
                //     player->collision_z = true;
                // } else if (floor(boxes[i].min.z) > floor(player->position.z)) {
                //     //camera->position.z = player->position.z;
                //     player->collision_z = true;
                // }
                // if (floor(boxes[i].min.x) < floor(player->position.x)) {
                //     //camera->position.x = player->position.x;
                //     player->collision_x = true;
                // } else if (floor(boxes[i].min.x) > floor(player->position.x)) {
                //     //camera->position.x = player->position.x;
                //     player->collision_x = true;
                // }                
                //camera->position.z = player->position.z;
                //DrawBoundingBox(boxes[i], BLUE);
            }
        }
    }

    //need a catch-all, if player ever gets stuck inside a block that isn't air, get out!
    if(!IsBlockAir(player->position, hash_table)) {
        player->position.y += 1;
    }

    // if (player->collision_x == true && player->collision_z == true) {
    //     player->collision_z = false;
    // }
    if(player->collision_x == false) {
        camera->position.x = player->position.x;
    }
    if(player->collision_z == false) {
        camera->position.z = player->position.z;
    }
    
    player->collision_x = false;
    player->collision_z = false;

    camera->position.y = player->position.y + PLAYER_HEIGHT;
    camera->up = player->up;
    camera->target = player->target;

}

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// Note: angle must be provided in radians
void PlayerYaw(Player* player, float angle)
{
    // Rotation axis
    Vector3 up = GetPlayerUp(player);

    // View vector
    Vector3 targetPosition = Vector3Subtract(player->target, player->position);

    // Rotate view vector around up axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

    // Move target relative to position
    player->target = Vector3Add(player->position, targetPosition);
}

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
// NOTE: [angle] must be provided in radians
void PlayerPitch(Player* player, float angle)
{
    // Up direction
    Vector3 up = GetPlayerUp(player);

    // View vector
    Vector3 targetPosition = Vector3Subtract(player->target, player->position);

    // Clamp view up
    float maxAngleUp = Vector3Angle(up, targetPosition);
    maxAngleUp -= 0.001f; // avoid numerical errors
    if (angle > maxAngleUp) angle = maxAngleUp;

    // Clamp view down
    float maxAngleDown = Vector3Angle(Vector3Negate(up), targetPosition);
    maxAngleDown *= -1.0f; // downwards angle is negative
    maxAngleDown += 0.001f; // avoid numerical errors
    if (angle < maxAngleDown) angle = maxAngleDown;

    // Rotation axis
    Vector3 right = GetPlayerRight(player);

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

    player->target = Vector3Add(player->position, targetPosition);
}
