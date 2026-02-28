#include <raylib.h>
#include "constants.h"

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
	bool locked;
} Crosshair;

typedef struct {
    Vector2 pos;
    float radius;
    Color col;
} Target;


void enterQuickdraw();
void drawCrosshair(Crosshair xh);
bool checkXhairOnTarget(Crosshair* xh, Target tg);
void clampXhair(Crosshair* xh);
int updateCrossHair(Crosshair* xh, float dt);
LastShot checkShot(Crosshair xh, bool on_target);
