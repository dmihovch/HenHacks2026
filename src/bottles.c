#include "../include/bottles.h"

void enterBottles()
{
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{
		BeginDrawing();
		DrawFPS(0,0);
		ClearBackground(BLACK);
		DrawText("Bottles",WIDTH/2, HEIGHT/2, 20, RED);
		EndDrawing();
	}
}
