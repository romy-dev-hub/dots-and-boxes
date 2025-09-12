#ifndef GAME_H
#define GAME_H

#include "grid.h"
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

// Full definition of Game struct
typedef struct Game {
    GameMode mode;
    GameState state;
    Grid grid;
    int current_player;
    int scores[2];
    bool extra_turn;
    int cell_size;
    int offset_x, offset_y;
    Player players[2];
} Game;

extern Game game;

void InitGame(GameMode mode);
void UpdateGame(void);
void DrawGame(void);
void ResetGrid(void);

#endif // GAME_H