#pragma once
#include <raylib.h>
#include <vector>
#include <string>

enum class AnimState{
    IDLE,
    RUN,
    ATTACK,
    HIT,
};

struct Animation{
    Texture2D texture;  // Now each animation haw its own image file
    // int row; // which row in the sprit sheet
    int frameCount; //  How many frames in this animation
    float speed;   // How fast switch frames (seconds per frame)
    bool loop; // Should it restart after the last frame ?
};

struct Animator {
    AnimState currentState;
    Texture2D spriteSheet;
    float timer;
    int currentFrame;
    int frameWidth;
    int frameHeight;
};

//-----logic functions
// Initialize the animator for one big sprite
// void InitAnimator(Animator* anim, Texture2D tex, int width, int height){
//     anim->spriteSheet = tex;
//     anim->frameWidth = width;
//     anim->frameHeight = height;
//     anim->currentState = AnimState::IDLE;
//     anim->currentFrame = 0;
//     anim->timer = 0.0F;
// }


void InitAnimator(Animator* anim, int width, int height){
    anim->currentState = AnimState::IDLE;
    anim->currentFrame = 0;
    anim->timer = 0.0F;
    anim->frameWidth = width;
    anim->frameHeight = height;
}

//Switch to a different animation state
void SetAnimation(Animator* anim, AnimState newState){
    if(anim->currentState !=  newState){
        anim->currentState = newState;
        anim->currentFrame = 0;
        anim->timer = 0.0F;
    }
}

//Calculate which rectangle of the texture draw for the one big texture
// Rectangle GetCurrentFrameRec(Animator* anim, Animation config){
//     return {
//         (float)(anim->currentFrame * anim->frameWidth),
//         (float)(config.row * anim->frameHeight),
//         (float)anim->frameWidth,
//         (float)anim->frameHeight,
//     };
// }

Rectangle GetCurrentFrameRec(Animator* anim){
    return {
        (float)(anim->currentFrame * anim->frameWidth),
        0.0F, // Always 0 because it's a single-row image
        (float)anim->frameWidth,
        (float)anim->frameHeight,
    };
}


//Update the frame base on time
void UpdateAnimator(Animator* anim, Animation config, float deltaTime){
    anim->timer += deltaTime;

    if(anim->timer >= config.speed){
        anim->timer = 0.0F;
        anim->currentFrame = 0;
    }else {
        anim->currentFrame = config.loop ? 0 : config.frameCount - 1;  // Stay on last frame
    }
}
