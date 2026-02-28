#include <raylib.h>
#include "../include/platformer.h"
#include "../include/bottles.h"
#include "../include/quickdraw.h"

#define QUICK_DRAW 1
#define BOTTLES 2
#define PLATFORMER 3
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
			enterQuickdraw();
		}
		if(currentScene == BOTTLES)
		{
			enterBottles();
		}
		if(currentScene == PLATFORMER)
		{
			enterPlatformer();
		}

	}


	CloseWindow();
	return 0;


}
