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

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60); //Detect window close button or ESC key

    //Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = {0};
    camera.position  = (Vector3){0.0F, 2.0F, 4.0F}; //Camera position
    camera.target = (Vector3){0.0F, 2.0F, 0.0F}; //Camera looking at point
    camera.up = (Vector3){0.0F, 1.0F, 0.0F}; // Camera up vector (rotation toward target)
    camera.fovy = 60.0F; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    //Generates some random columns
    float heights[MAX_COLUMNS] = {0};
    Vector3 positions[MAX_COLUMNS] = {0};
    Color colors[MAX_COLUMNS] = {0};

    for(int i{}; i < MAX_COLUMNS; ++i){
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){(float)GetRandomValue(-15, 15), heights[i]/2.0F, (float)GetRandomValue(-15, 15)};
        colors[i] = (Color){static_cast<unsigned char>(GetRandomValue(20, 255)), static_cast<unsigned char>(GetRandomValue(10, 55)), 30, 255};
    }
    DisableCursor();   // Limit cursor to relative movement inside the window
    //main game loop
    while (!WindowShouldClose()) {

        // float deltaTime = GetFrameTime() * ball.speed;
        if(IsKeyPressed(KEY_ONE)){
            cameraMode  = CAMERA_FREE;
            camera.up   = (Vector3){0.0F, 1.0F, 0.0F}; // Reset roll
        }
        if(IsKeyPressed(KEY_TWO)){
            cameraMode  = CAMERA_FIRST_PERSON;
            camera.up   = (Vector3){0.0F, 1.0F, 0.0F}; //Rest roll
        }
        if(IsKeyPressed(KEY_THREE)){
            cameraMode = CAMERA_THIRD_PERSON;
            camera.up = (Vector3){0.0F, 1.0F, 0.0F};// rest roll
        }
        if(IsKeyPressed(KEY_P)){
            cameraMode = CAMERA_ORBITAL;
            camera.up = (Vector3){0.0F, 1.0F, 0.0F}; //Rest roll
        }
        //Switch camera projection
        if(IsKeyPressed(KEY_P)){
            if(camera.projection == CAMERA_PERSPECTIVE){
                //Create isomeric view
                cameraMode = CAMERA_THIRD_PERSON;
                //Note: The target distance is related to the render distance in the orthographic projection
                camera.position = (Vector3){0.0F, 2.0F, -100.0F};
                camera.target = (Vector3){0.0F, 2.0F, 0.0F};
                camera.up = (Vector3){0.0F, 1.0F, 0.0F};
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0F;  // near plane width in CAMERA_ORTHOGRAPHIC
                CameraYaw(&camera, -135 * DEG2RAD, true);
                CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
            }else if(camera.projection == CAMERA_ORTHOGRAPHIC){
                //Reset to default view
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){0.0F, 2.0F, 10.0F};
                camera.target = (Vector3){0.0F, 2.0F, 0.0F};
                camera.up = (Vector3){0.0F, 1.0F, 0.0F};
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0F;
            }
        }
        //Update
        // ---------------
        // TODO: update your variable here
        //Update camera compture movment internally depending on the camera mode
        //Some default standard keyboard/mouse inputs ar hardcode to simplify use
        //For advanced camera controls, it's recommended to compute camera movement manually
        UpdateCamera(&camera, cameraMode);  // update the camera
        // --------------------
        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
                BeginMode3D(camera);
                    DrawPlane((Vector3){0.0F, 0.0F, 0.0F}, (Vector2){32.0F, 32.0F}, LIGHTGRAY); //Draw Ground
                    DrawCube((Vector3){-16.0F, 2.5F, 0.0F}, 1.0F, 5.0F, 32.0F, BLUE);//Draw a blue wall
                    DrawCube((Vector3){16.0F, 2.5F, 0.0F}, 1.0F, 5.0F, 32.0F, LIME);//Draw a blue wall
                    DrawCube((Vector3){-16.0F, 2.5F, 0.0F}, 32.0F, 5.0F, 1.0F, GOLD);//Draw a blue wall

                    //Draw some cubes around
                    for(int i{}; i < MAX_COLUMNS; ++i){
                        DrawCube(positions[i], 2.0F, heights[i], 2.0F, colors[i]);
                        DrawCubeWires(positions[i], 2.0F, heights[i], 2.0F,  MAROON);
                    }
                    //Draw player cube
                    if(cameraMode == CAMERA_THIRD_PERSON){
                        DrawCube(camera.target, 0.5F, 0.5F, 0.5F, PURPLE);
                        DrawCubeWires(camera.target, 0.5F, 0.5F, 0.5F, DARKPURPLE);
                    }
                EndMode3D();

                //draw info boxes
                DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5F));
                DrawRectangleLines(5, 5, 330, 100, BLUE);

                DrawText("camera controls:", 15 , 15 , 10, BLACK);
                DrawText("- Move keys: W,A,S,D, Space, Left-Ctrl", 15 , 30 , 10, BLACK);
                DrawText("- Look around: arrow keys or mouse", 15 , 60, 10, BLACK);
                DrawText("- Camera mode keys: 1, 2, 3, 4", 15 , 45 , 10, BLACK);
                DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15 , 75 , 10, BLACK);
                DrawText("- Camera projection key: P", 15 , 90 , 10, BLACK);

                DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5F));
                DrawRectangleLines(600, 5, 195, 100, BLUE);

                DrawText("Camera status: ", 610, 15, 10, BLACK);
                DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                    (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON":
                    (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                    (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
                DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 45, 10, BLACK);
                DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 60, 10, BLACK);
                DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
