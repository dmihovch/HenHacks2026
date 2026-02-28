#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 60
#define PLAYER_SPEED 6
#define JUMP_FORCE -17
#define GRAVITY 0.8f

#define PLATFORM_COUNT 30
#define PLATFORM_WIDTH 500
#define PLATFORM_HEIGHT 20
#define VERTICAL_SPACING 140

#define MAX_BULLETS 250
#define BULLET_SPEED 8
#define SHOOT_INTERVAL 120

typedef struct {
    Rectangle rect;
    Rectangle dropZone;
} Platform;

typedef struct {
    Rectangle rect;
    int direction;
    int shootTimer;
    bool active;
} Shooter;

typedef struct {
    Vector2 pos;
    int direction;
    bool active;
} Bullet;

void GeneratePlatforms(Platform platforms[])
{
    float baseX = 400;
    float baseY = 850;

    platforms[0].rect = (Rectangle){
        baseX, baseY,
        PLATFORM_WIDTH, PLATFORM_HEIGHT
    };

    platforms[0].dropZone = (Rectangle){0,0,0,0};

    float prevX = baseX;
    float prevY = baseY;
    float prevWidth = PLATFORM_WIDTH;

    for (int i = 1; i < PLATFORM_COUNT; i++)
    {
        float newWidth = GetRandomValue(260, 500);
        float overlap = GetRandomValue(80, 160);
        float minSeparation = 450;

        int direction = GetRandomValue(0,1) == 0 ? -1 : 1;
        float newX;

        if (direction == 1)
        {
            newX = prevX + prevWidth - overlap;
            if (fabs(newX - prevX) < minSeparation)
                newX = prevX + minSeparation;
        }
        else
        {
            newX = prevX - newWidth + overlap;
            if (fabs(newX - prevX) < minSeparation)
                newX = prevX - minSeparation;
        }

        if (newX < 100) newX = 100;
        if (newX + newWidth > 1180) newX = 1180 - newWidth;

        float newY = prevY - VERTICAL_SPACING;

        platforms[i].rect = (Rectangle){
            newX, newY,
            newWidth, PLATFORM_HEIGHT
        };

        platforms[i].dropZone = (Rectangle){
            newX + newWidth * 0.4f,
            newY,
            newWidth * 0.2f,
            PLATFORM_HEIGHT
        };

        prevX = newX;
        prevY = newY;
        prevWidth = newWidth;
    }
}

void RegenerateShooters(Platform platforms[], Shooter shooters[])
{
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        shooters[i].active = false;
        shooters[i].shootTimer = 0;

        if (i == 0) continue;

        float spawnChance = (float)i / PLATFORM_COUNT;

        if (GetRandomValue(0,100) < spawnChance * 100)
        {
            shooters[i].active = true;

            Rectangle p = platforms[i].rect;
            bool spawnLeft = GetRandomValue(0,1) == 0;

            if (spawnLeft)
            {
                shooters[i].rect = (Rectangle){
                    p.x - 20,
                    p.y - 40,
                    20,
                    40
                };
                shooters[i].direction = 1;
            }
            else
            {
                shooters[i].rect = (Rectangle){
                    p.x + p.width,
                    p.y - 40,
                    20,
                    40
                };
                shooters[i].direction = -1;
            }
        }
    }
}

