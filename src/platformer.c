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

// generate platforms
void GeneratePlatforms(Platform platforms[])
{
    float baseX = 400;
    float baseY = 850;

    platforms[0].rect = (Rectangle){ baseX, baseY, PLATFORM_WIDTH, PLATFORM_HEIGHT };
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

        if (direction == 1) {
            newX = prevX + prevWidth - overlap;
            if (fabs(newX - prevX) < minSeparation) newX = prevX + minSeparation;
        } else {
            newX = prevX - newWidth + overlap;
            if (fabs(newX - prevX) < minSeparation) newX = prevX - minSeparation;
        }

        if (newX < 100) newX = 100;
        if (newX + newWidth > 1180) newX = 1180 - newWidth;

        float newY = prevY - VERTICAL_SPACING;

        platforms[i].rect = (Rectangle){ newX, newY, newWidth, PLATFORM_HEIGHT };
        platforms[i].dropZone = (Rectangle){ newX + newWidth*0.4f, newY, newWidth*0.2f, PLATFORM_HEIGHT };

        prevX = newX;
        prevY = newY;
        prevWidth = newWidth;
    }
}

// generate shooters based on platforms
void RegenerateShooters(Platform platforms[], Shooter shooters[])
{
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        shooters[i].active = false;
        shooters[i].shootTimer = 0;

        if (i == 0) continue;

        float spawnChance = (float)i / PLATFORM_COUNT;
        if (GetRandomValue(0,100) < spawnChance*100)
        {
            shooters[i].active = true;

            Rectangle p = platforms[i].rect;
            bool spawnLeft = GetRandomValue(0,1) == 0;

            if (spawnLeft) {
                shooters[i].rect = (Rectangle){ p.x - 20, p.y - 40, 20, 40 };
                shooters[i].direction = 1;
            } else {
                shooters[i].rect = (Rectangle){ p.x + p.width, p.y - 40, 20, 40 };
                shooters[i].direction = -1;
            }
        }
    }
}

