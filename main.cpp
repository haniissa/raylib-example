#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <fmt/format.h>
#include <raylib.h>
#include <fmt/chrono.h>
#include <stdlib.h>
#include <map>

#define FPS 60
#define WIDTH 800
#define HEIGHT 450




//convert index to coordinate

int main(){
    //Initization
    constexpr int screenWidth{WIDTH};
    constexpr int screenHeight{HEIGHT};
    InitWindow(screenWidth, screenHeight, "raylib animation");

    SetTargetFPS(FPS); //Detect window close button or ESC key

    //We generate a checked image for texturing WALLS
    Image checked = GenImageChecked(2, 2, 1, 1, WHITE, DARKGRAY);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);

    Model model = {0};
    model = LoadModelFromMesh(GenMeshCube(1.0F, 1.0F, 1.0F));

    //Set checked texture as default diffuse component for all models material
    model.materials[0].maps[MAP_DIFFUSE].texture = texture;

    //WALKABLE
    Image checked2 = GenImageChecked(2,2, 1, 1, (Color){100, 0,0, 255}, RED);
    Texture2D texture2 = LoadTextureFromImage(checked2);
    UnloadImage(checked2);

    Model model2 = {0};
    model2 = LoadModelFromMesh(GenMeshCube(1.0F, 1.0F, 1.0F));

    //Set checked texture as default diffuse component for all models material
    model2.materials[0].maps[MAP_DIFFUSE].texture = texture2;
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------

        // Draw
        // ---------------------
        BeginDrawing();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
