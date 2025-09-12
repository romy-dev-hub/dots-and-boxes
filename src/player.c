// player.c
#include "player.h"

void Players_Init(Game *game) {
    game->players[0].id = 0;
    game->players[0].color = RED;
    game->players[0].score = 0;
    game->players[0].is_ai = false;  // Player 1 always human

    game->players[1].id = 1;
    game->players[1].color = BLUE;
    game->players[1].score = 0;

    // Set AI based on mode
    switch (game->mode) {
        case MODE_PVP:
        case MODE_SOLO:  // Treat solo as PVP for now
            game->players[1].is_ai = false;
            break;
        case MODE_PVM:
            game->players[1].is_ai = true;
            break;
        case MODE_MVM:
            game->players[0].is_ai = true;
            game->players[1].is_ai = true;
            break;
    }
}

void Player_Switch(Game *game) {
    game->current_player = 1 - game->current_player;
}

bool Player_ShouldSwitch(int claimed) {
    return claimed == 0;
}

bool Game_IsOver(const Grid *grid) {
    // Game over if all boxes are claimed (or all edges drawn, but boxes is equivalent)
    for (int i = 0; i < grid->rows * grid->cols; i++) {
        if (grid->owners[i] == -1) return false;
    }
    return true;
}

int Game_GetWinner(const int scores[2]) {
    if (scores[0] > scores[1]) return 0;
    if (scores[1] > scores[0]) return 1;
    return -1;  // Tie
}