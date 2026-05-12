#include "animation.h"
#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <fmt/chrono.h>
#include <map>

#define FPS 60
#define WIDTH 800
#define HEIGHT 450




//convert index to coordinate

int main(){
    //Initization
    constexpr int screenWidth{WIDTH};
    constexpr int screenHeight{HEIGHT};
    InitWindow(screenWidth, screenHeight, "raylib animation");

    //make the asset folder the working directory
    // SetWorkingDirectory("..\\..\\asset");


    //Load assets (Replace "sprite_sheet.png") with your actual filename
    Texture2D texIdle   = LoadTexture("asset/FreeKnight_v1/Colour1/Outline/120x80_PNGSheets/_Idle.png");
    Texture2D texRun    = LoadTexture("asset/FreeKnight_v1/Colour1/Outline/120x80_PNGSheets/_Run.png");
    Texture2D texAttack = LoadTexture("asset/FreeKnight_v1/Colour1/Outline/120x80_PNGSheets/_Attack.png");
    Texture2D texHit    = LoadTexture("asset/FreeKnight_v1/Colour1/Outline/120x80_PNGSheets/_Hit.png");

    ///Define the frames for each state based on your image rows
    // rows 0:  Idle, row 1: Attack, row 3: HIt
    std::map<AnimState, Animation> animConfigs = {
        {AnimState::IDLE, {texIdle, 10,  0.15F, false}},
        {AnimState::RUN, {texRun, 10,  0.35F, false}},
        {AnimState::ATTACK, {texAttack, 4,  0.25F, false}},
        {AnimState::HIT, {texHit, 10,  0.07F,false}}
    };

    //Initilize our Animator struct
    Animator knight;
    // InitAnimator(&knight, knightTex, knightTex.width / 6, knightTex.height / 4);
    InitAnimator(&knight, 120, 80);

    fmt::print("system initliazed. pressed 1: Idle, 2: Run,  3:Attack, 4: Hit\n");

    SetTargetFPS(FPS); //Detect window close button or ESC key
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        float dt = GetFrameTime();

        //Input
        if(IsKeyPressed(KEY_ONE)) SetAnimation(&knight, AnimState::IDLE);
        if(IsKeyPressed(KEY_TWO)) SetAnimation(&knight, AnimState::RUN);
        if(IsKeyPressed(KEY_THREE)) SetAnimation(&knight, AnimState::ATTACK);
        if(IsKeyPressed(KEY_FOUR)) SetAnimation(&knight, AnimState::HIT);

        //Update
        Animation currentCfg = animConfigs[knight.currentState];
        UpdateAnimator(&knight, currentCfg, dt);


        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            Rectangle source = GetCurrentFrameRec(&knight);
            Rectangle dest = {(screenWidth/2.0F) - 64, (screenHeight/2.0F) - 64, 128, 128};
            Vector2 origin = {64, 64};

            DrawTexturePro(currentCfg.texture, source, dest, origin, 0.0F, WHITE );
            DrawText("1:Idle | 2:Run | 3:Attack | 4: Hit", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    // UnloadTexture(knightTex); // for the big sprite sheet
    // Unload all textures
    UnloadTexture(texIdle);
    UnloadTexture(texRun);
    UnloadTexture(texAttack);
    UnloadTexture(texHit);

    CloseWindow();

    return 0;
}
