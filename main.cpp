#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <fmt/base.h>
#include <fmt/chrono.h>


//Initization
// constexpr int screenWidth{800};
// constexpr int screenHeight{450};

#define screenWidth 800
#define screenHeight 450

struct Ball{
    Vector2 position{0.0F, 0.0F}; //Safe efault value
    float speed{120};
    float size{5};
    float dx{2};
    float dy{2};
};

struct Paddle{
    float WIDTH = 10.0F;
    float HEIGHT = 50.0F;
    float x = 15.0F;
    float y = screenHeight / 2;
    Rectangle leftPaddle = {x, y, WIDTH, HEIGHT};
    Rectangle rightPaddle = {screenWidth - 25, y, WIDTH, HEIGHT};
    int movePaddle{250};
};

int main(){

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60); //Detect window close button or ESC key
    Ball ball;
    ball.position = {.x=(float)screenWidth/ 2.0F, .y=(float)screenHeight /2.0F};
    Paddle* paddle = new Paddle();
    // auto paddle = std::make_unique<Paddle>();
    auto lastTime = std::chrono::system_clock::now();
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        // float deltaTime = elapsed.count();
        float deltaTime = GetFrameTime();
        // lastTime  = currentTime;

        //update the ball position
        // ball.dx change randomnly the ball when hit outside the wall
        ball.position.x += ball.dx * ball.speed * deltaTime;
        ball.position.y += ball.dy * ball.speed * deltaTime;



        if(ball.position.x + ball.size > screenWidth or ball.position.x + ball.size < 0){
            // ball.position.x +=ball.dx * ball.speed * deltaTime;
            ball.position.x = screenWidth / 2.0F;
            ball.position.y = screenHeight / 2.0F;
            ball.dx *= -1 ;
        }
        else if (ball.position.y + ball.size > screenHeight or ball.position.y + ball.size< 0){
            // ball.position.y +=ball.dy * ball.speed * deltaTime;
            // ball.position.y = screenHeight / 2;
            ball.dy *= -1 ;
        }

        // Check collision for paddles
        if(CheckCollisionCircleRec(ball.position,  ball.size, paddle->leftPaddle)){
            ball.dx *= -1; //reverse direction
            ball.position.x += ball.dx * ball.speed * deltaTime;
        }

        if(CheckCollisionCircleRec(ball.position, ball.size, paddle->rightPaddle)){
            ball.dx *= -1; //reverse direction
            ball.position.x += ball.dx * ball.speed * deltaTime;
        }

        //add input function for paddle
        if(IsKeyDown(KEY_S))
            paddle->leftPaddle.y += paddle->movePaddle * deltaTime;


        if(IsKeyDown(KEY_W))
            paddle->leftPaddle.y -= paddle->movePaddle * deltaTime ;

        if(IsKeyDown(KEY_UP))
            paddle->rightPaddle.y -= paddle->movePaddle * deltaTime;

        if(IsKeyDown(KEY_DOWN))
            paddle->rightPaddle.y += paddle->movePaddle * deltaTime;

        //prevent the paddle from go out y-direction
        if(paddle->leftPaddle.y < 0){
            // paddle->leftPaddle.y = 0;
            paddle->leftPaddle.y = std::max<float>(paddle->leftPaddle.y, 0);
        }

        if(paddle->leftPaddle.y + paddle->leftPaddle.height > screenHeight){
            // paddle->leftPaddle.y =  screenHeight - paddle->leftPaddle.height;
            paddle->leftPaddle.y = std::max<float>(screenHeight - paddle->leftPaddle.height, 0);
        }

        if(paddle->rightPaddle.y < 0){
            paddle->rightPaddle.y = 0;
        }

        if(paddle->rightPaddle.y + paddle->rightPaddle.height > screenHeight){
            paddle->rightPaddle.y = screenHeight - paddle->rightPaddle.height;
        }

        // paddle->leftPaddle.y = std::clamp(paddle->leftPaddle.y, 0.0F, screenHeight - paddle->leftPaddle.height);

        // paddle->rightPaddle.y = std::clamp(paddle->leftPaddle.y, 0.0F, screenHeight - paddle->leftPaddle.height);



        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(paddle->leftPaddle.x, paddle->leftPaddle.y, paddle->leftPaddle.width, paddle->leftPaddle.height, WHITE);
            DrawRectangle(paddle->rightPaddle.x,paddle->rightPaddle.y, paddle->rightPaddle.width, paddle->rightPaddle.height, WHITE);
            DrawCircle(ball.position.x, ball.position.y, ball.size, WHITE);
            DrawLine(screenWidth / 2, 0, screenWidth/2, screenHeight, WHITE);
        EndDrawing();
    }
    delete paddle;
    CloseWindow();

    return 0;
}
