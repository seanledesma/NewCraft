#include "include.h"

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


void UpdatePlayer(Player* player, Camera* camera, BoundingBox* boxes, int nearby_boxes_count) {
    TraceLog(LOG_DEBUG, TextFormat("camera y: %.2f", camera->position.y));
    TraceLog(LOG_DEBUG, TextFormat("player y: %.2f", player->position.y));

    //must update bounding box with camera's position to check if bounding box will hit
    //if bounding box hits, need to roll both camera position and bounding box position back
    // player->bounding_box.min = (Vector3) {
    //     camera->position.x,
    //     camera->position.y - PLAYER_HEIGHT,
    //     camera->position.z
    // };
    // player->bounding_box.max = (Vector3) {
    //     camera->position.x + 1.0,
    //     camera->position.y - PLAYER_HEIGHT + 2.0,
    //     camera->position.z + 1.0
    // };

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


    float deltatime = GetFrameTime();

    //handle falling
    if(player->flying == false) {
        //gravity, affects camera then player position
        player->velocity.y += GRAVITY * (deltatime / 1); 
        camera->position.y += player->velocity.y * (deltatime / 1);
        player->target_offset += player->velocity.y * deltatime;
        //camera->target.y += player->velocity.y * (deltatime / 1);
        
        TraceLog(LOG_DEBUG, TextFormat("player velocity y: %.2f", player->velocity.y));
        TraceLog(LOG_DEBUG, TextFormat("deltatime: %.2f", deltatime));
    }
    //TraceLog(LOG_WARNING, "hi");
    
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
    */
    for (int i = 1; i < nearby_boxes_count; i++) {
        if (boxes[i].min.y > camera->position.y - PLAYER_HEIGHT) {
            if (CheckCollisionBoxes(player->bounding_box, boxes[i]) && player->flying == false) {
                camera->position.x = player->position.x;
                camera->position.z = player->position.z;
                //DrawBoundingBox(boxes[i], BLUE);
            }
        }
    }




    // for (int i = 0; i < 10*10*5; i++) {
    //     if(CheckCollisionBoxes(player->bounding_box, boxes[i])) {
    //         camera->position = (Vector3) {
    //             player->position.x,
    //             player->position.y + PLAYER_HEIGHT,
    //             player->position.z
    //         };
    //         player->on_ground = true;
    //     } else {
    //         player->position = (Vector3) {
    //             camera->position.x,
    //             camera->position.y - PLAYER_HEIGHT,
    //             camera->position.z
    //         };
    //     }
    // }


    // handle jumping
    if (IsKeyPressed(KEY_SPACE) && player->on_ground == true && player->flying == false) {
        player->on_ground = false;
        camera->position.y += 0.1f;
        //camera->target.y += 0.1f;
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


}
