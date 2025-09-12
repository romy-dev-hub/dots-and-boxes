#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "grid.h"
#include "game.h"

typedef struct {
    int id;
    Color color;
    int score;
    bool is_ai;
} Player;

void Players_Init(Game *game);
void Player_Switch(Game *game);
bool Player_ShouldSwitch(int claimed);
bool Game_IsOver(const Grid *grid);
int Game_GetWinner(const int scores[2]);

#endif // PLAYER_H