#include <raylib.h>
#include <fmt/base.h>

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    int currentFps{60};

    Vector2 ballPosition = {(float)screenWidth/2, (float)screenHeight/2};
    SetTargetFPS(currentFps); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        if(IsKeyPressed(KEY_RIGHT)) ballPosition.x += 2.0F;
        if(IsKeyPressed(KEY_LEFT)) ballPosition.x -= 2.0F;
        if(IsKeyPressed(KEY_UP)) ballPosition.y -= 2.0F;
        if(IsKeyPressed(KEY_DOWN)) ballPosition.y += 2.0F;


        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);
            DrawCircleV(ballPosition, 50, MAROON);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
