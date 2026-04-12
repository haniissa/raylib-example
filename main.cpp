#include <raylib.h>
#include <fmt/base.h>

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    int currentFps{60};

    //Store the position for the both of the circles
    Vector2 deltaCircle = {0, (float)screenHeight/ 3.0F};
    Vector2 frameCircle = {0, (float)screenHeight * (2.0F/3.0F)};

    //The speed applied to both circles
    constexpr float speed{10.0F};
    constexpr float circleRadius{32.0F};

    SetTargetFPS(currentFps); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // Adjst the FPS target based on the mouse wheel
        float mouseWheel = GetMouseWheelMove();
        if(mouseWheel != 0){
            currentFps += (int)mouseWheel;
            if(currentFps < 0) currentFps = 0;
            SetTargetFPS(currentFps);
        }
        //GetFrameTime() returns the time it took to draw the last frame, in seconds (usually called delta time)
        //Uses the delta time to make the circle look like it's moving at a "consistent" speed regardless of FPS
        //
        // Multiply by 6.0 (an arbitary value) in order to make the speed
        // visually closer to the other circle (at 60 fps), for comparison
        deltaCircle.x += GetFrameTime() * 6.0F * speed;
        //this circle can move faster or slower visually depending on the FPS
        frameCircle.x += 6.0F * speed;

        //If either circle is off the screen, reset it back to the start
        if(deltaCircle.x > screenWidth) deltaCircle.x = 0;
        if(frameCircle.x > screenWidth) frameCircle.x = 0;

        //Reset both circles positions
        if(IsKeyPressed(KEY_R)){
            deltaCircle.x = 0;
            frameCircle.x = 0;
        }
        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            //Draw both circles to the screen
            DrawCircleV(deltaCircle, circleRadius,  RED);
            DrawCircleV(frameCircle, circleRadius,  BLUE);

            //Draw both circles to the screen
            //Determine what help text to show depending on the current FPS target
            const char *fpsText{};
            if(currentFps <= 0) fpsText = TextFormat("FPS: unlimited (%i)", GetFPS());
            else fpsText = TextFormat("FPS; %i (target: %i)", GetFPS(), currentFps);
            DrawText(fpsText, 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("Frame time: %02.02fms", GetFrameTime()), 10, 30, 20, DARKGRAY);
            DrawText("Use the scroll wheel to change the fps limit, r to reset", 10, 50,  20, DARKGRAY);


            //Draw the text above the circles
            DrawText("FUNC: x += GetFrameTime()*speed", 10, 90, 20, RED);
            DrawText("FUNC: x += speed", 10, 240, 20, BLUE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
