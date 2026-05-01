#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <fmt/chrono.h>

#define FPS 60
#define WIDTH 800
#define HEIGHT 450

int main(){
    //Initization
    constexpr int screenWidth{WIDTH};
    constexpr int screenHeight{HEIGHT};
    InitWindow(screenWidth, screenHeight, "raylib animation");



    SetTargetFPS(FPS); //Detect window close button or ESC key
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------

        // Draw
        // ---------------------
        BeginDrawing();

        EndDrawing();
    }
    UnloadTexture(player_idle_texture);
    CloseWindow();

    return 0;
}
