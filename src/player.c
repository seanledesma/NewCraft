#include "include.h"

void InitPlayer(Player* player, Camera* camera) {

    player->position = (Vector3) {
        camera->position.x,
        camera->position.y - PLAYER_HEIGHT,
        camera->position.z
    };

    player->bounding_box.min = player->position;
    player->bounding_box.max = (Vector3) {
        player->position.x + 1.0,
        player->position.y + 2.0,
        player->position.z + 1.0
    };

    player->on_ground = false;
}


void UpdatePlayer(Player* player, Camera* camera, BoundingBox* boxes) {
    TraceLog(LOG_DEBUG, TextFormat("camera y: %.2f", camera->position.y));
    TraceLog(LOG_DEBUG, TextFormat("player y: %.2f", player->position.y));

    //must update bounding box with camera's position to check if bounding box will hit
    //if bounding box hits, need to roll both camera position and bounding box position back
    player->bounding_box.min = (Vector3) {
        camera->position.x,
        camera->position.y - PLAYER_HEIGHT,
        camera->position.z
    };
    player->bounding_box.max = (Vector3) {
        camera->position.x + 1.0,
        camera->position.y - PLAYER_HEIGHT + 2.0,
        camera->position.z + 1.0
    };


    float deltatime = GetFrameTime();
    player->velocity.y += GRAVITY * (deltatime / 1); 
    camera->position.y += player->velocity.y * (deltatime / 1);
    TraceLog(LOG_DEBUG, TextFormat("player velocity y: %.2f", player->velocity.y));
    TraceLog(LOG_DEBUG, TextFormat("deltatime: %.2f", deltatime));
    if(player->on_ground == false) {
        //gravity, affects camera then player position
        

    }
    //TraceLog(LOG_WARNING, "hi");
    
    //check if player hits the bounding box under player, if so, reset camera position
    //else update player position to align with camera
    if(CheckCollisionBoxes(player->bounding_box, boxes[0])) {
        TraceLog(LOG_DEBUG, TextFormat("hit %.5f", deltatime));
        camera->position.y = ceil(boxes[0].max.y) + PLAYER_HEIGHT + 0.00001f;
        //player->position.y = ceil(boxes[0].max.y) + 0.0000001f;
        // camera->position = (Vector3) {
        //     player->position.x,
        //     player->position.y + PLAYER_HEIGHT,
        //     player->position.z
        // };
        // player->bounding_box.min = (Vector3) {
        //     player->position.x,
        //     player->position.y,
        //     player->position.z
        // };
        // player->bounding_box.max = (Vector3) {
        //     player->position.x + 1.0,
        //     player->position.y + 2.0,
        //     player->position.z + 1.0
        // };
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
        player->velocity.y = 0;
        player->on_ground = true;
    } else {
        // player->position = (Vector3) {
        //     camera->position.x,
        //     camera->position.y - PLAYER_HEIGHT,
        //     camera->position.z
        // };
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
    if (IsKeyPressed(KEY_SPACE) && player->on_ground == true) {
        player->on_ground = false;
        camera->position.y += 0.1f;
        player->velocity.y = 5;
    }




}
