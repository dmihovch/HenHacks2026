#include <raylib.h>
#include <math.h>
#include "../include/platformer.h"
#include "../include/bottles.h"
#include "../include/quickdraw.h"

#define QUICK_DRAW KEY_ONE
#define BOTTLES KEY_TWO
#define PLATFORMER KEY_THREE 
int main(int argc, char** argv)
{
	SetRandomSeed((int)GetTime());

	const int width = 1280;
	const int height = 960;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);

	InitWindow(width,height,"Hackathon 2026");
	if(!IsWindowReady())
	{
		return 1;
	}
	ToggleBorderlessWindowed();


	// --- LOAD BACKGROUND HERE ---
    // If you run from the project root (what VS Code usually does):
    Texture2D background = LoadTexture("assets/town.png");

    Rectangle building1 = { (float)width * 0.12f, (float)height * 0.07f, (float)width * 0.32f, (float)height * 0.32f};
	Rectangle building2 = { (float)width * 0.55f, (float)height * 0.08f, (float)width * 0.45f, (float)height * 0.35f};
	Rectangle building3 = { (float)width * 0.18f, (float)height * 0.58f, (float)width * 0.27f, (float)height * 0.32f};
	Rectangle building4 = { (float)width * 0.55f, (float)height * 0.58f, (float)width * 0.27f, (float)height * 0.32f};



	Vector2 ballPosition = { (float)width/2, (float)height/2 };
	Vector2 ballVelocity = { 0, 0 };

	const float ACCEL = 800.0f;      // acceleration rate
	const float MAX_SPEED = 300.0f;  // max movement speed
	const float FRICTION = 600.0f;   // how fast you slow down


	SetTargetFPS(60);
	int currentScene = 0;
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		float dt = GetFrameTime();

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

		// Accelerate on Y axis
		if (IsKeyDown(KEY_DOWN)) {
			ballVelocity.y += ACCEL * dt;
		} else if (IsKeyDown(KEY_UP)) {
			ballVelocity.y -= ACCEL * dt;
		} else {
			// Apply friction on Y
			if (ballVelocity.y > 0) {
				ballVelocity.y -= FRICTION * dt;
				if (ballVelocity.y < 0) ballVelocity.y = 0;
			} else if (ballVelocity.y < 0) {
				ballVelocity.y += FRICTION * dt;
				if (ballVelocity.y > 0) ballVelocity.y = 0;
			}
		}

		// Clamp diagonal speed so diagonals aren't faster
		float speed = sqrtf(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
		if (speed > MAX_SPEED) {
			float scale = MAX_SPEED / speed;
			ballVelocity.x *= scale;
			ballVelocity.y *= scale;
		}

		// Apply velocity to position
		ballPosition.x += ballVelocity.x * dt;
		ballPosition.y += ballVelocity.y * dt;

		currentScene = GetKeyPressed();

		if(IsKeyPressed(QUICK_DRAW))
		{
			enterQuickdraw();
		}
		if(IsKeyPressed(BOTTLES))
		{
			enterBottles(width, height);
		}
		if(IsKeyPressed(PLATFORMER))
		{
			enterPlatformer();
		}

		BeginDrawing();

		// --- DRAW BACKGROUND FIRST ---
        DrawTexturePro(
            background,
            (Rectangle){0, 0, (float)background.width, (float)background.height},   // source
            (Rectangle){0, 0, (float)width, (float)height},                         // destination (fills window)
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

		DrawRectangleLinesEx(building1, 3, RED);
		DrawRectangleLinesEx(building2, 3, RED);
		DrawRectangleLinesEx(building3, 3, RED);
		DrawRectangleLinesEx(building4, 3, RED);

		ClearBackground(BLACK);
		DrawText("Town Lobby",WIDTH/2,HEIGHT/2,20,RED);
		DrawCircleV(ballPosition, 50, MAROON);

		EndDrawing();

	}


	CloseWindow();
	return 0;


}
