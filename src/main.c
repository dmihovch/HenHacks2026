#include <raylib.h>

int main(int argc, char** argv)
{

	const int width = 900;
	const int height = 600;

	InitWindow(width,height,"Hackathon 2026");
	if(!IsWindowReady())
	{
		return 1;
	}

	SetTargetFPS(60);
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{

		BeginDrawing();

		ClearBackground(BLACK);
		DrawCircle(width/2,height/2,10,RED);

		EndDrawing();
	}


	CloseWindow();
	return 0;


}
