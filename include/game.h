#ifndef GAME_H
#define GAME_H
#include "grid.h"
#include <stdbool.h>
#include "player.h"

typedef enum {
    MODE_PVP,
    MODE_PVM,
    MODE_MVM,
    MODE_SOLO,
} GameMode;

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
} GameState;

typedef struct {
    GameMode mode;
    GameState state;
    Grid grid;
    int current_player;  // 0: player1/human, 1: AI/opponent
    int scores[2];       // scores[0] for player 0, etc.
    bool extra_turn;     // If true, current_player plays again after claiming
    int cell_size;       // Pixels per cell, for drawing
    int offset_x, offset_y;  // Grid position on screen
    Player players[2];   // Two players
} Game;

extern Game game;  // Global for simplicity

void InitGame(GameMode mode);
void UpdateGame(void);
void DrawGame(void);
void ResetGrid(void);  // Helper to restart grid

#endif // GAME_H