#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <fmt/chrono.h>

struct Ball{
    Vector2 position{(float)GetScreenWidth()/ 2, (float)GetScreenHeight() / 2};
    float speed{100};
    float size{5};
    float dx{2};
    float dy{2};
};

// struct Paddle{};

int main(){
    //Initization
    constexpr int screenWidth{800};
    constexpr int screenHeight{450};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60); //Detect window close button or ESC key
    Ball ball;
    auto lastTime = std::chrono::system_clock::now();
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        float deltaTime = elapsed.count();
        // float deltaTime = GetFrameTime() * ball.speed;
        lastTime  = currentTime;

        ball.position.x += ball.dx * ball.speed * deltaTime;
        ball.position.y += ball.dy * ball.speed * deltaTime;

        if(ball.position.x - ball.size > screenWidth or ball.position.x - ball.size < 0)
            ball.position.x = screenWidth / 2;
            // ball.dx *= -1 ;
        else if (ball.position.y - ball.size > screenHeight or ball.position.y - ball.size< 0)
            ball.position.y = screenHeight / 2;
            // ball.dy *= -1 ;

        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(BLACK);
            DrawLine( 15,  screenHeight / 2, 15,  screenHeight/2 + 50, WHITE);
            DrawLine( screenWidth - 15,  screenHeight / 2, screenWidth-15,  screenHeight/2 + 50, WHITE);
            DrawCircle(ball.position.x, ball.position.y, ball.size, WHITE);
            DrawLine(screenWidth / 2, 0, screenWidth/2, screenHeight, WHITE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
