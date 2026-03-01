#include <raylib.h>
#include <stdio.h>
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

    int textWIDTH = MeasureText(text, fontSize);
    int textHEIGHT = fontSize;

    DrawRectangle((int)(x - padding), (int)(y - padding), textWIDTH + padding*2, textHEIGHT + padding*2, WHITE);
    DrawText(text, (int)x, (int)y, fontSize, textColor);
}

void drawScoreboard(int p1_score, int p2_score)
{
	int scoreFontSize = 30;
	const char* scoreText = TextFormat("Player 1: %d   |   Player 2: %d", p1_score, p2_score);
	int scoreWidth = MeasureText(scoreText, scoreFontSize);

	float scoreX = (WIDTH / 2.0f) - (scoreWidth / 2.0f);
	float scoreY = HEIGHT * 0.05f; 

	DrawLabelWithHighlight(scoreText, scoreX, scoreY, scoreFontSize, BLACK);
}


int main(int argc, char** argv)
{
	SetRandomSeed((int)GetTime());


	// Hitboxes
	const bool hitBoxes = false;
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);

	InitWindow(WIDTH,HEIGHT,"Hackathon 2026");
	if(!IsWindowReady())
	{
		return 1;
	}
	ToggleBorderlessWindowed();





	Texture2D sheriffMaleTexture = LoadTexture("assets/male-sheriff-removebg-preview.png");
	Texture2D sheriffFemaleTexture = LoadTexture("assets/new-sheriff-removebg-preview.png");
    Texture2D background = LoadTexture("assets/town.png");

    Rectangle q1 = { (float)WIDTH * 0.0f, (float)HEIGHT * 0.0f, (float)WIDTH * 0.44f, (float)HEIGHT * 0.39f};
	Rectangle q2 = { (float)WIDTH * 0.55f, (float)HEIGHT * 0.0f, (float)WIDTH * 0.45f, (float)HEIGHT * 0.43f};
	Rectangle q3 = { (float)WIDTH * 0.0f, (float)HEIGHT * 0.58f, (float)WIDTH * 0.45f, (float)HEIGHT * 0.42f};
	Rectangle q4 = { (float)WIDTH * 0.55f, (float)HEIGHT * 0.58f, (float)WIDTH * 0.45f, (float)HEIGHT * 0.42f};
	Rectangle leftW = { (float)WIDTH * 0.0f, (float)HEIGHT * 0.39f, (float)WIDTH * 0.10f, (float)HEIGHT * 0.19f};
	Rectangle rightW = { (float)WIDTH * 0.90f, (float)HEIGHT * 0.43f, (float)WIDTH * 0.10f, (float)HEIGHT * 0.15f};
	//Rectangle upW = { (float)WIDTH * 0.44f, (float)HEIGHT * 0.0f, (float)WIDTH * 0.11f, (float)HEIGHT * 0.05f};
	//Rectangle downW = { (float)WIDTH * 0.45f, (float)HEIGHT * 0.95f, (float)WIDTH * 0.10f, (float)HEIGHT * 0.05f};
	Rectangle weirdW = { (float)WIDTH * 0.78f, (float)HEIGHT * 0.54f, (float)WIDTH * 0.12f, (float)HEIGHT * 0.04f};
	
	Rectangle walls[] = { q1, q2, q3, q4, leftW, rightW, weirdW };
	int wallCount = sizeof(walls) / sizeof(walls[0]);

	Rectangle bottles_select = { (float)WIDTH * 0.23f, (float)HEIGHT * 0.39f, (float)WIDTH * 0.10f, (float)HEIGHT * 0.04f};
	Rectangle quick_draw_select = { (float)WIDTH * 0.76f, (float)HEIGHT * 0.43f, (float)WIDTH * 0.10f, (float)HEIGHT * 0.04f};
	Rectangle platformers_select = { (float)WIDTH * 0.46f, (float)HEIGHT * 0.70f, (float)WIDTH * 0.03f, (float)HEIGHT * 0.12f};

	Vector2 ballPosition1 = { (float)WIDTH/2 - WIDTH*0.05, (float)HEIGHT/2 };
	Vector2 ballVelocity1 = { 0, 0 };
	Vector2 ballPosition2 = { (float)WIDTH/2 + WIDTH*0.05, (float)HEIGHT/2 };
	Vector2 ballVelocity2 = { 0, 0 };
	Vector2 playersP[] = {ballPosition1, ballPosition2};
	Vector2 playersV[] = {ballVelocity1, ballVelocity2};
	int playerCount = sizeof(playersP) / sizeof(playersP[0]);

	float r = 30.0f;
	

	const float ACCEL = 1200.0f;
	const float MAX_SPEED = 300.0f;
	const float FRICTION = 800.0f;

	int player1_points = 0;
	int player2_points = 0;

	SetTargetFPS(60);
	while(!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_Q)) break;
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
			if (playersP[i].y - 30 > HEIGHT || playersP[i].y + 30 < 0) {
				doBreak = true;
			}
		}

		if (doBreak) {
			break;
		}


		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(0,0);

        DrawTexturePro(
            background,
            (Rectangle){0, 0, (float)background.width, (float)background.height},   // source
            (Rectangle){0, 0, (float)WIDTH, (float)HEIGHT},                         // destination (fills window)
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

        if (p0Quick && p1Quick) {
            QuickDrawWinner qdw = enterQuickdraw();
            player1_points += qdw.p1wins;
            player2_points += qdw.p2wins;
            
            playersP[0] = ballPosition1; 
            playersP[1] = ballPosition2;
            playersV[0] = (Vector2){0,0};
            playersV[1] = (Vector2){0,0};
        }

        if (p0Bottles && p1Bottles) {
            BottlesScore bs = enterBottles(WIDTH, HEIGHT);
			player1_points += bs.p1Score;
			player2_points += bs.p2Score;
            playersP[0] = ballPosition1; 
            playersP[1] = ballPosition2;
            playersV[0] = (Vector2){0,0};
            playersV[1] = (Vector2){0,0};
        }

        if (p0Plat && p1Plat) {
            int points = enterPlatformer();
			player1_points += points;
			player2_points += points;
            playersP[0] = ballPosition1; 
            playersP[1] = ballPosition2;
            playersV[0] = (Vector2){0,0};
            playersV[1] = (Vector2){0,0};
        }

        DrawLabelWithHighlight("Quick Draw", quick_draw_select.x, quick_draw_select.y - 30, 20, BLACK);
        DrawLabelWithHighlight("Bottles", bottles_select.x, bottles_select.y - 30, 20, BLACK);
        DrawLabelWithHighlight("Platformer", platformers_select.x, platformers_select.y - 30, 20, BLACK);

        DrawLabelWithHighlight("Wild West", (int)WIDTH * 0.02f, (int)HEIGHT * 0.02f, 20, BLACK);

		DrawLabelWithHighlight("Walk off to Exit", WIDTH * 0.44f, HEIGHT * 0.02f, 20, MAROON);
        DrawLabelWithHighlight("Walk off to Exit", WIDTH * 0.44f, HEIGHT * 0.94f, 20, MAROON);
		float spriteSize = r * 2.0f;
        
        Rectangle p1Source = { 0, 0, (float)sheriffMaleTexture.width, (float)sheriffMaleTexture.height };
        Rectangle p1Dest = { playersP[0].x, playersP[0].y, spriteSize, spriteSize };
        Vector2 origin = { r, r }; // Center the texture exactly on the coordinate
        
        DrawTexturePro(sheriffMaleTexture, p1Source, p1Dest, origin, 0.0f, WHITE);

        Rectangle p2Source = { 0, 0, (float)sheriffFemaleTexture.width, (float)sheriffFemaleTexture.height };
        Rectangle p2Dest = { playersP[1].x, playersP[1].y, spriteSize, spriteSize };
        
        DrawTexturePro(sheriffFemaleTexture, p2Source, p2Dest, origin, 0.0f, WHITE);

        drawScoreboard(player1_points, player2_points);

        EndDrawing();
	}
  
	UnloadTexture(sheriffFemaleTexture);
	UnloadTexture(sheriffMaleTexture);
	UnloadTexture(background);
	CloseWindow();
	return 0;
}
