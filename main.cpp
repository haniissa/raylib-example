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
    camera.position = (Vector3){10.0F, 10.0F, 10.0F};
    camera.target = (Vector3){0.0F, 0.0F, 0.0F};
    camera.up = (Vector3){0.0F, 1.0F, 0.0F};
    camera.fovy = 45.0F;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 cubePosition = {0.0F, 0.0F, 0.0F};

    SetTargetFPS(60); //Detect window close button or ESC key
    DisableCursor();   // Limit cursor to relative movement inside the window
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        UpdateCamera(&camera, CAMERA_FREE);
        if(IsKeyPressed(KEY_Z)) camera.target = (Vector3){0.0F, 0.0F, 0.0F};
        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
            DrawCube(cubePosition, 2.0F, 2.0F, 2.0F, RED);
            DrawCubeWires(cubePosition, 2.0F, 2.0F, 2.0F, MAROON);
            DrawGrid(10, 1.0F);
        EndMode3D();
        DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5F));
        DrawRectangleLines(10, 10, 320, 93, BLUE);

        DrawText("Free camera default controls:", 20, 20, 10, BLACK);
        DrawText("- Mouse Wheel to Zoom in-out:", 40, 40, 10, DARKGRAY);
        DrawText("- Mouse Wheel Pressed to Pan:", 40, 60, 10, DARKGRAY);
        DrawText("- Z to zoom to (0, 0, 0):", 40, 80, 10, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
