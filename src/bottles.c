#include <raylib.h>
#include <math.h>
#include "../include/platformer.h"
#include "../include/bottles.h"
#include "../include/quickdraw.h"

typedef struct Bottle {
		Vector2 pos;
		bool active;
} Bottle;

typedef struct Bullet {
    Vector2 pos;
    float speed;
    bool active;
} Bullet;



void enterBottles(int width, int height)
{

	Vector2 ballPosition = { (float)width/2, (float)height - (float)height/10 };
	Vector2 ballVelocity = { 0, 0 };

	const float ACCEL = 1200.0f;      // acceleration rate
	const float MAX_SPEED = 350.0f;  // max movement speed
	const float FRICTION = 600.0f;   // how fast you slow down
	const float BULLET_SPEED = 2400.0f;

	// Bottle System
	Bottle bottles[100] = {0};
    int bottleCount = 0;
    float spawnTimer = 0.0f;
	float spawnInterval = 3.0f;      // starts at 3 seconds
	float minInterval = 1.5f;
	float intervalDecay = 0.90f;     // multiply interval by this each spawn

	float barWidth = width * 0.75f;
	float barHeight = height * 0.05f;
	float leftX  = (width - barWidth) / 2.0f;
	float rightX = leftX + barWidth;

	// Bullet system
    Bullet bullets[20] = {0};
	float shootCooldown = 0.0f;
	float shootInterval = 0.5f;

    int score = 0;

	SetTargetFPS(60);

	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		float dt = GetFrameTime();

        // -------------------------
        // PLAYER MOVEMENT
        // -------------------------

		// Accelerate on X axis
		if (IsKeyDown(KEY_RIGHT)) {
			ballVelocity.x += ACCEL * dt;
		} else if (IsKeyDown(KEY_LEFT)) {
			ballVelocity.x -= ACCEL * dt;
		} else {
		// Apply friction on X
			if (ballVelocity.x > 0) {
				ballVelocity.x -= FRICTION * dt;
				if (ballVelocity.x < 0) ballVelocity.x = 0;
			} else if (ballVelocity.x < 0) {
				ballVelocity.x += FRICTION * dt;
				if (ballVelocity.x > 0) ballVelocity.x = 0;
			}
		}

		// Clamp diagonal speed so diagonals aren't faster
		float speed = sqrtf(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
		if (speed > MAX_SPEED) {
			float scale = MAX_SPEED / speed;
			ballVelocity.x *= scale;
		}

		// Apply velocity to position
		ballPosition.x += ballVelocity.x * dt;

		// -------------------------
        // SHOOTING (UP ARROW)
        // -------------------------

		shootCooldown += dt;

        if (IsKeyPressed(KEY_UP) && shootCooldown >= shootInterval){

    		shootCooldown = 0.0f;

			for (int i = 0; i < 20; i++) {
				if (!bullets[i].active) {
					bullets[i].active = true;
					bullets[i].pos = ballPosition;
					bullets[i].speed = BULLET_SPEED;
					break;
				}
			}
        }

		// Update bullets
        for (int i = 0; i < 20; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].pos.y -= bullets[i].speed * dt;

                if (bullets[i].pos.y < 0)
                    bullets[i].active = false;
            }
        }

        // -------------------------
        // BOTTLE SPAWNING
        // -------------------------
        spawnTimer += dt;

        if (spawnTimer >= spawnInterval && bottleCount < 100)
        {
            spawnTimer = 0.0f;

            // Find an inactive bottle slot
            for (int i = 0; i < 100; i++)
            {
                if (!bottles[i].active)
                {
                    bottles[i].active = true;
                    bottles[i].pos.x = (float)GetRandomValue((int)leftX, (int)rightX);
					int rand = GetRandomValue(1, 3);
					if (rand == 1) bottles[i].pos.y = (height * 0.1f);
					if (rand == 2) bottles[i].pos.y = (height * 0.2f);
					if (rand == 3) bottles[i].pos.y = (height * 0.3f);
                    bottleCount++;
                    break;
                }
            }

			// Make next spawn faster
			spawnInterval *= intervalDecay;
			if (spawnInterval < minInterval)
				spawnInterval = minInterval;

        }

        // -------------------------
        // BULLET–BOTTLE COLLISION
        // -------------------------
		for (int b = 0; b < 20; b++)
		{
			if (!bullets[b].active) continue;

			for (int i = 0; i < 100; i++)
			{
				if (!bottles[i].active) continue;

				Rectangle bulletRect = {
					bullets[b].pos.x - 3,
					bullets[b].pos.y - 15,
					6,
					30
				};

				Rectangle bottleRect = {
					bottles[i].pos.x - 20,
					bottles[i].pos.y - 30,
					40,
					60
				};

				if (CheckCollisionRecs(bulletRect, bottleRect))
				{
					bullets[b].active = false;
					bottles[i].active = false;
					score++;
				}
			}
		}


        // -------------------------
        // DRAWING
        // -------------------------
        BeginDrawing();
        ClearBackground(WHITE);

		// Environment
        DrawFPS(0, 0);
        DrawText("Bottles", width/2 - 50, height/2, 20, RED);
        DrawText(TextFormat("Score: %d", score), 20, 20, 30, DARKBLUE);
		DrawRectangle((int)leftX, (int)(height * 0.1f), (int)barWidth, (int)barHeight, BROWN);
		DrawRectangle((int)leftX, (int)(height * 0.2f), (int)barWidth, (int)barHeight, BROWN);
		DrawRectangle((int)leftX, (int)(height * 0.3f), (int)barWidth, (int)barHeight, BROWN);

        // Player
        DrawCircleV(ballPosition, 50, MAROON);

        // Bottles
        for (int i = 0; i < 100; i++)
            if (bottles[i].active)
                DrawRectangle((int)(bottles[i].pos.x) - 20, (int)bottles[i].pos.y - 30, 40, 60, BLUE);

        // Bullets
        for (int i = 0; i < 20; i++)
            if (bullets[i].active)
                DrawRectangle((int)bullets[i].pos.x - 3, (int)bullets[i].pos.y - 60, 6, 75, YELLOW);

        EndDrawing();

	}
}
