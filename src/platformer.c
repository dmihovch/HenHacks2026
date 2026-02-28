#include "../include/platformer.h"

void enterPlatformer()
{
	while(!WindowShouldClose()&& GetKeyPressed() != KEY_Q)
	{
		BeginDrawing();
		DrawFPS(0,0);
		ClearBackground(BLACK);
		DrawText("Platormer",WIDTH/2, HEIGHT/2, 20, RED);
		EndDrawing();
	}
}