void enterPlatformer(void)
{
    srand(time(NULL));

    Platform platforms[PLATFORM_COUNT];
    Shooter shooters[PLATFORM_COUNT];
    Bullet bullets[MAX_BULLETS];

    GeneratePlatforms(platforms);
    RegenerateShooters(platforms, shooters);

    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    Vector2 playerPos = {
        platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2,
        platforms[0].rect.y - PLAYER_HEIGHT
    };

    Vector2 velocity = {0,0};
    bool onGround = true;

    Camera2D camera = {0};
    camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q))
            break;

        if (IsKeyPressed(KEY_R))
        {
            GeneratePlatforms(platforms);
            RegenerateShooters(platforms, shooters);

            for (int i = 0; i < MAX_BULLETS; i++)
                bullets[i].active = false;

            playerPos.x = platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2;
            playerPos.y = platforms[0].rect.y - PLAYER_HEIGHT;
            velocity = (Vector2){0,0};
            onGround = true;
        }

        float previousY = playerPos.y;

        if (IsKeyDown(KEY_A)) playerPos.x -= PLAYER_SPEED;
        if (IsKeyDown(KEY_D)) playerPos.x += PLAYER_SPEED;

        if (IsKeyPressed(KEY_SPACE) && onGround)
        {
            velocity.y = JUMP_FORCE;
            onGround = false;
        }

        velocity.y += GRAVITY;
        playerPos.y += velocity.y;
        onGround = false;

        for (int i = 0; i < PLATFORM_COUNT; i++)
        {
            Rectangle platform = platforms[i].rect;
            Rectangle dropZone = platforms[i].dropZone;

            if (velocity.y > 0)
            {
                float previousBottom = previousY + PLAYER_HEIGHT;
                float currentBottom = playerPos.y + PLAYER_HEIGHT;

                bool crossingPlatform =
                    previousBottom <= platform.y &&
                    currentBottom >= platform.y &&
                    playerPos.x + PLAYER_WIDTH > platform.x &&
                    playerPos.x < platform.x + platform.width;

                if (crossingPlatform)
                {
                    Rectangle playerRect = {
                        playerPos.x,
                        playerPos.y,
                        PLAYER_WIDTH,
                        PLAYER_HEIGHT
                    };

                    bool inDropZone = CheckCollisionRecs(playerRect, dropZone);

                    if (inDropZone && IsKeyDown(KEY_S) && i != 0)
                        continue;

                    playerPos.y = platform.y - PLAYER_HEIGHT;
                    velocity.y = 0;
                    onGround = true;
                }
            }
        }

        // Shooter update (STRONGER FIX)
        for (int i = 0; i < PLATFORM_COUNT; i++)
        {
            if (!shooters[i].active) continue;

            shooters[i].shootTimer++;

            if (shooters[i].shootTimer >= SHOOT_INTERVAL)
            {
                shooters[i].shootTimer = 0;

                bool fired = false;

                for (int b = 0; b < MAX_BULLETS; b++)
                {
                    if (!bullets[b].active)
                    {
                        bullets[b].active = true;
                        bullets[b].pos = (Vector2){
                            shooters[i].rect.x + shooters[i].rect.width/2,
                            shooters[i].rect.y + shooters[i].rect.height/2
                        };
                        bullets[b].direction = shooters[i].direction;
                        fired = true;
                        break;
                    }
                }

                if (!fired)
                {
                    bullets[0].active = true;
                    bullets[0].pos = (Vector2){
                        shooters[i].rect.x + shooters[i].rect.width/2,
                        shooters[i].rect.y + shooters[i].rect.height/2
                    };
                    bullets[0].direction = shooters[i].direction;
                }
            }
        }

        // Bullet update
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active) continue;

            bullets[i].pos.x += BULLET_SPEED * bullets[i].direction;

            if (bullets[i].pos.x < -2000 || bullets[i].pos.x > 4000)
            {
                bullets[i].active = false;
                continue;
            }

            Rectangle bulletRect = {
                bullets[i].pos.x - 5,
                bullets[i].pos.y - 5,
                10, 10
            };

            Rectangle playerRect = {
                playerPos.x,
                playerPos.y,
                PLAYER_WIDTH,
                PLAYER_HEIGHT
            };

            if (CheckCollisionRecs(bulletRect, playerRect))
            {
                GeneratePlatforms(platforms);
                RegenerateShooters(platforms, shooters);

                for (int j = 0; j < MAX_BULLETS; j++)
                    bullets[j].active = false;

                playerPos.x = platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2;
                playerPos.y = platforms[0].rect.y - PLAYER_HEIGHT;
                velocity = (Vector2){0,0};
                onGround = true;
                break;
            }
        }

        camera.target = playerPos;

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawFPS(100,0);

        BeginMode2D(camera);

        for (int i = 0; i < PLATFORM_COUNT; i++)
        {
            if (i == 0)
                DrawRectangleRec(platforms[i].rect, BLACK);
            else if (i == PLATFORM_COUNT - 1)
                DrawRectangleRec(platforms[i].rect, GOLD);
            else
                DrawRectangleRec(platforms[i].rect, GRAY);

            if (i != 0)
                DrawRectangleRec(platforms[i].dropZone, ORANGE);
        }

        for (int i = 0; i < PLATFORM_COUNT; i++)
            if (shooters[i].active)
                DrawRectangleRec(shooters[i].rect, PURPLE);

        for (int i = 0; i < MAX_BULLETS; i++)
            if (bullets[i].active)
                DrawCircle(bullets[i].pos.x, bullets[i].pos.y, 5, YELLOW);

        DrawRectangle(playerPos.x, playerPos.y,
                      PLAYER_WIDTH, PLAYER_HEIGHT, RED);

        EndMode2D();

        DrawText("Q: Lobby | R: Respawn | S: Drop Down", 20, 20, 20, WHITE);

        EndDrawing();
    }
}