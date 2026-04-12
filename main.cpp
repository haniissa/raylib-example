// #include <cmath>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h> // A multi-OpenGL abstraction layer
#include <fmt/base.h>

#define MAX_BUILDINGS 100

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};
    constexpr int currentFps{60};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    Camera2D camera = {0};
    camera.zoom = 1.0F;

    int zoomMode{};

    SetTargetFPS(currentFps); //Detect window close button or ESC key
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // ----------------------------
        if(IsKeyPressed(KEY_ONE)) zoomMode =0;
        else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

        //Translate based on mouse right click
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0F/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }
         if(zoomMode == 0){
             //Zoom based on mouse wheel
             float wheel = GetMouseWheelMove();
             if(wheel != 0){
                 //Get the world point that is under the mouse
                 Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                 //Set the offset to where the mouse is
                 camera.offset = GetMousePosition();

                 //Set the target to match, so that  the camera maps the world space point
                 //under the cursor to the screen space point under the cursor at any zoom
                 camera.target = mouseWorldPos;

                 //Zoom increment
                 //Uses log scaling to provide consistent zoom speed
                 float scale = wheel * 0.2F;
                 camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.124F, 64.0F);
             }
         }else{
             //Zoom based on mouse right click
             if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                 //Get the world point that is under the mouse
                 Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                 //Set the offset to where the mouse is
                 camera.offset = GetMousePosition();

                 //Set the target to match, so that the camera maps the world space point
                 // under the cursor to the screen space point under the cursor at any zoom
                 camera.target = mouseWorldPos;
             }
             if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
                 //Zoom increment
                 //Uses log scaling to provide consistent zoom speed
                 float deltaX = GetMouseDelta().x;
                 float scale = 0.005F * deltaX;
                 camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125F, 64.0F);
             }
         }


        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                //Draw the 3d grid, rotated 90 degrees and centered around 0,0
                //just so we have something in the XY plane
                rlPushMatrix();
                    rlTranslatef(0, 25 * 50, 0);
                    rlRotatef(90, 1, 0, 0);
                    DrawGrid(100, 50);
                rlPopMatrix();

                //Draw a reference circle
                DrawCircle(GetScreenWidth() / 2, GetScreenHeight()/2, 50, MAROON);
            EndMode2D();

            //Draw mouse reference
            //Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera);
            DrawCircleV(GetMousePosition(), 4, DARKGRAY);
            DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), Vector2Add(GetMousePosition(), (Vector2){-44, -24}), 20, 2, BLACK);
            DrawText("[1][2] Select mouse zoom mode (Wheel or Move)", 20, 20, 20, DARKGRAY);
            if(zoomMode == 0) DrawText("Mouse left button drag to move, mouse wheel to zoom", 20, 50, 20, DARKGRAY);
            else DrawText("Mouse left button drag to move, mouse press and move to zoom", 20, 50, 20, DARKGRAY);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
