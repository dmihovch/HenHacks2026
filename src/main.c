#include <raylib.h>
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

	InitWindow(width,height,"Hackathon 2026");
	if(!IsWindowReady())
	{
		return 1;
	}

	SetTargetFPS(60);
	int currentScene = 0;
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		currentScene = GetKeyPressed();

		if(IsKeyPressed(QUICK_DRAW))
		{
			enterQuickdraw();
		}
		if(IsKeyPressed(BOTTLES))
		{
			enterBottles();
		}
		if(IsKeyPressed(PLATFORMER))
		{
			enterPlatformer();
		}

		BeginDrawing();

		ClearBackground(BLACK);
		DrawText("Town Lobby",WIDTH/2,HEIGHT/2,20,RED);

		EndDrawing();

	}


	CloseWindow();
	return 0;


}
