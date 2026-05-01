#include <chrono>
#include <cmath>
#include <fmt/base.h>
#include <raylib.h>
#include <fmt/chrono.h>

#define FPS 60
#define WIDTH 800
#define HEIGHT 450

enum AnimationType{
    REPEATING = 1,
    ONESHOT = 2,
};

enum Direction{
    LEFT = -1,
    RIGHT = 1,
};

struct Animation{
    int first;
    int last;
    int cur;
    int step;
    float speed;
    float duration_left;
    AnimationType type;
};

void animation_update(Animation* self){
    float dt = GetFrameTime();
    self->duration_left -= dt;
    // fmt::print("duration_left: {}\n", self->duration_left);
    if(self->duration_left <= 0.0){
        // fmt::print("less than 0 duration_left: {}\n", self->duration_left);
        self->duration_left = self->speed;
        self->cur = self->step;
        if(self->cur > self->last){
            switch(self->type){
                case REPEATING:
                    self->cur = self->first;
                    break;
                case ONESHOT:
                    self->cur = self->last;
                    break;
            }
        }else if(self->cur < self->first){
            switch(self->type){
                case REPEATING:
                    self->cur = self->last;
                    break;
                case ONESHOT:
                    self->cur = self->first;
                    break;
            }
        }
    }
}
//convert index to coordinate
Rectangle animation_frame(Animation* self, int num_per_row){
    int x = (self->cur % num_per_row) * 16.0; // with multiply we convert to pixel not tail
    int y = (self->cur / num_per_row) * 16.0;  // with multiply we convert to pixel not tail
    return (Rectangle){(float)x, (float)y, 16.0, 16.0};
};

int main(){
    //Initization
    constexpr int screenWidth{WIDTH};
    constexpr int screenHeight{HEIGHT};
    InitWindow(screenWidth, screenHeight, "raylib animation");


    Texture2D player_idle_texture = LoadTexture("asset/herochar sprites(new)/herochar_idle_anim_strip_4.png");
    Animation anim = (Animation){
        .first = 0,
        .last = 3,
        .cur = 0,
        .step = 1,
        .speed = 0.1,
        .duration_left = 0.1,
        .type = REPEATING,
    };

    Animation anim2 = (Animation){
           .first = 0,
           .last = 3,
           .cur = 3,
           .step = -1,
           .speed = 0.1,
           .duration_left = 0.1,
           .type = REPEATING,
       };

    Direction player_direction = LEFT;

    SetTargetFPS(FPS); //Detect window close button or ESC key
    //main game loop
    while (!WindowShouldClose()) {
        //Update
        // ---------------
        // TODO: update your variable here
        // --------------------
        if(IsKeyPressed(KEY_SPACE))
            anim.cur = anim.first;

        if(IsKeyPressed(KEY_A))
            player_direction = LEFT;

        if(IsKeyPressed(KEY_D))
             player_direction = RIGHT;

        animation_update(&anim);
        animation_update(&anim2);
        // Draw
        // ---------------------
        BeginDrawing();
            ClearBackground(SKYBLUE);
            // DrawTexturePro(player_idle_texture,
            //         {0, 0,static_cast<float>(player_idle_texture.width),
            //         static_cast<float>(player_idle_texture.height)},
            //         {10, 10, 100, 100},
            //       {0, 0}, 0.0F, WHITE);

            Rectangle player_frame = animation_frame(&anim, 4);
            player_frame.width *= player_direction;

            DrawTexturePro(player_idle_texture,
                          player_frame,
                          {10, 10, 100, 100},
                        {0, 0}, 0.0F, WHITE);
            DrawTexturePro(player_idle_texture,
                                    animation_frame(&anim2, 4) ,
                                     {130, 10, 100, 100},
                                   {0, 0}, 0.0F, WHITE);
        EndDrawing();
    }
    UnloadTexture(player_idle_texture);
    CloseWindow();

    return 0;
}
