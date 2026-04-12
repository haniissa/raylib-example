#include <raylib.h>
#include <fmt/base.h>

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    int currentFps{60};

    Vector2 ballPosition = {-100.0F, -100.0F};
    Color ballColor = DARKBLUE;

    SetTargetFPS(currentFps); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // ----------------------------
        if(IsKeyPressed(KEY_H)){
            if(IsCursorHidden()) ShowCursor();
            else HideCursor();
        }
        ballPosition = GetMousePosition();

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ballColor = MAROON;
        else if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) ballColor = LIME;
        else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) ballColor = DARKBLUE;
        else if(IsMouseButtonDown(MOUSE_BUTTON_SIDE)) ballColor = PURPLE;
        else if(IsMouseButtonDown(MOUSE_BUTTON_EXTRA)) ballColor = YELLOW;
        else if(IsMouseButtonDown(MOUSE_BUTTON_FORWARD)) ballColor = ORANGE;
        else if(IsMouseButtonDown(MOUSE_BUTTON_BACK)) ballColor = BEIGE;
        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(ballPosition, 40, ballColor);
            DrawText("move ball with mouse and click mouse button to change color", 10, 10, 20, DARKGRAY);
            DrawText("Press 'H' to toggle cursor visiblity", 10, 30, 20, DARKGRAY);

            if(IsCursorHidden()) DrawText("CURSOR Hidden", 20, 60, 20, RED);
            else DrawText("CURSOR VISIBLE", 20, 60, 20, LIME);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
