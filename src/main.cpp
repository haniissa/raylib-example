#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <fmt/core.h> // base give print and println core.h give a  format
#include "Game.h"
#include "VulkanCompute.h"

using namespace fmt;

//Initization
constexpr int screenWidth{800};
constexpr int screenHeight{450};
constexpr float ARENA_SIZE = 30.0F;
constexpr float HALF_ARENA = ARENA_SIZE / 2.0F;



int main(){

    InitWindow(screenWidth, screenHeight, "raylib #D GLSL Shader Game");
    SetTargetFPS(60); //Detect window close button or ESC key


    //--------Vulkan Compute (separate from raylib's OpenGL)
    VulkanCompute vkComp;
    vkComp.Init();

    //Print startup message using fmt:
    print("Game initialization complete. Arena size: {:.1F}\n", ARENA_SIZE);

    //2. Lood Shaders
    // Shader paths are relative to the executable output directory
    Shader defaultShader = LoadShader("shaders/shader.vert", "shaders/shader.frag");

    //Get the location of our custom "time" uniform in the shader
    int timeLoc = GetShaderLocation(defaultShader, "time");
    fmt::print("[Shader] time uniform @ location {}\n", timeLoc);


    //3. Initialize Camera
    Camera3D camera = {0};
    camera.position = (Vector3){0.0F, 15.0F, 35.0F}; //Camera starting position
    camera.target = (Vector3){0.0F, 0.0F, 0.0F}; // Look at the point
    camera.up = (Vector3){0.0F, 1.0F, 0.0F}; // Field-of-view up-direction
    camera.fovy = 45.0F;
    camera.projection = CAMERA_PERSPECTIVE;

    //disable default cursor behavior so camera arbits cleanly
    DisableCursor();

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    int score{};

    //----------Game state ----------------
    Game game(ARENA_SIZE);
    game.spawnEnemies(8);

    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        float dt = GetFrameTime();
        auto timeVal = (float)GetTime();
        //Update the custom shader uniform
        SetShaderValue(defaultShader, timeLoc, &timeVal, SHADER_UNIFORM_FLOAT);

        //5. Camera Control & Movement
        //update the camera's target to follow the player
        camera.target = game.GetPlayer().position;
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        //Calculate movement directions relative to camera viewpoint
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        forward.y = 0.0F; //Keep movement Locaked to horizontal plane

        Vector3 right = Vector3Normalize({-forward.z, 0.0F, forward.x}); // Orthogonal vector

        Vector3 moveDir = {0.0F, 0.0F, 0.0F};

        //Keyboard Inputs: WASD Movement
        //normalized moveDir so no moving faster when press (w+d)
        // so speed will be normal No vector normalization**: `forward` and `right` aren't
        // normalized via `Vector3Normalize`. Without it, diagonal movement
        // (W+D) will be ~1.41× faster than cardinal movement.
        moveDir = Vector3Normalize(moveDir);
        if(IsKeyDown(KEY_W)) moveDir = Vector3Add(moveDir, forward);
        if(IsKeyDown(KEY_S)) moveDir = Vector3Subtract(moveDir, forward);
        if(IsKeyDown(KEY_A)) moveDir = Vector3Subtract(moveDir, right);
        if(IsKeyDown(KEY_D)) moveDir = Vector3Add(moveDir, right);

        bool shootPresed = IsKeyPressed(KEY_SPACE);
        game.Update(dt, moveDir, camera, shootPresed);

        //Vulkan compute tick
        vkComp.Dispatch(timeVal);

        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground({10, 10, 20, 255});

            BeginMode3D(camera);
           		DrawGrid(20, 2.0F);
             	// arena wireframe
             	BeginShaderMode(defaultShader);
              		game.Draw();
                EndShaderMode();
            EndMode3D();

            //2D HUD
            DrawFPS(10, 10);
            DrawText(fmt::format("Score: {} Lives: {}", game.GetScore(),
            game.GetPlayer().lives).c_str(), 10, 40, 22, RAYWHITE);
            DrawText("[WASD] Move  [SPACE] Shoot  [ESC] Exit", 10, screenHeight - 30, 16, ColorAlpha(RAYWHITE, 0.6F));
        EndDrawing();
    }
    vkComp.Cleanup();
    UnloadShader(defaultShader);
    CloseWindow();
    return 0;
}
