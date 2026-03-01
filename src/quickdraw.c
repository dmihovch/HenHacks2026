#include "../include/quickdraw.h"
#include <stdio.h>



void drawCrosshair(Crosshair xh)
{
	DrawRectangle(xh.pos.x - (xh.length / 2.0f), xh.pos.y - (xh.thickness / 2.0f), xh.length, xh.thickness, xh.col);
	DrawRectangle(xh.pos.x - (xh.thickness / 2.0f), xh.pos.y - (xh.length / 2.0f), xh.thickness, xh.length, xh.col);
}

bool checkXhairOnTarget(Crosshair* xh, Target tg)
{
	return CheckCollisionPointCircle(xh->pos,tg.pos,tg.radius);
}

void clampXhair(Crosshair* xh)
{
	if(xh->pos.x < 0)
	{
		xh->pos.x = 0;
	}
	if(xh->pos.x > WIDTH)
	{
		xh->pos.x = WIDTH;
	}

	if(xh->pos.y < 0)
	{
		xh->pos.y = 0;
	}

	if(xh->pos.y > HEIGHT)
	{
		xh->pos.y = HEIGHT;
	}
}

int updateCrossHair(Crosshair* xh, float dt)
{
	
	float accel = 100000.f;
	float drag = 25.f;

	if(IsKeyDown(xh->key_up)) xh->vel.y-= accel * dt;
	if(IsKeyDown(xh->key_down)) xh->vel.y+= accel * dt;
	if(IsKeyDown(xh->key_left)) xh->vel.x-= accel * dt;
	if(IsKeyDown(xh->key_right)) xh->vel.x+= accel * dt;

	xh->vel.x -= xh->vel.x * drag * dt;
	xh->vel.y -= xh->vel.y * drag * dt;

	xh->pos.x += xh->vel.x * dt;
	xh->pos.y += xh->vel.y * dt;

	clampXhair(xh);

	return 0;
}


LastShot checkShot(Crosshair xh, bool on_target)
{
	if(IsMouseButtonPressed(xh.key_shoot))
	{
		if(on_target)
		{
			printf("HIT!\n");
			return HIT;
		}
		printf("MISS!\n");
		return MISS;
	}
	return NO_SHOT;
}

void enterQuickdraw()
{
	

	Crosshair p1xh = (Crosshair){
		(Vector2){WIDTH*0.1, HEIGHT/2.},
		(Vector2){0,0},
		25,
		5,
		RED,
		.key_up = KEY_W,
		.key_down = KEY_S,
		.key_right = KEY_D,
		.key_left = KEY_A,
		.key_shoot = MOUSE_LEFT_BUTTON,
		.shot = NO_SHOT,
		false,
	};


	Crosshair p2xh = (Crosshair){
		(Vector2){WIDTH*0.9, HEIGHT/2.},
		(Vector2){0,0},
		25,
		5,
		BLUE,
		.key_up = KEY_UP,
		.key_down = KEY_DOWN,
		.key_right = KEY_RIGHT,
		.key_left = KEY_LEFT,
		.key_shoot = MOUSE_RIGHT_BUTTON,
		.shot = NO_SHOT,
		false,
	};

	float target_rad = 100.;

	Target p1tg = (Target){
		(Vector2){(float)GetRandomValue(0+target_rad,(WIDTH/2.)-target_rad),(float)GetRandomValue(0+target_rad,HEIGHT-target_rad)},
		target_rad,
		RED,
	};

	Target p2tg = (Target){
		(Vector2){(float)GetRandomValue((WIDTH/2.)+target_rad,(WIDTH)-target_rad),(float)GetRandomValue(0+target_rad,HEIGHT-target_rad)},
		target_rad,
		BLUE,
	};

	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{


		float dt = GetFrameTime();
		if(!p1xh.locked)
		{
			updateCrossHair(&p1xh,dt);
		}
		if(!p2xh.locked)
		{
			updateCrossHair(&p2xh,dt);
		}

		bool p1_ontarget = checkXhairOnTarget(&p1xh,p2tg);
		bool p2_ontarget = checkXhairOnTarget(&p2xh,p1tg);
		p1xh.shot = checkShot(p1xh,p1_ontarget);
		p2xh.shot = checkShot(p2xh,p2_ontarget);

		//need to penalize for a miss

		BeginDrawing();
		DrawFPS(0,0);
		ClearBackground(BLACK);

		if(p1_ontarget)
		{
			DrawCircle(p2tg.pos.x,p2tg.pos.y,p2tg.radius,GREEN);
		}
		else
		{
			DrawCircle(p2tg.pos.x,p2tg.pos.y,p2tg.radius,p2tg.col);
		}

		if(p2_ontarget)
		{
			DrawCircle(p1tg.pos.x,p1tg.pos.y,p1tg.radius,GREEN);
		}
		else
		{
			DrawCircle(p1tg.pos.x,p1tg.pos.y,p1tg.radius,p1tg.col);
		}

		drawCrosshair(p1xh);
		drawCrosshair(p2xh);

		EndDrawing();
	}
}
