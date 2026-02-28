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

