#include "raylib.h"
#include <stdlib.h>
#include <time.h>

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

// #define BASE_MAX_HORIZONTAL_SHIFT 550
// #define MAX_ALLOWED_SHIFT 1000
// #define MIN_HORIZONTAL_SHIFT 500

typedef struct {
    Rectangle rect;
    Rectangle dropZone;
} Platform;

void GeneratePlatforms(Platform platforms[])
{
    float baseX = 400;
    float baseY = 850;

    // Bottom platform (fixed)
    platforms[0].rect = (Rectangle){
        baseX,
        baseY,
        PLATFORM_WIDTH,
        PLATFORM_HEIGHT
    };

    platforms[0].dropZone = (Rectangle){ 0, 0, 0, 0 };

    float prevX = baseX;
    float prevY = baseY;
    float prevWidth = PLATFORM_WIDTH;

    for (int i = 1; i < PLATFORM_COUNT; i++)
    {
        float newWidth = GetRandomValue(260, 500);

        // Controls how much the platforms overlap horizontally
        // Lower overlap = harder jump
        float overlap = GetRandomValue(80, 160);

        // Enforce strong horizontal displacement
        float minSeparation = 450;

        int direction = GetRandomValue(0, 1) == 0 ? -1 : 1;

        float newX;

        if (direction == 1)
        {
            // Spawn to the right
            newX = prevX + prevWidth - overlap;

            // Ensure not stacking too close
            if (fabs(newX - prevX) < minSeparation)
                newX = prevX + minSeparation;
        }
        else
        {
            // Spawn to the left
            newX = prevX - newWidth + overlap;

            if (fabs(newX - prevX) < minSeparation)
                newX = prevX - minSeparation;
        }

        // Clamp inside bounds
        if (newX < 100)
            newX = 100;

        if (newX + newWidth > 1180)
            newX = 1180 - newWidth;

        float newY = prevY - VERTICAL_SPACING;

        platforms[i].rect = (Rectangle){
            newX,
            newY,
            newWidth,
            PLATFORM_HEIGHT
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

void enterPlatformer(void)
{
    srand(time(NULL));

    Platform platforms[PLATFORM_COUNT];
    GeneratePlatforms(platforms);

    Vector2 playerPos = {
        platforms[0].rect.x + PLATFORM_WIDTH / 2 - PLAYER_WIDTH / 2,
        platforms[0].rect.y - PLAYER_HEIGHT
    };

    Vector2 velocity = { 0, 0 };
    bool onGround = true;

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q))
            break;

        if (IsKeyPressed(KEY_R))
        {
            GeneratePlatforms(platforms);

            playerPos.x = platforms[0].rect.x + PLATFORM_WIDTH / 2 - PLAYER_WIDTH / 2;
            playerPos.y = platforms[0].rect.y - PLAYER_HEIGHT;

            velocity = (Vector2){ 0, 0 };
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

                    if (inDropZone && IsKeyDown(KEY_S))
                        continue;

                    playerPos.y = platform.y - PLAYER_HEIGHT;
                    velocity.y = 0;
                    onGround = true;
                }
            }
        }

        camera.target = playerPos;

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawFPS(100, 0);


        BeginMode2D(camera);

        for (int i = 0; i < PLATFORM_COUNT; i++)
        {
            DrawRectangleRec(platforms[i].rect, GRAY);
            DrawRectangleRec(platforms[i].dropZone, ORANGE);
        }

        DrawRectangle(playerPos.x, playerPos.y,
                      PLAYER_WIDTH,
                      PLAYER_HEIGHT,
                      RED);

        EndMode2D();

        DrawText("Q: Lobby | R: Respawn | S: Drop Down", 20, 20, 20, WHITE);

        EndDrawing();
    }
}