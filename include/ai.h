#ifndef AI_H
#define AI_H

#include "game.h"

typedef enum {
    AI_DIFFICULTY_RANDOM,
    AI_DIFFICULTY_EASY,
    AI_DIFFICULTY_MEDIUM,
    AI_DIFFICULTY_HARD
} AIDifficulty;

void AI_MakeMove(Game *game, AIDifficulty difficulty);

#endif // AI_H