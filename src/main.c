#include "raylib.h"
#include "game.h"

int main (void){
    InitWindow(800, 600, "Dots and Boxes");
    SetTargetFPS(60);

    InitGame(MODE_SOLO);

    while(!WindowShouldClose()){
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    return 0;
}