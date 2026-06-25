#include <raylib.h>
#include <raymath.h>
#include <fmt/base.h>

//Initization
constexpr int screenWidth{800};
constexpr int screenHeight{450};

int main(){

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
