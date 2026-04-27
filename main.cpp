#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <raymath.h>
#include "rcamera.h"

#define MAX_COLUMNS 20
#define FPS 60
#define  WIDTH 800
#define  HEIGHT 450

// struct Paddle{};

int main(){
    //Initization
    constexpr int screenWidth{WIDTH};
    constexpr int screenHeight{HEIGHT};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    SetTargetFPS(FPS); //Detect window close button or ESC key
    DisableCursor();   // Limit cursor to relative movement inside the window

    //Setup player1 camera and screen
    Camera cameraPlayer1 = {0};
    cameraPlayer1.fovy = 45.0F;
    cameraPlayer1.up.y = 1.0F;
    cameraPlayer1.target.y = 1.0F;
    cameraPlayer1.position.z = -3.0F;
    cameraPlayer1.position.y = 1.0F;

    RenderTexture screenPlayer1 = LoadRenderTexture(screenWidth/2, screenHeight);

    //Setup player two camera and screen
    Camera cameraPlayer2 = {0};
    cameraPlayer2.fovy = 45.0F;
    cameraPlayer2.up.y = 1.0F;
    cameraPlayer2.target.y = 3.0F;
    cameraPlayer2.position.x = -3.0F;
    cameraPlayer2.position.y = 3.0F;

    RenderTexture screenPlayer2 = LoadRenderTexture(screenWidth/2, screenHeight);

    //Build a flipped rectangle the size of the split view to use for drawing later
    Rectangle splitScreenRect = {0.0F, 0.0F, (float)screenPlayer1.texture.width, (float)-screenPlayer1.texture.height};

    //Grid data
    int count{5};
    float spacing{4};

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        float offsetThisFrame = 10.0F*GetFrameTime();

        //move player1 forward and backwords (no turning)
        if(IsKeyDown(KEY_W)){
            cameraPlayer1.position.z += offsetThisFrame;
            cameraPlayer1.target.z += offsetThisFrame;
        }else if(IsKeyDown(KEY_S)){
            cameraPlayer1.position.z -= offsetThisFrame;
            cameraPlayer1.target.z -= offsetThisFrame;
        }
        //MOve Player2 forawrd and backwords (no turning)
        if(IsKeyDown(KEY_UP)){
            cameraPlayer2.position.x += offsetThisFrame;
            cameraPlayer2.target.x += offsetThisFrame;
        }else if(IsKeyDown(KEY_DOWN)){
            cameraPlayer2.position.x += offsetThisFrame;
            cameraPlayer2.target.x -= offsetThisFrame;
        }
        // ---------------------
        // Draw
        // ---------------------
            //draw player 1
            BeginTextureMode(screenPlayer1);
                ClearBackground(RAYWHITE);
                        BeginMode3D(cameraPlayer1);
                            //Draw scene: grid of cube trees on a plane to make a "world"
                            DrawPlane((Vector3){0, 0, 0}, Vector2{50, 50}, BEIGE);

                            for(float x = -count*spacing; x <= count*spacing; x += spacing){
                                for(float z = -count*spacing; z <= count*spacing; z += spacing){
                                    DrawCube((Vector3){x, 1.5F, z}, 1, 1, 1, LIME);
                                    DrawCube((Vector3){x, 1.5F, z}, 0.25F, 1, 0.25F, BROWN);
                                }
                            }
                        EndMode3D();

                    DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8F));
                    DrawText("PLAYER1: w/s to move", 10, 10, 20, MAROON);
            EndTextureMode();

            //Draw player 2 view to the render texture
            BeginTextureMode(screenPlayer2);
                ClearBackground(SKYBLUE);
                BeginMode3D(cameraPlayer2);
                    //Draw scene: grid of cube trees on a plane to make a world
                    DrawPlane((Vector3){0,0, 0}, (Vector2){50, 50} , BEIGE);
                    for(float x = -count*spacing; x <= count*spacing; x += spacing){
                        for(float z = -count*spacing; z <= count*spacing; z += spacing){
                            DrawCube((Vector3){x, 1.5F,z}, 1, 1, 1, LIME);
                            DrawCube((Vector3){x, 0.5F, z}, 0.25F, 1, 0.25F, BROWN);
                        }
                    }
                    //Draw a cube at each player's position
                    DrawCube(cameraPlayer1.position, 1, 1, 1, RED);
                    DrawCube(cameraPlayer2.position, 1, 1, 1, BLUE);
                EndMode3D();

                DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8F));
                DrawText("Player2: Up/Down to move", 10, 10, 20, DARKBLUE);
            EndTextureMode();

            //Draw both views render textures to the screen side by side
            BeginDrawing();
                ClearBackground(BLACK);

                DrawTextureRec(screenPlayer1.texture, splitScreenRect, (Vector2){0,0}, WHITE);
                DrawTextureRec(screenPlayer2.texture, splitScreenRect, (Vector2){screenWidth/2.0F,0}, WHITE);
                DrawRectangle((GetScreenWidth()/2)-2, 0, 4, GetScreenHeight(), LIGHTGRAY);
            EndDrawing();
    }
    //De-Initialization
    UnloadRenderTexture(screenPlayer1);
    UnloadRenderTexture(screenPlayer2);

    CloseWindow();

    return 0;
}
