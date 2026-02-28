#include "raylib.h"
#include <stdio.h>

typedef enum {
    STATE_TOWN = 0,
    STATE_MINIGAME_SALOON,
    STATE_MINIGAME_SHOOTING,
    STATE_MINIGAME_POKER
} GameState;

typedef struct {
    Rectangle rect;
    const char *name;
    GameState targetState;
} Building;

typedef struct {
    Rectangle rect;
    float speed;
} Player;

// Forward declarations for mini-games
GameState RunSaloonMinigame(void);
GameState RunShootingMinigame(void);
GameState RunPokerMinigame(void);

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Wild West Hub - High Noon Town");

    // Player setup
    Player player = {0};
    player.rect = (Rectangle){ 380, 350, 32, 48 };
    player.speed = 200.0f;

    // Buildings in the town
    Building buildings[3] = {
        { (Rectangle){ 100, 150, 120, 80 }, "Saloon",   STATE_MINIGAME_SALOON   },
        { (Rectangle){ 340, 120, 120, 80 }, "Shooting", STATE_MINIGAME_SHOOTING },
        { (Rectangle){ 580, 150, 120, 80 }, "Poker",    STATE_MINIGAME_POKER    }
    };

    GameState state = STATE_TOWN;
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (state)
        {
            case STATE_TOWN:
            {
                float dt = GetFrameTime();

                // --- Input & movement ---
                Vector2 move = { 0 };
                if (IsKeyDown(KEY_W)) move.y -= 1;
                if (IsKeyDown(KEY_S)) move.y += 1;
                if (IsKeyDown(KEY_A)) move.x -= 1;
                if (IsKeyDown(KEY_D)) move.x += 1;

                if (move.x != 0 || move.y != 0) {
                    float len = sqrtf(move.x*move.x + move.y*move.y);
                    move.x /= len;
                    move.y /= len;
                }

                player.rect.x += move.x * player.speed * dt;
                player.rect.y += move.y * player.speed * dt;

                // Keep player in bounds
                if (player.rect.x < 0) player.rect.x = 0;
                if (player.rect.y < 0) player.rect.y = 0;
                if (player.rect.x + player.rect.width > screenWidth)
                    player.rect.x = screenWidth - player.rect.width;
                if (player.rect.y + player.rect.height > screenHeight)
                    player.rect.y = screenHeight - player.rect.height;

                // --- Check building interaction ---
                GameState nextState = STATE_TOWN;
                const char *prompt = NULL;

                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionRecs(player.rect, buildings[i].rect)) {
                        prompt = TextFormat("Press E to enter %s", buildings[i].name);
                        if (IsKeyPressed(KEY_E)) {
                            nextState = buildings[i].targetState;
                        }
                    }
                }

                if (nextState != STATE_TOWN) {
                    state = nextState;
                    break;
                }

                // --- Draw town ---
                BeginDrawing();
                    ClearBackground((Color){ 210, 180, 140, 255 }); // dusty tan

                    // Ground / simple town background
                    DrawText("High Noon Town", 10, 10, 30, DARKBROWN);
                    DrawText("WASD to move, E to enter buildings", 10, 50, 20, BROWN);

                    // Draw buildings
                    for (int i = 0; i < 3; i++) {
                        DrawRectangleRec(buildings[i].rect, (Color){ 139, 69, 19, 255 }); // brown
                        DrawRectangleLinesEx(buildings[i].rect, 2, BLACK);
                        int textWidth = MeasureText(buildings[i].name, 20);
                        DrawText(buildings[i].name,
                                 buildings[i].rect.x + buildings[i].rect.width/2 - textWidth/2,
                                 buildings[i].rect.y + buildings[i].rect.height/2 - 10,
                                 20, RAYWHITE);
                    }

                    // Draw player
                    DrawRectangleRec(player.rect, (Color){ 200, 0, 0, 255 });

                    // Interaction prompt
                    if (prompt != NULL) {
                        int w = MeasureText(prompt, 20);
                        DrawText(prompt, screenWidth/2 - w/2, screenHeight - 40, 20, BLACK);
                    }

                EndDrawing();
            } break;

            case STATE_MINIGAME_SALOON:
                state = RunSaloonMinigame();
                break;

            case STATE_MINIGAME_SHOOTING:
                state = RunShootingMinigame();
                break;

            case STATE_MINIGAME_POKER:
                state = RunPokerMinigame();
                break;
        }
    }

    CloseWindow();
    return 0;
}

// ---------------- Mini-game stubs ----------------

// Each mini-game runs its own loop and returns STATE_TOWN when done.
// You can replace the internals with your real gameplay.

GameState RunSaloonMinigame(void)
{
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
            return STATE_TOWN;
        }

        BeginDrawing();
            ClearBackground((Color){ 80, 40, 20, 255 });
            DrawText("Saloon Minigame", 200, 150, 40, RAYWHITE);
            DrawText("Press ESC or BACKSPACE to return to town", 120, 220, 20, RAYWHITE);
        EndDrawing();
    }
    return STATE_TOWN;
}

GameState RunShootingMinigame(void)
{
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
            return STATE_TOWN;
        }

        BeginDrawing();
            ClearBackground(DARKGREEN);
            DrawText("Shooting Range Minigame", 150, 150, 40, RAYWHITE);
            DrawText("Press ESC or BACKSPACE to return to town", 120, 220, 20, RAYWHITE);
        EndDrawing();
    }
    return STATE_TOWN;
}

GameState RunPokerMinigame(void)
{
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
            return STATE_TOWN;
        }

        BeginDrawing();
            ClearBackground((Color){ 0, 80, 0, 255 });
            DrawText("Poker Minigame", 230, 150, 40, RAYWHITE);
            DrawText("Press ESC or BACKSPACE to return to town", 120, 220, 20, RAYWHITE);
        EndDrawing();
    }
    return STATE_TOWN;
}