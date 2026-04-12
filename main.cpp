// #include <cmath>
#include <math.h>
#include <raylib.h>
#include <fmt/base.h>

#define MAX_BUILDINGS 100

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    Rectangle player = {400, 280, 40, 40};
    Rectangle buildings[MAX_BUILDINGS] = {0};
    Color buildColor[MAX_BUILDINGS] = {0};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    int currentFps{60};

    int spacing{};
    for(int i{}; i < MAX_BUILDINGS; ++i){
        buildings[i].width = (float)GetRandomValue(50, 200);
        buildings[i].height = (float)GetRandomValue(100, 800);
        buildings[i].y = screenHeight - 130.0F - buildings[i].height;
        buildings[i].x = -6000.0F + spacing;

        spacing += (int)buildings[i].width;

        buildColor[i] = (Color){
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 250),
            255
        };
    }
    Camera2D camera = {0};
    camera.target = (Vector2){player.x + 20.0F, player.y + 20.0F};
    camera.offset = (Vector2){screenWidth/2.0F, screenHeight/2.0F};
    camera.rotation = 0.0F;
    camera.zoom = 1.0F;

    SetTargetFPS(currentFps); //Detect window close button or ESC key

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // ----------------------------
        //player movement
        if(IsKeyDown(KEY_RIGHT)) player.x += 2;
        else if(IsKeyDown(KEY_LEFT)) player.x -= 2;

        //Camera target follows player
        camera.target = (Vector2){ player.x + 20, player.y + 20};

        //Camera rotation controls
        if(IsKeyDown(KEY_A)) camera.rotation--;
        else if(IsKeyDown(KEY_S)) camera.rotation++;

        //Limit camera roation to 80 degrees (-40, 40)
        if(camera.rotation > 40) camera.rotation = 40;
        else if(camera.rotation < -40) camera.rotation = -40;

        //Camera zoom controls
        // Use log scaling to provide consistent zoom speed
        camera.zoom = expf(logf(camera.zoom)+ ((float)GetMouseWheelMove() * 0.1F));

        if(camera.zoom > 3.0F) camera.zoom = 3.0F;
        else if(camera.zoom < 0.1F) camera.zoom = 0.1F;

        //Camera reset (zoom and rotation)
        if(IsKeyPressed(KEY_R)){
            camera.zoom = 1.0F;
            camera.rotation = 0.0F;
        }


        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                DrawRectangle(-6000, 320, 13000, 8000, DARKGRAY);
                for(int i{}; i < MAX_BUILDINGS; ++i) DrawRectangleRec(buildings[i], buildColor[i]);
                DrawRectangleRec(player, RED);
                DrawLine((int)camera.target.x, -screenHeight*10, (int)camera.target.x, screenHeight*10, GREEN);
                DrawLine(-screenWidth*10, (int)camera.target.y, screenWidth*10, (int)camera.target.y, GREEN);
            EndMode2D();

            DrawText("SCREEN AREA", 640, 10, 20, RED);

            DrawRectangle(0, 0, screenWidth, 5, RED);
            DrawRectangle(0, 5, 5, screenHeight - 10, RED);
            DrawRectangle(screenHeight -5, 5, 5, screenHeight - 10, RED);
            DrawRectangle(0, screenHeight - 5, screenWidth, 5, RED);


            DrawRectangle(10, 10, 250, 113, Fade(SKYBLUE, 0.5F));
            DrawRectangle(10, 10, 250, 113, BLUE);

            DrawText("Free 2D camera controls:", 20, 20, 10, BLACK);
            DrawText("- Right/Left to move player", 40, 40, 10, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, DARKGRAY);
            DrawText("- A / S to Rotate", 40, 80, 10, DARKGRAY);
            DrawText("- R to reset Zoom Rotation", 40, 100, 10, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
