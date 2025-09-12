#include "ai.h"
#include "box.h"
#include "grid.h"
#include "player.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int type; // 0 for horizontal, 1 for vertical
    int r;
    int c;
} Move;

static void GetValidMoves(const Grid *grid, Move *moves, int *move_count) {
    *move_count = 0;
    for (int r = 0; r <= grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            int index = Grid_index_h(grid, r, c);
            if (!grid->h_edges[index]) {
                moves[*move_count].type = 0;
                moves[*move_count].r = r;
                moves[*move_count].c = c;
                (*move_count)++;
            }
        }
    }
    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c <= grid->cols; c++) {
            int index = Grid_index_v(grid, r, c);
            if (!grid->v_edges[index]) {
                moves[*move_count].type = 1;
                moves[*move_count].r = r;
                moves[*move_count].c = c;
                (*move_count)++;
            }
        }
    }
}

static int SimulateMove(Grid *grid, Move move) {
    int claimed = 0;
    
    if (move.type == 0) {
        // Horizontal move
        int index = Grid_index_h(grid, move.r, move.c);
        if (grid->h_edges[index]) return 0; // Already set
        
        grid->h_edges[index] = true;
        
        // Check boxes above and below
        if (move.r > 0) {
            int box_r = move.r - 1;
            int box_c = move.c;
            if (grid->owners[box_r * grid->cols + box_c] == -1 && Box_IsComplete(grid, box_r, box_c)) {
                claimed++;
            }
        }
        if (move.r < grid->rows) {
            int box_r = move.r;
            int box_c = move.c;
            if (grid->owners[box_r * grid->cols + box_c] == -1 && Box_IsComplete(grid, box_r, box_c)) {
                claimed++;
            }
        }
    } else {
        // Vertical move
        int index = Grid_index_v(grid, move.r, move.c);
        if (grid->v_edges[index]) return 0; // Already set
        
        grid->v_edges[index] = true;
        
        // Check boxes to the left and right
        if (move.c > 0) {
            int box_r = move.r;
            int box_c = move.c - 1;
            if (grid->owners[box_r * grid->cols + box_c] == -1 && Box_IsComplete(grid, box_r, box_c)) {
                claimed++;
            }
        }
        if (move.c < grid->cols) {
            int box_r = move.r;
            int box_c = move.c;
            if (grid->owners[box_r * grid->cols + box_c] == -1 && Box_IsComplete(grid, box_r, box_c)) {
                claimed++;
            }
        }
    }
    
    return claimed;
}

static void AI_Random(Game *game) {
    Move moves[200];
    int move_count = 0;
    GetValidMoves(&game->grid, moves, &move_count);
    
    if (move_count == 0) return;
    
    int random_index = rand() % move_count;
    Move selected_move = moves[random_index];
    
    if (selected_move.type == 0) {
        Grid_set_horizontal(&game->grid, selected_move.r, selected_move.c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, selected_move.r, selected_move.c, game->current_player);
    } else {
        Grid_set_vertical(&game->grid, selected_move.r, selected_move.c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, selected_move.r, selected_move.c, game->current_player);
    }
}

static void AI_Easy(Game *game) {
    Move moves[200];
    int move_count = 0;
    GetValidMoves(&game->grid, moves, &move_count);
    
    if (move_count == 0) return;
    
    // Try to find a move that completes a box
    for (int i = 0; i < move_count; i++) {
        Grid temp_grid;
        temp_grid.rows = game->grid.rows;
        temp_grid.cols = game->grid.cols;
        
        // Copy edges and owners
        temp_grid.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        temp_grid.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        temp_grid.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
        
        memcpy(temp_grid.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        memcpy(temp_grid.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        memcpy(temp_grid.owners, game->grid.owners, game->grid.rows * game->grid.cols * sizeof(int));
        
        int claimed = SimulateMove(&temp_grid, moves[i]);
        
        free(temp_grid.h_edges);
        free(temp_grid.v_edges);
        free(temp_grid.owners);
        
        if (claimed > 0) {
            // This move completes at least one box
            if (moves[i].type == 0) {
                Grid_set_horizontal(&game->grid, moves[i].r, moves[i].c);
                game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, moves[i].r, moves[i].c, game->current_player);
            } else {
                Grid_set_vertical(&game->grid, moves[i].r, moves[i].c);
                game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, moves[i].r, moves[i].c, game->current_player);
            }
            return;
        }
    }
    
    // If no box-completing move found, make a random move
    AI_Random(game);
}

static void AI_Medium(Game *game) {
    Move moves[200];
    int move_count = 0;
    GetValidMoves(&game->grid, moves, &move_count);
    
    if (move_count == 0) return;
    
    // Try to find a move that doesn't give the opponent a chance to complete a box
    for (int i = 0; i < move_count; i++) {
        Grid temp_grid;
        temp_grid.rows = game->grid.rows;
        temp_grid.cols = game->grid.cols;
        
        // Copy edges and owners
        temp_grid.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        temp_grid.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        temp_grid.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
        
        memcpy(temp_grid.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        memcpy(temp_grid.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        memcpy(temp_grid.owners, game->grid.owners, game->grid.rows * game->grid.cols * sizeof(int));
        
        int claimed = SimulateMove(&temp_grid, moves[i]);
        
        // If this move doesn't complete any boxes, it's safe
        if (claimed == 0) {
            if (moves[i].type == 0) {
                Grid_set_horizontal(&game->grid, moves[i].r, moves[i].c);
                game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, moves[i].r, moves[i].c, game->current_player);
            } else {
                Grid_set_vertical(&game->grid, moves[i].r, moves[i].c);
                game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, moves[i].r, moves[i].c, game->current_player);
            }
            
            free(temp_grid.h_edges);
            free(temp_grid.v_edges);
            free(temp_grid.owners);
            return;
        }
        
        free(temp_grid.h_edges);
        free(temp_grid.v_edges);
        free(temp_grid.owners);
    }
    
    // If no safe move found, use the easy AI strategy
    AI_Easy(game);
}

static void AI_Hard(Game *game) {
    // For a hard AI, we would implement a minimax algorithm with alpha-beta pruning
    // For simplicity, we'll use the medium strategy for now
    AI_Medium(game);
}

void AI_MakeMove(Game *game, AIDifficulty difficulty) {
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    
    switch (difficulty) {
        case AI_DIFFICULTY_RANDOM:
            AI_Random(game);
            break;
        case AI_DIFFICULTY_EASY:
            AI_Easy(game);
            break;
        case AI_DIFFICULTY_MEDIUM:
            AI_Medium(game);
            break;
        case AI_DIFFICULTY_HARD:
            AI_Hard(game);
            break;
    }
}