#include <raylib.h>
#include "../include/minigame2.h"
#include "../include/minigame3.h"
#include "../include/quickDraw.h"

#define QUICK_DRAW 1
#define MINIGAME_2 2
#define MINIGAME_3 3
int main(int argc, char** argv)
{

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
		if(currentScene == QUICK_DRAW)
		{
			quickDraw();
		}
		if(currentScene == MINIGAME_2)
		{
			miniGame2();
		}
		if(currentScene == MINIGAME_3)
		{
			miniGame3();
		}

	}


	CloseWindow();
	return 0;


}