// when enter platformer..
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

    Vector2 player1Pos = { platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2,
                            platforms[0].rect.y - PLAYER_HEIGHT };
    Vector2 player2Pos = { platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2,
                            platforms[0].rect.y - PLAYER_HEIGHT };

    Vector2 player1Vel = {0,0};
    Vector2 player2Vel = {0,0};
    bool player1OnGround = true;
    bool player2OnGround = true;

    int score = 0;
    Camera2D camera = {0};
    camera.offset = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
    camera.zoom = 1.0f;

    float winTextTimer = 0.0f; // for floating text effect
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q)) break;

        // respawn
        if (IsKeyPressed(KEY_R))
        {
            GeneratePlatforms(platforms);
            RegenerateShooters(platforms, shooters);

            for (int i = 0; i < MAX_BULLETS; i++)
                bullets[i].active = false;

            player1Pos.x = platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2;
            player1Pos.y = platforms[0].rect.y - PLAYER_HEIGHT;
            player2Pos = player1Pos;

            player1Vel = (Vector2){0,0};
            player2Vel = (Vector2){0,0};
            player1OnGround = player2OnGround = true;

            score = 0;
            winTextTimer = 0.0f;
        }

        Rectangle topPlatform = platforms[PLATFORM_COUNT-1].rect;

        // Define player rectangles early for collision checks
        Rectangle player1Rect = { player1Pos.x, player1Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
        Rectangle player2Rect = { player2Pos.x, player2Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };

        // --- WIN CHECK ---
        bool player1OnTop = (player1OnGround && player1Pos.y + PLAYER_HEIGHT == topPlatform.y);
        bool player2OnTop = (player2OnGround && player2Pos.y + PLAYER_HEIGHT == topPlatform.y);
        bool win = player1OnTop && player2OnTop;

        if (!win)
        {
            // --- NORMAL GAME UPDATES ---
            float prevY1 = player1Pos.y;
            float prevY2 = player2Pos.y;

            // player controls
            if (IsKeyDown(KEY_A)) player1Pos.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_D)) player1Pos.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_LEFT)) player2Pos.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_RIGHT)) player2Pos.x += PLAYER_SPEED;

            if (IsKeyPressed(KEY_W) && player1OnGround) { player1Vel.y = JUMP_FORCE; player1OnGround = false; }
            if (IsKeyPressed(KEY_UP) && player2OnGround) { player2Vel.y = JUMP_FORCE; player2OnGround = false; }

            player1Vel.y += GRAVITY;
            player2Vel.y += GRAVITY;
            player1Pos.y += player1Vel.y;
            player2Pos.y += player2Vel.y;
            player1OnGround = false;
            player2OnGround = false;

            // platform collision & score
            for (int i = 0; i < PLATFORM_COUNT; i++)
            {
                Rectangle plat = platforms[i].rect;
                Rectangle dropZone = platforms[i].dropZone;

                // player 1
                if (player1Vel.y > 0)
                {
                    float prevBot = prevY1 + PLAYER_HEIGHT;
                    float currBot = player1Pos.y + PLAYER_HEIGHT;
                    if (prevBot <= plat.y && currBot >= plat.y &&
                        player1Pos.x + PLAYER_WIDTH > plat.x && player1Pos.x < plat.x + plat.width)
                    {
                        Rectangle pRect = { player1Pos.x, player1Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
                        bool inDropZone = CheckCollisionRecs(pRect, dropZone);
                        if (inDropZone && IsKeyDown(KEY_S) && i != 0) continue;
                        player1Pos.y = plat.y - PLAYER_HEIGHT;
                        player1Vel.y = 0;
                        player1OnGround = true;

                        int newScore = (int)((850 - plat.y) / VERTICAL_SPACING);
                        if (newScore > score) score = newScore;
                    }
                }

                // player 2
                if (player2Vel.y > 0)
                {
                    float prevBot = prevY2 + PLAYER_HEIGHT;
                    float currBot = player2Pos.y + PLAYER_HEIGHT;
                    if (prevBot <= plat.y && currBot >= plat.y &&
                        player2Pos.x + PLAYER_WIDTH > plat.x && player2Pos.x < plat.x + plat.width)
                    {
                        Rectangle pRect = { player2Pos.x, player2Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
                        bool inDropZone = CheckCollisionRecs(pRect, dropZone);
                        if (inDropZone && IsKeyDown(KEY_DOWN) && i != 0) continue;
                        player2Pos.y = plat.y - PLAYER_HEIGHT;
                        player2Vel.y = 0;
                        player2OnGround = true;

                        int newScore = (int)((850 - plat.y) / VERTICAL_SPACING);
                        if (newScore > score) score = newScore;
                    }
                }
            }

            // shooter & bullet updates
            for (int i = 0; i < PLATFORM_COUNT; i++)
            {
                if (!shooters[i].active) continue;
                shooters[i].shootTimer++;
                if (shooters[i].shootTimer >= SHOOT_INTERVAL)
                {
                    shooters[i].shootTimer = 0;
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].pos = (Vector2){ shooters[i].rect.x + shooters[i].rect.width/2,
                                                       shooters[i].rect.y + shooters[i].rect.height/2 };
                            bullets[b].direction = shooters[i].direction;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active) continue;
                bullets[i].pos.x += BULLET_SPEED * bullets[i].direction;

                if (bullets[i].pos.x < -2000 || bullets[i].pos.x > 4000)
                {
                    bullets[i].active = false;
                    continue;
                }

                Rectangle bulletRect = { bullets[i].pos.x - 5, bullets[i].pos.y - 5, 10, 10 };
                if (CheckCollisionRecs(bulletRect, player1Rect) || CheckCollisionRecs(bulletRect, player2Rect))
                {
                    GeneratePlatforms(platforms);
                    RegenerateShooters(platforms, shooters);
                    for (int j = 0; j < MAX_BULLETS; j++) bullets[j].active = false;

                    player1Pos.x = platforms[0].rect.x + PLATFORM_WIDTH/2 - PLAYER_WIDTH/2;
                    player1Pos.y = platforms[0].rect.y - PLAYER_HEIGHT;
                    player2Pos = player1Pos;

                    player1Vel = (Vector2){0,0};
                    player2Vel = (Vector2){0,0};
                    player1OnGround = player2OnGround = true;

                    score = 0;
                    break;
                }
            }

            camera.target.x = (player1Pos.x + player2Pos.x)/2;
            camera.target.y = (player1Pos.y + player2Pos.y)/2;
        }
        else
        {
            // --- WIN SCREEN CAMERA ---
            camera.target.x = topPlatform.x + topPlatform.width/2;
            camera.target.y = topPlatform.y + topPlatform.height/2;
            winTextTimer += 0.05f;
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(DARKBLUE);

        if (win)
        {
            // Dimmed background
            DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,(Color){0,0,0,150});

            // Floating "YOU WIN!" text
            int floatOffset = (int)(10 * sin(winTextTimer));
            DrawText("YOU WIN!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 40 + floatOffset, 60, GOLD);
            DrawText("Press R to Play Again or Q to Quit", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 + 50 + floatOffset, 30, WHITE);

            // Players hidden on win screen
        }
        else
        {
            BeginMode2D(camera);

            for (int i = 0; i < PLATFORM_COUNT; i++)
            {
                if (i == 0)
                    DrawRectangleRec(platforms[i].rect, BLACK);
                else if (i == PLATFORM_COUNT-1)
                    DrawRectangleRec(platforms[i].rect, GOLD);
                else
                    DrawRectangleRec(platforms[i].rect, GRAY);

                if (i != 0)
                    DrawRectangleRec(platforms[i].dropZone, ORANGE);

                if (shooters[i].active)
                    DrawRectangleRec(shooters[i].rect, PURPLE);
            }

            for (int i = 0; i < MAX_BULLETS; i++)
                if (bullets[i].active)
                    DrawCircle(bullets[i].pos.x, bullets[i].pos.y, 5, YELLOW);

            DrawRectangle(player1Pos.x, player1Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, RED);
            DrawRectangle(player2Pos.x, player2Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, BLUE);

            EndMode2D();

            DrawText(TextFormat("Score: %i", score), 20, 20, 20, WHITE);
            DrawText("Q: Lobby | R: Respawn | S/DOWN: Drop | WASD: P1 | Arrows: P2", 20, 50, 20, WHITE);
        }

        EndDrawing();
    }
}