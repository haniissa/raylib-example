#include <raylib.h>
#include <fmt/base.h>

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    int currentFps{60};

    int  boxPositionY = {screenHeight/2 - 40};
    int scrollSpeed{4};

    Color ballColor = DARKBLUE;

    SetTargetFPS(currentFps); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // ----------------------------
        boxPositionY -= (int)(GetMouseWheelMove()*scrollSpeed);
        // Draw
        // ---------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(screenWidth/2 - 40, boxPositionY, 80, 80, MAROON);

        DrawText("Use mouse wheel to move the cube up and down!", 10, 10, 20, GRAY);
        DrawText(TextFormat("Box position Y: %03i", boxPositionY), 10,40, 20 ,LIGHTGRAY);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
