#include <raylib.h>
#include "constants.h"
#include <math.h>
#define PLAYER_1_WIN 1
#define PLAYER_2_WIN 2
#define TIE 3
#define MAX_RELOAD_TIME 1.0f

typedef struct {
	int p1wins;
	int p2wins;
}QuickDrawWinner;

typedef enum
{
	COUNTDOWN,
	GAMEPLAY,
	RESULT,
}QuickDrawState;

typedef enum 
{
	NO_SHOT,
	MISS,
	HIT,
} LastShot;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float length;
    float thickness;
    Color col;
    int key_up;
    int key_down;
    int key_left;
    int key_right;
    int key_shoot;
	LastShot shot;
	bool reloading;
	float reload_timer;
} Crosshair;

typedef struct {
    Vector2 pos;
    float radius;
    Color col;
} Target;


QuickDrawWinner enterQuickdraw();
void drawCrosshair(Crosshair xh);
bool checkXhairOnTarget(Crosshair* xh, Target tg);
void clampXhair(Crosshair* xh);
int updateCrossHair(Crosshair* xh, float dt);
LastShot checkShot(Crosshair xh, bool on_target);
