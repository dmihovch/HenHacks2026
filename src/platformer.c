#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../include/constants.h"

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 60
#define PLAYER_SPEED 6
#define JUMP_FORCE -17
#define GRAVITY 0.8f

#define PLATFORM_COUNT 5
#define PLATFORM_WIDTH 500
#define PLATFORM_HEIGHT 20
#define VERTICAL_SPACING 140

#define MAX_BULLETS 250
#define BULLET_SPEED 8
#define SHOOT_INTERVAL 120

#define MAX_PARTICLES 500

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

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    float radius;
    Color color;
    int life;
    bool active;
} Particle;

void GeneratePlatforms(Platform platforms[])
{
    float baseX = (float)WIDTH / 2.0f - PLATFORM_WIDTH / 2.0f;
    float baseY = (float)HEIGHT - 110.0f; 
    
    platforms[0].rect = (Rectangle){ baseX, baseY, PLATFORM_WIDTH, PLATFORM_HEIGHT };
    platforms[0].dropZone = (Rectangle){0,0,0,0};
    
    float prevX = baseX;
    float prevY = baseY;
    float prevWidth = PLATFORM_WIDTH;

    for (int i = 1; i < PLATFORM_COUNT; i++)
    {
        float newWidth = (float)GetRandomValue(260, 500);
        float overlap = (float)GetRandomValue(80, 160);
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
        if (newX + newWidth > WIDTH - 100) newX = (float)WIDTH - 100.0f - newWidth;

        float newY = prevY - VERTICAL_SPACING;
        platforms[i].rect = (Rectangle){ newX, newY, newWidth, PLATFORM_HEIGHT };
        // The Drop Zone is the middle 30% of the platform
        platforms[i].dropZone = (Rectangle){ newX + newWidth*0.35f, newY, newWidth*0.3f, PLATFORM_HEIGHT };
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
        if (GetRandomValue(0,100) < spawnChance*100)
        {
            shooters[i].active = true;
            Rectangle p = platforms[i].rect;
            if (GetRandomValue(0,1) == 0) {
                shooters[i].rect = (Rectangle){ p.x - 20, p.y - 40, 20, 40 };
                shooters[i].direction = 1;
            } else {
                shooters[i].rect = (Rectangle){ p.x + p.width, p.y - 40, 20, 40 };
                shooters[i].direction = -1;
            }
        }
    }
}

void SpawnJumpParticles(Particle particles[], Vector2 pos)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
        {
            particles[i].active = true;
            particles[i].pos = (Vector2){ pos.x + PLAYER_WIDTH/2., pos.y + PLAYER_HEIGHT };
            particles[i].radius = (float)GetRandomValue(3,6);
            particles[i].velocity = (Vector2){ (float)GetRandomValue(-4,4), (float)GetRandomValue(-7,-2) };
            particles[i].color = (Color){ 200, 200, 200, 255 };
            particles[i].life = 30;
            break;
        }
    }
}

