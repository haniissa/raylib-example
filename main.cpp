#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <fmt/format.h>
#include <raylib.h>
#include <fmt/chrono.h>
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

    //We set up our 3 dimensional array here.it is a int array.
    int map[10][10][10] = {0}; // the = {} means all the starting values are set to 0.

    //Here we put some values inside the array.
    map[0][0][0]=  10;
    map[9][0][0]=  20; // With a setup size of 10 the maximum slot to be used is 9!
    map[0][0][9]=  30;
    map[9][0][9]=  40;

    //Boolean map. Only true or false
    bool boolmap[10][10][10] = {false};
    boolmap[1][1][2] = true;

    SetTargetFPS(FPS); //Detect window close button or ESC key
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------

        // Draw
        // ---------------------
        BeginDrawing();
            //Here we show the contents of the 3 dimensional array on the screen        //Here we show the contents of the 3 dimensional array on the screen.
            ClearBackground(RAYWHITE);
            DrawText(TextFormat("map[0][0][0]: %i", map[0][0][0]), 100, 100, 20, BLACK);
            DrawText(TextFormat("map[9][0][0]: %i", map[0][0][0]), 100, 120, 20, BLACK);
            DrawText(TextFormat("map[0][0][9]: %i", map[0][0][0]), 100, 140, 20, BLACK);
            DrawText(TextFormat("map[0][0][9]: %i", map[0][0][0]), 100, 160, 20, BLACK);
            DrawText(TextFormat("default = {0} slot of map[1][1][1]: %i", map[1][1][1]), 100, 180, 20, BLACK);

            //Check if boolmap[1][1][2] = true or false
            if(boolmap[1][1][2] == true)
                DrawText("boolmap[1][1][2] set to true.", 100, 200, 20, BLACK);
            else
                DrawText("boolmap[1][1][2] set to false.", 100, 200, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
