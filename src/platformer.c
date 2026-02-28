#include "../include/platformer.h"

#define MAX_ENEMIES 3
#define MAX_BULLETS 20

typedef struct {
    Rectangle rect;
    float velY;
    int jumpCount;
    int maxJumps;
    Color color;
} Player;

typedef struct {
    Rectangle rect;
    float shootTimer;
} Enemy;

typedef struct {
    Rectangle rect;
    float speed;
    bool active;
} Bullet;

void ResetPlayer(Player* p, float x, float y)
{
    p->rect.x = x;
    p->rect.y = y;
    p->velY = 0;
    p->jumpCount = 0;
}

void enterPlatformer()
{
    float gravity = 0.6f;
    float jumpForce = -12;
    float moveSpeed = 5;

    int score = 10;

    // ===== PLAYERS =====
    Player p1 = {{100, 400, 40, 60}, 0, 0, 2, BLUE};
    Player p2 = {{200, 400, 40, 60}, 0, 0, 2, GREEN};

    // ===== GROUND =====
    Rectangle ground = {0, HEIGHT - 40, 3000, 40};

    // ===== ENEMIES =====
    Enemy enemies[MAX_ENEMIES] = {
        {{600, HEIGHT-100, 40, 60}, 0},
        {{1100, HEIGHT-100, 40, 60}, 0},
        {{1600, HEIGHT-100, 40, 60}, 0}
    };

    // ===== BULLETS =====
    Bullet bullets[MAX_BULLETS] = {0};

    Camera2D camera = {0};
    camera.offset = (Vector2){WIDTH/2.0f, HEIGHT/2.0f};
    camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q)) break;

        // ======================
        // PLAYER 1 CONTROLS
        // ======================
        if (IsKeyDown(KEY_A)) p1.rect.x -= moveSpeed;
        if (IsKeyDown(KEY_D)) p1.rect.x += moveSpeed;

        if (IsKeyPressed(KEY_SPACE) && p1.jumpCount < p1.maxJumps)
        {
            p1.velY = jumpForce;
            p1.jumpCount++;
        }

        // ======================
        // PLAYER 2 CONTROLS
        // ======================
        if (IsKeyDown(KEY_LEFT)) p2.rect.x -= moveSpeed;
        if (IsKeyDown(KEY_RIGHT)) p2.rect.x += moveSpeed;

        if (IsKeyPressed(KEY_UP) && p2.jumpCount < p2.maxJumps)
        {
            p2.velY = jumpForce;
            p2.jumpCount++;
        }

        // ======================
        // GRAVITY
        // ======================
        Player* players[2] = {&p1, &p2};

        for (int i = 0; i < 2; i++)
        {
            players[i]->velY += gravity;
            players[i]->rect.y += players[i]->velY;

            if (CheckCollisionRecs(players[i]->rect, ground))
            {
                players[i]->rect.y = ground.y - players[i]->rect.height;
                players[i]->velY = 0;
                players[i]->jumpCount = 0;
            }
        }

        // ======================
        // ENEMY SHOOTING
        // ======================
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            enemies[i].shootTimer += GetFrameTime();

            if (enemies[i].shootTimer > 2.0f)
            {
                enemies[i].shootTimer = 0;

                // Find free bullet
                for (int b = 0; b < MAX_BULLETS; b++)
                {
                    if (!bullets[b].active)
                    {
                        bullets[b].active = true;
                        bullets[b].rect = (Rectangle){
                            enemies[i].rect.x,
                            enemies[i].rect.y + 20,
                            15,
                            5
                        };
                        bullets[b].speed = -8;
                        break;
                    }
                }
            }
        }

        // ======================
        // BULLET UPDATE
        // ======================
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active) continue;

            bullets[i].rect.x += bullets[i].speed;

            // Offscreen deactivate
            if (bullets[i].rect.x < 0)
                bullets[i].active = false;

            // Check collision with players
            for (int p = 0; p < 2; p++)
            {
                if (CheckCollisionRecs(bullets[i].rect, players[p]->rect))
                {
                    bullets[i].active = false;
                    score--;
                }
            }
        }

        // ======================
        // CAMERA FOLLOW (P1)
        // ======================
        camera.target = (Vector2){
            p1.rect.x + p1.rect.width/2,
            p1.rect.y + p1.rect.height/2
        };

        // ======================
        // DRAW
        // ======================
        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(camera);

        DrawRectangleRec(ground, DARKBROWN);

        for (int i = 0; i < MAX_ENEMIES; i++)
            DrawRectangleRec(enemies[i].rect, MAROON);

        for (int i = 0; i < MAX_BULLETS; i++)
            if (bullets[i].active)
                DrawRectangleRec(bullets[i].rect, BLACK);

        DrawRectangleRec(p1.rect, p1.color);
        DrawRectangleRec(p2.rect, p2.color);

        EndMode2D();

        DrawText(TextFormat("Score: %d", score), 20, 20, 30, BLACK);
        DrawText("P1: WASD + SPACE | P2: ARROWS + RCTRL", 20, 60, 20, BLACK);
        DrawText("Q to Quit", 20, 90, 20, BLACK);

        DrawFPS(0, 0);

        EndDrawing();
    }
}