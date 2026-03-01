#include <raylib.h>
#include <math.h>
#include "../include/platformer.h"
#include "../include/bottles.h"
#include "../include/quickdraw.h"

#define QUICK_DRAW KEY_ONE
#define BOTTLES KEY_TWO
#define PLATFORMER KEY_THREE 

#define CLEAR CLITERAL(Color){ 0, 0, 0, 0 }

bool CollidesWithWalls(Vector2 pos, float radius, Rectangle *walls, int count) {
    Rectangle player = { pos.x - radius, pos.y - radius, radius*2, radius*2 };

    for (int i = 0; i < count; i++) {
        if (CheckCollisionRecs(player, walls[i])) {
            return true;
        }
    }
    return false;
}

bool CircleIntersectsRect(Vector2 pos, float radius, Rectangle rect) {
    Rectangle player = { pos.x - radius, pos.y - radius, radius*2, radius*2 };
    return CheckCollisionRecs(player, rect);
}

void DrawLabelWithHighlight(const char *text, float x, float y, int fontSize, Color textColor) {
    int padding = 6;

    int textWidth = MeasureText(text, fontSize);
    int textHeight = fontSize;

    DrawRectangle((int)(x - padding), (int)(y - padding), textWidth + padding*2, textHeight + padding*2, WHITE);
    DrawText(text, (int)x, (int)y, fontSize, textColor);
}


int main(int argc, char** argv)
{
	SetRandomSeed((int)GetTime());

	const int width = 1280;
	const int height = 960;

	// Hitboxes
	const hitBoxes = false;

	InitWindow(width,height,"Hackathon 2026");
	if(!IsWindowReady())
	{
		return 1;
	}

	// --- LOAD BACKGROUND HERE ---
    // If you run from the project root (what VS Code usually does):
    Texture2D background = LoadTexture("assets/town.png");

	// Bounds
    Rectangle q1 = { (float)width * 0.0f, (float)height * 0.0f, (float)width * 0.44f, (float)height * 0.39f};
	Rectangle q2 = { (float)width * 0.55f, (float)height * 0.0f, (float)width * 0.45f, (float)height * 0.43f};
	Rectangle q3 = { (float)width * 0.0f, (float)height * 0.58f, (float)width * 0.45f, (float)height * 0.42f};
	Rectangle q4 = { (float)width * 0.55f, (float)height * 0.58f, (float)width * 0.45f, (float)height * 0.42f};
	Rectangle leftW = { (float)width * 0.0f, (float)height * 0.39f, (float)width * 0.10f, (float)height * 0.19f};
	Rectangle rightW = { (float)width * 0.90f, (float)height * 0.43f, (float)width * 0.10f, (float)height * 0.15f};
	//Rectangle upW = { (float)width * 0.44f, (float)height * 0.0f, (float)width * 0.11f, (float)height * 0.05f};
	//Rectangle downW = { (float)width * 0.45f, (float)height * 0.95f, (float)width * 0.10f, (float)height * 0.05f};
	Rectangle weirdW = { (float)width * 0.78f, (float)height * 0.54f, (float)width * 0.12f, (float)height * 0.04f};
	
	Rectangle walls[] = { q1, q2, q3, q4, leftW, rightW, weirdW };
	int wallCount = sizeof(walls) / sizeof(walls[0]);

	Rectangle bottles_select = { (float)width * 0.23f, (float)height * 0.39f, (float)width * 0.10f, (float)height * 0.04f};
	Rectangle quick_draw_select = { (float)width * 0.76f, (float)height * 0.43f, (float)width * 0.10f, (float)height * 0.04f};
	Rectangle platformers_select = { (float)width * 0.46f, (float)height * 0.70f, (float)width * 0.03f, (float)height * 0.12f};

	Vector2 ballPosition = { (float)width/2, (float)height/2 };
	Vector2 ballVelocity = { 0, 0 };
	float r = 30.0f; // your player radius

	const float ACCEL = 1200.0f;      // acceleration rate
	const float MAX_SPEED = 300.0f;  // max movement speed
	const float FRICTION = 800.0f;   // how fast you slow down


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
		float nextX = ballPosition.x + ballVelocity.x * dt;
		float nextY = ballPosition.y + ballVelocity.y * dt;

		// Try X movement
		Vector2 tryX = { nextX, ballPosition.y };
		if (!CollidesWithWalls(tryX, 30, walls, wallCount)) {
			ballPosition.x = nextX;
		} else {
			ballVelocity.x = 0;
		}

		// Try Y movement
		Vector2 tryY = { ballPosition.x, nextY };
		if (!CollidesWithWalls(tryY, 30, walls, wallCount)) {
			ballPosition.y = nextY;
		} else {
			ballVelocity.y = 0;
		}

		// out of bounds --> quit
		if (ballPosition.y - 30 > height || ballPosition.y + 30 < 0) {
			break;
		}

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
		ClearBackground(BLACK);

		// --- DRAW BACKGROUND FIRST ---
        DrawTexturePro(
            background,
            (Rectangle){0, 0, (float)background.width, (float)background.height},   // source
            (Rectangle){0, 0, (float)width, (float)height},                         // destination (fills window)
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

		if (hitBoxes) {
			for (int i = 0; i < wallCount; i++) {
				DrawRectangleLinesEx(walls[i], 3, RED);
			}
		} else {
			for (int i = 0; i < wallCount; i++) {
				DrawRectangleLinesEx(walls[i], 3, CLEAR);
			}
		}

		// Game Selector
		DrawRectangleRec(quick_draw_select, GOLD);
		DrawRectangleLinesEx(quick_draw_select, 5, BLACK);

		DrawRectangleRec(bottles_select, GOLD);
		DrawRectangleLinesEx(bottles_select, 5, BLACK);

		DrawRectangleRec(platformers_select, GOLD);
		DrawRectangleLinesEx(platformers_select, 5, BLACK);

		if (CircleIntersectsRect(ballPosition, r, quick_draw_select)) {
			DrawLabelWithHighlight("Press E to enter Quick Draw",
								quick_draw_select.x,
								quick_draw_select.y - 30,
								20,
								BLACK);
			if (IsKeyPressed(KEY_E)) {
				enterQuickdraw();
			}
		}

		if (CircleIntersectsRect(ballPosition, r, bottles_select)) {
			DrawLabelWithHighlight("Press E to enter Bottles",
								bottles_select.x,
								bottles_select.y - 30,
								20,
								BLACK);
			if (IsKeyPressed(KEY_E)) {
				enterBottles(width, height);
			}
		}

		if (CircleIntersectsRect(ballPosition, r, platformers_select)) {
			DrawLabelWithHighlight("Press E to enter Platformer",
								platformers_select.x,
								platformers_select.y - 30,
								20,
								BLACK);
			if (IsKeyPressed(KEY_E)) {
				enterPlatformer();
			}
		}
		
		DrawLabelWithHighlight("Wild West", (int)width * 0.02f, (int)height * 0.02f, 20, BLACK);
		DrawCircleV(ballPosition, 30, MAROON);

		EndDrawing();

	}

	CloseWindow();
	return 0;
}
