#include "../include/quickdraw.h"

void enterQuickdraw()
{
	while(!WindowShouldClose()&& GetKeyPressed() != KEY_Q)
	{
		BeginDrawing();
		DrawFPS(0,0);
		ClearBackground(BLACK);
		DrawText("QuickDraw",WIDTH/2, HEIGHT/2, 20, RED);
		EndDrawing();
	}
}
