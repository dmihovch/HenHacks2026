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
	int who;
} Bullet;



void enterBottles(int width, int height)
{
	const float ACCEL = 1200.0f;      // acceleration rate
	const float MAX_SPEED = 350.0f;  // max movement speed
	const float FRICTION = 600.0f;   // how fast you slow down
	const float BULLET_SPEED = 2400.0f;

	float barWidth = width * 0.75f;
	float barHeight = height * 0.05f;
	float leftX  = (width - barWidth) / 2.0f;
	float rightX = leftX + barWidth;

	// Bottle System
	Bottle bottles[50] = {0};
    int bottleCount = 0;
    float spawnTimer = 0.0f;
	float spawnInterval = 3.0f;      // starts at 3 seconds
	float minInterval = 1.0f;
	float intervalDecay = 0.90f;     // multiply interval by this each spawn

	// Bullet system
    Bullet bullets[20] = {0};
	float shootCooldown1 = 0.0f;
	float shootCooldown2 = 0.0f;
	float shootInterval = 0.5f;

	Vector2 ballPosition1 = { (float)leftX, (float)height - (float)height/10 };
	Vector2 ballVelocity1 = { 0, 0 };
	
	Vector2 ballPosition2 = { (float)rightX, (float)height - (float)height/10 };
	Vector2 ballVelocity2 = { 0, 0 };

    int score1 = 0;
	int score2 = 0;

	SetTargetFPS(60);

	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		float dt = GetFrameTime();

        // -------------------------
        // PLAYER 1 MOVEMENT
        // -------------------------

		if (IsKeyDown(KEY_D)) {
			ballVelocity1.x += ACCEL * dt;
		} else if (IsKeyDown(KEY_A)) {
			ballVelocity1.x -= ACCEL * dt;
		} else {
			if (ballVelocity1.x > 0) {
				ballVelocity1.x -= FRICTION * dt;
				if (ballVelocity1.x < 0) ballVelocity1.x = 0;
			} else if (ballVelocity1.x < 0) {
				ballVelocity1.x += FRICTION * dt;
				if (ballVelocity1.x > 0) ballVelocity1.x = 0;
			}
		}

		// Clamp speed
		float speed1 = (float)fabs(ballVelocity1.x);
		if (speed1 > MAX_SPEED) {
			ballVelocity1.x = (ballVelocity1.x > 0 ? 1 : -1) * MAX_SPEED;
		}

		// Apply velocity
		ballPosition1.x += ballVelocity1.x * dt;

		// Keep inside screen
		if (ballPosition1.x < 50) ballPosition1.x = 50;
		if (ballPosition1.x > width - 50) ballPosition1.x = (float)width - 50;

		// -------------------------
        // PLAYER 2 MOVEMENT
        // -------------------------

		if (IsKeyDown(KEY_RIGHT)) {
			ballVelocity2.x += ACCEL * dt;
		} else if (IsKeyDown(KEY_LEFT)) {
			ballVelocity2.x -= ACCEL * dt;
		} else {
			if (ballVelocity2.x > 0) {
				ballVelocity2.x -= FRICTION * dt;
				if (ballVelocity2.x < 0) ballVelocity2.x = 0;
			} else if (ballVelocity2.x < 0) {
				ballVelocity2.x += FRICTION * dt;
				if (ballVelocity2.x > 0) ballVelocity2.x = 0;
			}
		}

		// Clamp speed
		float speed2 = (float)fabs(ballVelocity2.x);
		if (speed2 > MAX_SPEED) {
			ballVelocity2.x = (ballVelocity2.x > 0 ? 1 : -1) * MAX_SPEED;
		}

		// Apply velocity
		ballPosition2.x += ballVelocity2.x * dt;

		// Keep inside screen
		if (ballPosition2.x < 50) ballPosition2.x = 50;
		if (ballPosition2.x > width - 50) ballPosition2.x = (float)width - 50;


		// -------------------------
        // SHOOTING PLAYER 1
        // -------------------------

		shootCooldown1 += dt;

		if (IsKeyPressed(KEY_W) && shootCooldown1 >= shootInterval){
			shootCooldown1 = 0.0f;

			for (int i = 0; i < 20; i++) {
				if (!bullets[i].active) {
					bullets[i].active = true;
					bullets[i].pos = ballPosition1;
					bullets[i].speed = BULLET_SPEED;
					bullets[i].who = 1;
					break;
				}
			}
		}

		// -------------------------
        // SHOOTING PLAYER 2
        // -------------------------

		shootCooldown2 += dt;

		if (IsKeyPressed(KEY_UP) && shootCooldown2 >= shootInterval){
			shootCooldown2 = 0.0f;

			for (int i = 0; i < 20; i++) {
				if (!bullets[i].active) {
					bullets[i].active = true;
					bullets[i].pos = ballPosition2;
					bullets[i].speed = BULLET_SPEED;
					bullets[i].who = 2;
					break;
				}
			}
		}

		// Update Bullets
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

        if (spawnTimer >= spawnInterval && bottleCount < 50)
        {
            spawnTimer = 0.0f;

            // Find an inactive bottle slot
            for (int i = 0; i < 50; i++)
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

			for (int i = 0; i < 50; i++)
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
					if (bullets[b].who == 1) score1++;
					if (bullets[b].who == 2) score2++;
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
        DrawText(TextFormat("Score: %d", score1), 20, 20, 30, MAROON);
		DrawText(TextFormat("Score: %d", score2), width - 180, 20, 30, DARKBLUE);
		DrawRectangle((int)leftX, (int)(height * 0.1f), (int)barWidth, (int)barHeight, BROWN);
		DrawRectangle((int)leftX, (int)(height * 0.2f), (int)barWidth, (int)barHeight, BROWN);
		DrawRectangle((int)leftX, (int)(height * 0.3f), (int)barWidth, (int)barHeight, BROWN);

        // Players
        DrawCircleV(ballPosition1, 50, MAROON);
		DrawCircleV(ballPosition2, 50, BLUE);


        // Bottles
        for (int i = 0; i < 50; i++)
            if (bottles[i].active)
                DrawRectangle((int)(bottles[i].pos.x) - 20, (int)bottles[i].pos.y - 30, 40, 60, BLUE);

        // Bullets
        for (int i = 0; i < 20; i++)
            if (bullets[i].active)
                DrawRectangle((int)bullets[i].pos.x - 3, (int)bullets[i].pos.y - 60, 6, 75, YELLOW);

        EndDrawing();

	}
}
