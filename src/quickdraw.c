#include "../include/quickdraw.h"
#include <stdio.h>




void drawCrosshair(Crosshair xh)
{
	Color xh_col = xh.col;

	if(xh.reload_timer > 0.f)
	{
		xh_col = Fade(xh.col, 0.3f);

		float reload_progress = 1.0f - (xh.reload_timer / MAX_RELOAD_TIME);

		float inner_radius = (xh.length / 2.0f) + 4.0;
		float outer_radius = (xh.length / 2.0f) + 8.0f;
		DrawRing(xh.pos,inner_radius,outer_radius,0,360*reload_progress,32,Fade(xh.col, 0.8f));


	}

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

QuickDrawWinner enterQuickdraw()
{
	QuickDrawWinner wins = {0};
	
	
	printf("in quickdraw\n");

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

	//needs to not clip into stuff
	Target p1tg = (Target){
		(Vector2){(float)GetRandomValue(target_rad,(WIDTH/2.)-target_rad),(float)GetRandomValue(target_rad,HEIGHT-target_rad)},
		target_rad,
		RED,
	};

	Target p2tg = (Target){
		(Vector2){(float)GetRandomValue((WIDTH/2.)+target_rad,(WIDTH)-target_rad),(float)GetRandomValue(0+target_rad,HEIGHT-target_rad)},
		target_rad,
		BLUE,
	};


	int winner = 0;


	float countdown = 3.;
	bool round_start = true;

	QuickDrawState game_state = COUNTDOWN;

	bool p1_ontarget = false;
	bool p2_ontarget = false;
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{
		float dt = GetFrameTime();


		switch(game_state)
		{
			case COUNTDOWN:
				countdown -= dt;
				if(countdown < -0.25)
				{
					game_state = GAMEPLAY;
				}
				break;

			case GAMEPLAY:
				updateCrossHair(&p1xh,dt);
				updateCrossHair(&p2xh,dt);

				p1_ontarget = checkXhairOnTarget(&p1xh,p2tg);
				p2_ontarget = checkXhairOnTarget(&p2xh,p1tg);


				if(p1xh.reload_timer <= 0.0f)
				{
					p1xh.shot = checkShot(p1xh,p1_ontarget);
					if(p1xh.shot == MISS)
					{
						p1xh.reload_timer = MAX_RELOAD_TIME;
					}
				} else {
					p1xh.reload_timer -= dt;
					p1xh.shot = NO_SHOT;
				}

				if(p2xh.reload_timer <= 0.0f)
				{
					p2xh.shot = checkShot(p2xh,p2_ontarget);
					if(p2xh.shot == MISS)
					{
						p2xh.reload_timer = MAX_RELOAD_TIME;
					}
				} else{
					p2xh.reload_timer -= dt;
					p2xh.shot = NO_SHOT;
				}

				
				if(p1xh.shot == HIT && p2xh.shot == HIT)
				{
					game_state = RESULT;
					winner = TIE;
					break;
					//draw
				}

				if(p1xh.shot == HIT && p2xh.shot != HIT)
				{
					game_state = RESULT;
					winner = PLAYER_1_WIN;
					wins.p1wins++;
					break;
					//player 1 wins
				}

				if(p1xh.shot != HIT && p2xh.shot == HIT)
				{
					game_state = RESULT;
					winner = PLAYER_2_WIN;
					wins.p2wins++;
					break;
					//player 2 wins
				}

				break;
			case RESULT:
				if (IsKeyPressed(KEY_SPACE)) 
                {
                    countdown = 3.0f;

                    p1xh.shot = NO_SHOT;
                    p2xh.shot = NO_SHOT;

					p1xh.reload_timer = 0.0f;
					p2xh.reload_timer = 0.0f;

                    p1xh.pos = (Vector2){WIDTH * 0.1, HEIGHT / 2.0};
                    p2xh.pos = (Vector2){WIDTH * 0.9, HEIGHT / 2.0};
                    game_state = COUNTDOWN;
                }
				break;
		}


		//need to penalize for a miss

		BeginDrawing();
		DrawFPS(0,0);
		ClearBackground(BLACK);


		switch(game_state)
		{
			case COUNTDOWN:
			{
				int font_size = 60;
				int text_width;
				if(countdown <= 0 && countdown > -0.25)
				{
					const char* text = "DRAW!";
					text_width = MeasureText(text, font_size);

					DrawText(text, (WIDTH / 2) - (text_width / 2), (HEIGHT / 2) - (font_size / 2), font_size, GREEN);
					break;
				}
				if(countdown > 0)
				{
					const char* text = TextFormat("%d", (int)ceil(countdown));
					text_width = MeasureText(text, font_size);

					DrawText(text, (WIDTH / 2) - (text_width / 2), (HEIGHT / 2) - (font_size / 2), font_size, RED);
				}
				break;
			}
			case GAMEPLAY:
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
				break;
			case RESULT:
			{

				const char* result_text = "TIE!";
				Color result_color = WHITE;

				if (winner == PLAYER_1_WIN) 
				{
					result_text = "Player 1 Wins!";
					result_color = RED;
				} 
				else if (winner == PLAYER_2_WIN) 
				{
					result_text = "Player 2 Wins!";
					result_color = BLUE;
				}

				int font_size = 60;
				int textWidth = MeasureText(result_text, font_size);

				DrawText(result_text, (WIDTH / 2) - (textWidth / 2), (HEIGHT / 2) - (font_size / 2), font_size, result_color);

				const char* playText = "Press [SPACE] to Play Again";
                const char* exitText = "Press [Q] or [ESC] to Exit";

                int playWidth = MeasureText(playText, 20);
                int exitWidth = MeasureText(exitText, 20);

                DrawText(playText, (WIDTH / 2) - (playWidth / 2), (HEIGHT / 2) + 50, 20, LIGHTGRAY);
                DrawText(exitText, (WIDTH / 2) - (exitWidth / 2), (HEIGHT / 2) + 80, 20, DARKGRAY);

				break;
			}
		}


		EndDrawing();
	}

	printf("returning %d %d\n",wins.p1wins,wins.p2wins);
	return wins;


}
