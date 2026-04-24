#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <raymath.h>
#include "rcamera.h"

#define MAX_COLUMNS 20


// struct Paddle{};

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    //Define the camera to look into our 3D world
    Camera3D camera = {0};
    camera.position = (Vector3){0.0F, 10.0F, 10.0F}; // Camera position
    camera.target = (Vector3){0.0F, 0.0F, 0.0F}; // Camera looking at point
    camera.up = (Vector3){0.0F, 1.0F, 0.0F}; // camera up vector (rotation towards target)
    camera.fovy = 45.0F;
    camera.projection = CAMERA_PERSPECTIVE;  // camera mode type

    Vector3 cubePosition = {0.0F, 0.0F, 0.0F};

    SetTargetFPS(60); //Detect window close button or ESC key
    DisableCursor();   // Limit cursor to relative movement inside the window
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        //Update camera compture movment internally depending on the camera mode
        //Some default standard keyboard/mouse inputs ar hardcode to simplify use
        //For advanced camera controls, it's recommended to compute camera movement manually

        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);
                    DrawCube(cubePosition, 2.0F, 2.0F, 2.0F, RED);
                    DrawCubeWires(cubePosition, 2.0F, 2.0F, 2.0F, MAROON);

                    DrawGrid(10,1.0F);
                EndMode3D();

                DrawText("Welcome to the third cube dimension!", 10, 40, 20, DARKGRAY);
                DrawFPS(10, 10);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