int enterPlatformer(void)
{
	

    srand((unsigned int)time(NULL));
    Texture2D background = LoadTexture("assets/mine.png"); 

	Texture2D femaleSheriff = LoadTexture("assets/new-sheriff-removebg-preview.png");
	Texture2D maleSheriff = LoadTexture("assets/male-sheriff-removebg-preview.png");
	Texture2D bandit = LoadTexture("assets/new-bandit-removebg-preview.png");

    Platform platforms[PLATFORM_COUNT];
    Shooter shooters[PLATFORM_COUNT];
    Bullet bullets[MAX_BULLETS];
    Particle particles[MAX_PARTICLES];

    GeneratePlatforms(platforms);
    RegenerateShooters(platforms, shooters);

    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;

    Vector2 player1Pos = { platforms[0].rect.x + PLATFORM_WIDTH/2. - PLAYER_WIDTH/2.,
                            platforms[0].rect.y - PLAYER_HEIGHT };
    Vector2 player2Pos = player1Pos;
    Vector2 player1Vel = {0,0}, player2Vel = {0,0};
    bool player1OnGround = true, player2OnGround = true;

    int score = 0;
	int total_wins = 0;
	bool already_won = false;

    Camera2D camera = {0};
    camera.offset = (Vector2){ (float)WIDTH/2, (float)HEIGHT/2 };
    camera.zoom = 1.0f;

    float normalZoom = 1.0f, zoomedOut = 0.22f, zoomLerpSpeed = 0.05f;
    bool zoomToggled = false;
    float winTextTimer = 0.0f;
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q)) break;
        if (IsKeyPressed(KEY_K)) zoomToggled = !zoomToggled;

        if (IsKeyPressed(KEY_R))
        {
            GeneratePlatforms(platforms);
            RegenerateShooters(platforms, shooters);
            for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
            player1Pos.x = platforms[0].rect.x + PLATFORM_WIDTH/2. - PLAYER_WIDTH/2.;
            player1Pos.y = platforms[0].rect.y - PLAYER_HEIGHT;
            player2Pos = player1Pos;
            player1Vel = player2Vel = (Vector2){0,0};
            player1OnGround = player2OnGround = true;
            score = 0; 
			winTextTimer = 0.0f;
			already_won = false;
        }

        Rectangle topPlatform = platforms[PLATFORM_COUNT-1].rect;

        bool win = (player1OnGround && player1Pos.y + PLAYER_HEIGHT == topPlatform.y) && 
                   (player2OnGround && player2Pos.y + PLAYER_HEIGHT == topPlatform.y);

		if(win && !already_won)
		{
			total_wins++;
			already_won = true;
		}

        if (!win)
        {
            float prevY1 = player1Pos.y, prevY2 = player2Pos.y;
            if (IsKeyDown(KEY_A)) player1Pos.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_D)) player1Pos.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_LEFT)) player2Pos.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_RIGHT)) player2Pos.x += PLAYER_SPEED;

            if (IsKeyPressed(KEY_W) && player1OnGround) { player1Vel.y = JUMP_FORCE; player1OnGround = false; SpawnJumpParticles(particles, player1Pos); }
            if (IsKeyPressed(KEY_UP) && player2OnGround) { player2Vel.y = JUMP_FORCE; player2OnGround = false; SpawnJumpParticles(particles, player2Pos); }

            player1Vel.y += GRAVITY; player2Vel.y += GRAVITY;
            player1Pos.y += player1Vel.y; player2Pos.y += player2Vel.y;
            player1OnGround = player2OnGround = false;

            Rectangle p1Rect = { player1Pos.x, player1Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
            Rectangle p2Rect = { player2Pos.x, player2Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };

            for (int i = 0; i < PLATFORM_COUNT; i++)
            {
                Rectangle plat = platforms[i].rect;
                // P1 Collision
                if (player1Vel.y > 0 && (prevY1 + PLAYER_HEIGHT) <= plat.y && (player1Pos.y + PLAYER_HEIGHT) >= plat.y && 
                    player1Pos.x + PLAYER_WIDTH > plat.x && player1Pos.x < plat.x + plat.width)
                {
                    // Check if player is NOT trying to drop through
                    bool inDropZone = CheckCollisionRecs(p1Rect, platforms[i].dropZone);
                    if (!(inDropZone && IsKeyDown(KEY_S) && i != 0)) {
                        player1Pos.y = plat.y - PLAYER_HEIGHT; player1Vel.y = 0; player1OnGround = true;
                        int s = (int)((platforms[0].rect.y - plat.y) / VERTICAL_SPACING); if (s > score) score = s;
                    }
                }
                // P2 Collision
                if (player2Vel.y > 0 && (prevY2 + PLAYER_HEIGHT) <= plat.y && (player2Pos.y + PLAYER_HEIGHT) >= plat.y && 
                    player2Pos.x + PLAYER_WIDTH > plat.x && player2Pos.x < plat.x + plat.width)
                {
                    bool inDropZone = CheckCollisionRecs(p2Rect, platforms[i].dropZone);
                    if (!(inDropZone && IsKeyDown(KEY_DOWN) && i != 0)) {
                        player2Pos.y = plat.y - PLAYER_HEIGHT; player2Vel.y = 0; player2OnGround = true;
                        int s = (int)((platforms[0].rect.y - plat.y) / VERTICAL_SPACING); if (s > score) score = s;
                    }
                }
            }

            for (int i = 0; i < PLATFORM_COUNT; i++)
            {
                if (!shooters[i].active) continue;
                if (++shooters[i].shootTimer >= SHOOT_INTERVAL) {
                    shooters[i].shootTimer = 0;
                    for (int b = 0; b < MAX_BULLETS; b++) if (!bullets[b].active) {
                        bullets[b].active = true; bullets[b].pos = (Vector2){ shooters[i].rect.x + shooters[i].rect.width/2, shooters[i].rect.y + shooters[i].rect.height/2 };
                        bullets[b].direction = shooters[i].direction; break;
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                bullets[i].pos.x += BULLET_SPEED * bullets[i].direction;
                if (bullets[i].pos.x < -WIDTH/4. || bullets[i].pos.x > WIDTH + WIDTH/4.) bullets[i].active = false;
                else if (CheckCollisionRecs((Rectangle){ bullets[i].pos.x - 5, bullets[i].pos.y - 5, 10, 10 }, p1Rect) || 
                         CheckCollisionRecs((Rectangle){ bullets[i].pos.x - 5, bullets[i].pos.y - 5, 10, 10 }, p2Rect))
                {
                    GeneratePlatforms(platforms); RegenerateShooters(platforms, shooters);
                    for (int j = 0; j < MAX_BULLETS; j++) bullets[j].active = false;
                    player1Pos.x = platforms[0].rect.x + PLATFORM_WIDTH/2. - PLAYER_WIDTH/2.;
                    player1Pos.y = platforms[0].rect.y - PLAYER_HEIGHT;
                    player2Pos = player1Pos; player1Vel = player2Vel = (Vector2){0,0}; player1OnGround = player2OnGround = true; score = 0; already_won = false; break;
                }
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++) if (particles[i].active) {
            particles[i].pos.x += particles[i].velocity.x; particles[i].pos.y += particles[i].velocity.y;
            particles[i].velocity.y += 0.5f; if (--particles[i].life <= 0) particles[i].active = false;
        }

        if (zoomToggled) {
            camera.zoom += (zoomedOut - camera.zoom) * zoomLerpSpeed;
            camera.target.x += ((float)WIDTH/2.0f - camera.target.x) * zoomLerpSpeed;
            camera.target.y += ((platforms[PLATFORM_COUNT-1].rect.y + platforms[0].rect.y)/2 - camera.target.y) * zoomLerpSpeed;
        } else {
            camera.zoom += (normalZoom - camera.zoom) * zoomLerpSpeed;
            camera.target.x += ((player1Pos.x + player2Pos.x)/2 - camera.target.x) * zoomLerpSpeed;
            camera.target.y += ((player1Pos.y + player2Pos.y)/2 - camera.target.y) * zoomLerpSpeed;
        }

        BeginDrawing();
        ClearBackground(BLACK);
		DrawFPS(0,0);
        BeginMode2D(camera);
        
        float levelCeiling = platforms[PLATFORM_COUNT-1].rect.y - 1200; 
        float levelFloor = platforms[0].rect.y + 1200;              
        float tileWidth = 600.0f; 
        float tileHeight = (float)background.height * (tileWidth / (float)background.width); 
        float centerX = (float)WIDTH/2.0f - (tileWidth / 2.0f);

        for (int h = -2; h <= 2; h++) 
        {
            float currentColumnX = centerX + (h * tileWidth);
            for (float y = levelFloor; y > levelCeiling; y -= tileHeight) 
            {
                Rectangle destRect = { currentColumnX, y - tileHeight, tileWidth, tileHeight };
                DrawTexturePro(background, (Rectangle){ 0, 0, (float)background.width, (float)background.height }, destRect, (Vector2){0,0}, 0.0f, WHITE);
            }
        }

        for (int i = 0; i < PLATFORM_COUNT; i++) {
            if (i == 0) DrawRectangleRec(platforms[i].rect, BLACK);
            else if (i == PLATFORM_COUNT-1) DrawRectangleRec(platforms[i].rect, GOLD);
            else DrawRectangleRec(platforms[i].rect, DARKGRAY);
            
            DrawRectangleLinesEx(platforms[i].rect, 2, WHITE);

            if (i != 0) {
                Rectangle dz = platforms[i].dropZone;
                DrawRectangleRec(dz, (Color){20, 20, 20, 200});
                int pulse = (int)(155 + 100 * sin(winTextTimer * 4)); 
                Color gateColor = (Color){ pulse, 50, 50, 255 }; 
                for (int j = 0; j < dz.height; j += 4) {
                    DrawLineEx((Vector2){dz.x, dz.y + j}, (Vector2){dz.x + dz.width, dz.y + j}, 1, gateColor);
                }
                DrawRectangleLinesEx(dz, 2, BLACK);
            }

            if (shooters[i].active) {
                Rectangle bSource = { 0, 0, (float)bandit.width, (float)bandit.height };
                Rectangle bDest = shooters[i].rect;
                
                if (shooters[i].direction == -1) {
                    bSource.width *= -1; 
                }

                DrawTexturePro(bandit, bSource, bDest, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }

        for (int i = 0; i < MAX_BULLETS; i++) if (bullets[i].active) DrawCircle((int)bullets[i].pos.x, (int)bullets[i].pos.y, 5, YELLOW);
        for (int i = 0; i < MAX_PARTICLES; i++) if (particles[i].active) DrawCircleV(particles[i].pos, particles[i].radius, particles[i].color);

        if (!win) {
            Rectangle p1Source = { 0, 0, (float)maleSheriff.width, (float)maleSheriff.height };
            Rectangle p1Dest = { player1Pos.x, player1Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
            
            if (player1Vel.x < 0) p1Source.width *= -1; 
            
            DrawTexturePro(maleSheriff, p1Source, p1Dest, (Vector2){0, 0}, 0.0f, WHITE);
            Rectangle p2Source = { 0, 0, (float)femaleSheriff.width, (float)femaleSheriff.height };
            Rectangle p2Dest = { player2Pos.x, player2Pos.y, PLAYER_WIDTH, PLAYER_HEIGHT };
            
            if (player2Vel.x < 0) p2Source.width *= -1;
            
            DrawTexturePro(femaleSheriff, p2Source, p2Dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
        EndMode2D();

        if (win) {
            int floatOffset = (int)(10 * sin(winTextTimer));
            DrawRectangle(0,0,WIDTH,HEIGHT,(Color){0,0,0,150});
            DrawText("YOU WIN!", WIDTH/2 - 120, HEIGHT/2 - 40 + floatOffset, 60, GOLD);
            DrawText("Press R to Play Again or Q to Quit", WIDTH/2 - 220, HEIGHT/2 + 50 + floatOffset, 30, WHITE);
        }

        DrawText(TextFormat("Score: %i", score), 20, 20, 30, WHITE);
        DrawText("Q: Lobby | R: Respawn | S/DOWN: Drop | WASD: P1 | Arrows: P2 | K: Zoom Out", 20, 50, 30, WHITE);
        winTextTimer += 0.05f;
        EndDrawing();
    }
    UnloadTexture(background);
	UnloadTexture(maleSheriff);
	UnloadTexture(femaleSheriff);
	UnloadTexture(bandit);
	return total_wins;
}
