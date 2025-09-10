#ifndef GAME_H
#define GAME_H

#include <stdbool.h>


#define WINDOW_W 800
#define WINDOW_H 600


void Game_Init(void);
void Game_Update(void);
void Game_Draw(void);
void Game_Close(void);


#endif // GAME_H