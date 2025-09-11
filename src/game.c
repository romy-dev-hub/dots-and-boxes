#include "raylib.h"
#include "game.h"

GameMode currentMode;
GameState currentState;

void InitGame(GameMode mode){
    currentMode = mode;
    currentState = STATE_MENU;
}

void UpdateGame(void){
    // TODO: Implement game update logic based on the current state and mode
}

void DrawGame(void){
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Dots and Boxes - Skeleton", 200, 200, 20, BLACK);
    EndDrawing();
}