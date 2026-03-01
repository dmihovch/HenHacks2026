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

	Vector2 ballPosition1 = { (float)width/2, (float)height/2 };
	Vector2 ballVelocity1 = { 0, 0 };
	Vector2 ballPosition2 = { (float)width/2, (float)height/2 };
	Vector2 ballVelocity2 = { 0, 0 };
	Vector2 playersP[] = {ballPosition1, ballPosition2};
	Vector2 playersV[] = {ballVelocity1, ballVelocity2};
	int playerCount = sizeof(playersP) / sizeof(playersP[0]);

	float r = 30.0f; // your player radius
	

	const float ACCEL = 1200.0f;      // acceleration rate
	const float MAX_SPEED = 300.0f;  // max movement speed
	const float FRICTION = 800.0f;   // how fast you slow down


	SetTargetFPS(60);
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		float dt = GetFrameTime();
		int moveRight[2] = {KEY_D, KEY_RIGHT};
		int moveLeft[2] = {KEY_A, KEY_LEFT};
		int moveUp[2] = {KEY_W, KEY_UP};
		int moveDown[2] = {KEY_S, KEY_DOWN};
		bool doBreak = false;

		for (int i = 0; i < playerCount; i++) {
			// Accelerate on X axis
			if (IsKeyDown(moveRight[i])) {
				playersV[i].x += ACCEL * dt;
			} else if (IsKeyDown(moveLeft[i])) {
				playersV[i].x -= ACCEL * dt;
			} else {
				// Apply friction on X
				if (playersV[i].x > 0) {
					playersV[i].x -= FRICTION * dt;
					if (playersV[i].x < 0) playersV[i].x = 0;
				} else if (playersV[i].x < 0) {
					playersV[i].x += FRICTION * dt;
					if (playersV[i].x > 0) playersV[i].x = 0;
				}
			}

			// Accelerate on Y axis
			if (IsKeyDown(moveDown[i])) {
				playersV[i].y += ACCEL * dt;
			} else if (IsKeyDown(moveUp[i])) {
				playersV[i].y -= ACCEL * dt;
			} else {
				// Apply friction on Y
				if (playersV[i].y > 0) {
					playersV[i].y -= FRICTION * dt;
					if (playersV[i].y < 0) playersV[i].y = 0;
				} else if (playersV[i].y < 0) {
					playersV[i].y += FRICTION * dt;
					if (playersV[i].y > 0) playersV[i].y = 0;
				}
			}

			// Clamp diagonal speed so diagonals aren't faster
			float speed = sqrtf(playersV[i].x * playersV[i].x + playersV[i].y * playersV[i].y);
			if (speed > MAX_SPEED) {
				float scale = MAX_SPEED / speed;
				playersV[i].x *= scale;
				playersV[i].y *= scale;
			}

			// Apply velocity to position
			float nextX = playersP[i].x + playersV[i].x * dt;
			float nextY = playersP[i].y + playersV[i].y * dt;

			// Try X movement
			Vector2 tryX = { nextX, playersP[i].y };
			if (!CollidesWithWalls(tryX, 30, walls, wallCount)) {
				playersP[i].x = nextX;
			} else {
				playersV[i].x = 0;
			}

			// Try Y movement
			Vector2 tryY = { playersP[i].x, nextY };
			if (!CollidesWithWalls(tryY, 30, walls, wallCount)) {
				playersP[i].y = nextY;
			} else {
				playersV[i].y = 0;
			}

			// out of bounds --> quit
			if (playersP[i].y - 30 > height || playersP[i].y + 30 < 0) {
				doBreak = true;
			}
		}

		if (doBreak) {
			break;
		}


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
		
		
		bool p0Quick = CircleIntersectsRect(playersP[0], r, quick_draw_select);
		bool p1Quick = CircleIntersectsRect(playersP[1], r, quick_draw_select);

		bool p0Bottles = CircleIntersectsRect(playersP[0], r, bottles_select);
		bool p1Bottles = CircleIntersectsRect(playersP[1], r, bottles_select);

		bool p0Plat = CircleIntersectsRect(playersP[0], r, platformers_select);
		bool p1Plat = CircleIntersectsRect(playersP[1], r, platformers_select);


		// ---------------------------
		// FIRST: Player 1 labels (background)
		// ---------------------------
		if (p1Quick) {
			DrawLabelWithHighlight("Quick Draw",
								quick_draw_select.x,
								quick_draw_select.y - 30,
								20,
								BLACK);
		}

		if (p1Bottles) {
			DrawLabelWithHighlight("Bottles",
								bottles_select.x,
								bottles_select.y - 30,
								20,
								BLACK);
		}

		if (p1Plat) {
			DrawLabelWithHighlight("Platformer",
								platformers_select.x,
								platformers_select.y - 30,
								20,
								BLACK);
		}


		// ---------------------------
		// SECOND: Player 0 labels (foreground)
		// ---------------------------
		if (p0Quick) {
			DrawLabelWithHighlight("Press E to enter Quick Draw",
								quick_draw_select.x,
								quick_draw_select.y - 30,
								20,
								BLACK);
		}

		if (p0Bottles) {
			DrawLabelWithHighlight("Press E to enter Bottles",
								bottles_select.x,
								bottles_select.y - 30,
								20,
								BLACK);
		}

		if (p0Plat) {
			DrawLabelWithHighlight("Press E to enter Platformer",
								platformers_select.x,
								platformers_select.y - 30,
								20,
								BLACK);
		}


		// ---------------------------
		// E‑press functionality (player 0 only)
		// ---------------------------
		if (IsKeyPressed(KEY_E)) {
			if (p0Quick) enterQuickdraw();
			if (p0Bottles) enterBottles(width, height);
			if (p0Plat) enterPlatformer();
		}

		
		DrawLabelWithHighlight("Wild West", (int)width * 0.02f, (int)height * 0.02f, 20, BLACK);
		DrawCircleV(playersP[0], 30, MAROON);
		DrawCircleV(playersP[1], 30, BLUE);

		EndDrawing();

	}
  
	UnloadTexture(background);
	CloseWindow();
	return 0;
}