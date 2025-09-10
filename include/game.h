#ifndef GAME_H
#define GAME_H

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

void InitGame(GameMode mode);
void UpdateGame(void);
void DrawGame(void);


#endif // GAME_H