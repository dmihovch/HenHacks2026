#include "../include/platformer.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GRAVITY 0.6f
#define ACCELERATION 0.8f
#define FRICTION 0.85f
#define MAX_SPEED_X 9.0f
#define MAX_SPEED_Y 18.0f
#define JUMP_FORCE -14.0f

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 60

#define PLATFORM_COUNT 4
#define BALL_COUNT 6
#define SHOOTER_COUNT PLATFORM_COUNT
#define BULLET_POOL_SIZE 200   // effectively infinite

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
} Bullet;

typedef struct {
    Rectangle rect;
    float shootTimer;
} Shooter;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
} Ball;

void enterPlatformer()
{
    srand((unsigned int)time(NULL));

    // =====================
    // PLATFORMS (Longer)
    // =====================
    Rectangle platforms[PLATFORM_COUNT];
    for(int i = 0; i < PLATFORM_COUNT; i++)
    {
        platforms[i] = (Rectangle){
            100,
            HEIGHT - 100 - i * 250, //distance between each platform
            700,          // MUCH LONGER
            20
        };
    }

    // =====================
    // PLAYER (spawn on platform)
    // =====================
    Vector2 playerPos = {
        platforms[0].x + 60,
        platforms[0].y - PLAYER_HEIGHT
    };

    Vector2 playerVel = {0, 0};
    int jumpCount = 0;
    const int maxJumps = 2;

    // =====================
    // CAMERA
    // =====================
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    camera.offset = (Vector2){WIDTH/2.0f, HEIGHT/2.0f};
    camera.target = playerPos;

    // =====================
    // SHOOTERS (at platform ends)
    // =====================
    Shooter shooters[SHOOTER_COUNT];
    for(int i = 0; i < SHOOTER_COUNT; i++)
    {
        shooters[i].rect = (Rectangle){
            platforms[i].x + platforms[i].width - 40,
            platforms[i].y - 40,
            40,
            40
        };
        shooters[i].shootTimer = 0;
    }

    // =====================
    // BULLETS (large pool)
    // =====================
    Bullet bullets[BULLET_POOL_SIZE] = {0};

    // =====================
    // BALLS
    // =====================
    Ball balls[BALL_COUNT];
    for(int i = 0; i < BALL_COUNT; i++)
    {
        balls[i].position = (Vector2){
            300 + i * 600,
            HEIGHT - 150 - (i % 3) * 300
        };

        balls[i].velocity = (Vector2){
            (rand() % 3 + 2) * (rand() % 2 ? 1 : -1),
            0
        };

        balls[i].radius = 20;
    }

    while(!WindowShouldClose() && !IsKeyPressed(KEY_Q))
    {
        float delta = GetFrameTime();

        // =====================
        // INPUT
        // =====================
        if(IsKeyDown(KEY_A))
            playerVel.x -= ACCELERATION;

        if(IsKeyDown(KEY_D))
            playerVel.x += ACCELERATION;

        if(playerVel.x > MAX_SPEED_X) playerVel.x = MAX_SPEED_X;
        if(playerVel.x < -MAX_SPEED_X) playerVel.x = -MAX_SPEED_X;

        if(IsKeyPressed(KEY_SPACE) && jumpCount < maxJumps)
        {
            playerVel.y = JUMP_FORCE;
            jumpCount++;
        }

        // =====================
        // PHYSICS
        // =====================
        playerVel.y += GRAVITY;
        if(playerVel.y > MAX_SPEED_Y)
            playerVel.y = MAX_SPEED_Y;

        playerVel.x *= FRICTION;

        playerPos.x += playerVel.x;
        playerPos.y += playerVel.y;

        Rectangle playerRect = {
            playerPos.x,
            playerPos.y,
            PLAYER_WIDTH,
            PLAYER_HEIGHT
        };

        // =====================
        // PLATFORM COLLISION
        // =====================
        for(int i = 0; i < PLATFORM_COUNT; i++)
        {
            if(CheckCollisionRecs(playerRect, platforms[i]) &&
               playerVel.y >= 0 &&
               playerPos.y + PLAYER_HEIGHT <= platforms[i].y + 15)
            {
                playerPos.y = platforms[i].y - PLAYER_HEIGHT;
                playerVel.y = 0;
                jumpCount = 0;
            }
        }

        // =====================
        // RESPAWN IF FALL
        // =====================
        if(playerPos.y > HEIGHT + 200)
        {
            playerPos = (Vector2){
                platforms[0].x + 60,
                platforms[0].y - PLAYER_HEIGHT
            };
            playerVel = (Vector2){0,0};
            jumpCount = 0;
        }

        // =====================
        // SHOOTERS
        // =====================
        for(int i = 0; i < SHOOTER_COUNT; i++)
        {
            shooters[i].shootTimer += delta;

            if(shooters[i].shootTimer > 1.5f)
            {
                shooters[i].shootTimer = 0;

                for(int b = 0; b < BULLET_POOL_SIZE; b++)
                {
                    if(!bullets[b].active)
                    {
                        bullets[b].active = true;

                        bullets[b].position = (Vector2){
                            shooters[i].rect.x,
                            shooters[i].rect.y + 20
                        };

                        bullets[b].velocity = (Vector2){-7.0f, 0}; 
                        break;
                    }
                }
            }
        }

        // =====================
        // BULLET UPDATE
        // =====================
        float camLeft   = camera.target.x - WIDTH/2;
        float camRight  = camera.target.x + WIDTH/2;

        for(int i = 0; i < BULLET_POOL_SIZE; i++)
        {
            if(bullets[i].active)
            {
                bullets[i].position.x += bullets[i].velocity.x;

                Rectangle bulletRect = {
                    bullets[i].position.x,
                    bullets[i].position.y,
                    12,
                    5
                };

                // Collision with player
                if(CheckCollisionRecs(playerRect, bulletRect))
                {
                    playerPos = (Vector2){
                        platforms[0].x + 60,
                        platforms[0].y - PLAYER_HEIGHT
                    };
                    playerVel = (Vector2){0,0};
                    jumpCount = 0;
                    bullets[i].active = false;
                }

                // Deactivate if off camera
                if(bullets[i].position.x < camLeft - 50 ||
                   bullets[i].position.x > camRight + 50)
                {
                    bullets[i].active = false;
                }
            }
        }

        // =====================
        // BALLS
        // =====================
        for(int i = 0; i < BALL_COUNT; i++)
        {
            balls[i].position.x += balls[i].velocity.x;

            if(balls[i].position.x < 0 || balls[i].position.x > 3000)
                balls[i].velocity.x *= -1;

            if(CheckCollisionCircleRec(
                balls[i].position,
                balls[i].radius,
                playerRect))
            {
                playerPos = (Vector2){
                    platforms[0].x + 60,
                    platforms[0].y - PLAYER_HEIGHT
                };
                playerVel = (Vector2){0,0};
                jumpCount = 0;
            }
        }

        // =====================
        // CAMERA
        // =====================
        camera.target = (Vector2){
            playerPos.x + PLAYER_WIDTH/2,
            playerPos.y + PLAYER_HEIGHT/2
        };

        // =====================
        // DRAW
        // =====================
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        for(int i = 0; i < PLATFORM_COUNT; i++)
            DrawRectangleRec(platforms[i], DARKBROWN);

        for(int i = 0; i < SHOOTER_COUNT; i++)
            DrawRectangleRec(shooters[i].rect, RED);

        for(int i = 0; i < BULLET_POOL_SIZE; i++)
            if(bullets[i].active)
                DrawRectangle(
                    bullets[i].position.x,
                    bullets[i].position.y,
                    12, 5,
                    YELLOW
                );

        for(int i = 0; i < BALL_COUNT; i++)
            DrawCircleV(balls[i].position, balls[i].radius, BROWN);

        DrawRectangle(playerPos.x, playerPos.y,
                      PLAYER_WIDTH, PLAYER_HEIGHT, BLUE);

        EndMode2D();
        DrawFPS(10,10);
        EndDrawing();
    }
}