#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 60
#define PLAYER_SPEED 5
#define JUMP_FORCE -17
#define GRAVITY 0.8f

#define PLATFORM_COUNT 20

typedef struct {
    Rectangle rect;
    Rectangle dropZone;
} Platform;

void enterPlatformer(void)
{
    Vector2 playerPos = { 400, 850 - PLAYER_HEIGHT };
    Vector2 velocity = { 0, 0 };
    bool onGround = false;

    Camera2D camera = { 0 };
    camera.target = playerPos;
    camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Platform platforms[PLATFORM_COUNT];
    float spacing = 140;

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        float x = 200 + (i % 2) * 300;
        float y = 850 - i * spacing;

        platforms[i].rect = (Rectangle){
            x,
            y,
            500,
            20
        };

        platforms[i].dropZone = (Rectangle){
            x + 200,     // center section
            y,
            100,
            20
        };
    }

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q))
            break;

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
                    {
                        continue;
                    }

                    playerPos.y = platform.y - PLAYER_HEIGHT;
                    velocity.y = 0;
                    onGround = true;
                }
            }
        }

        camera.target = playerPos;

        BeginDrawing();
        ClearBackground(DARKBLUE);

        BeginMode2D(camera);

        for (int i = 0; i < PLATFORM_COUNT; i++)
        {
            DrawRectangleRec(platforms[i].rect, GRAY);
            DrawRectangleRec(platforms[i].dropZone, ORANGE);
        }

        DrawRectangle(playerPos.x, playerPos.y,
                      PLAYER_WIDTH, PLAYER_HEIGHT, RED);

        EndMode2D();

        DrawText("Press Q to return to Lobby", 20, 20, 20, WHITE);
        DrawText("Stand on orange area and press S to drop", 20, 50, 20, WHITE);

        EndDrawing();
    }
}