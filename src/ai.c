// ai.c
#include "ai.h"
#include "box.h"
#include "grid.h"
#include <stdlib.h>  // For rand
#include <time.h>    // For srand

// Helper: Get all valid moves (unset edges) without modifying grid
static void GetValidMoves(const Grid *grid, int *h_moves, int *v_moves, int *h_count, int *v_count) {
    *h_count = 0;
    *v_count = 0;
    for (int r = 0; r <= grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            int index = Grid_index_h(grid, r, c);
            if (!grid->h_edges[index]) {
                h_moves[(*h_count)++] = r * grid->cols + c;
            }
        }
    }
    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c <= grid->cols; c++) {
            int index = Grid_index_v(grid, r, c);
            if (!grid->v_edges[index]) {
                v_moves[(*v_count)++] = r * (grid->cols + 1) + c;
            }
        }
    }
}

// Helper: Simulate a move and count claimed boxes without changing grid
static int SimulateClaimHorizontal(const Grid *grid, int r, int c, int player_id) {
    Grid temp = *grid;
    temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
    temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
    temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
    memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
    memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
    memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));

    bool set = true;
    int index = Grid_index_h(&temp, r, c);
    temp.h_edges[index] = true;

    int claimed = 0;
    if (r > 0) {
        int box_r = r - 1, box_c = c;
        if (temp.owners[box_r * temp.cols + box_c] == -1 && Box_IsComplete(&temp, box_r, box_c)) {
            claimed++;
        }
    }
    if (r < temp.rows) {
        int box_r = r, box_c = c;
        if (temp.owners[box_r * temp.cols + box_c] == -1 && Box_IsComplete(&temp, box_r, box_c)) {
            claimed++;
        }
    }

    free(temp.h_edges);
    free(temp.v_edges);
    free(temp.owners);
    return claimed;
}

static int SimulateClaimVertical(const Grid *grid, int r, int c, int player_id) {
    Grid temp = *grid;
    temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
    temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
    temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
    memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
    memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
    memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));

    bool set = true;
    int index = Grid_index_v(&temp, r, c);
    temp.v_edges[index] = true;

    int claimed = 0;
    if (c > 0) {
        int box_r = r, box_c = c - 1;
        if (temp.owners[box_r * temp.cols + box_c] == -1 && Box_IsComplete(&temp, box_r, box_c)) {
            claimed++;
        }
    }
    if (c < temp.cols) {
        int box_r = r, box_c = c;
        if (temp.owners[box_r * temp.cols + box_c] == -1 && Box_IsComplete(&temp, box_r, box_c)) {
            claimed++;
        }
    }

    free(temp.h_edges);
    free(temp.v_edges);
    free(temp.owners);
    return claimed;
}

// Helper: Evaluate game state score (AI's boxes - opponent's boxes)
static int EvaluateState(const Grid *grid, int ai_player) {
    int ai_boxes = 0, opp_boxes = 0;
    for (int i = 0; i < grid->rows * grid->cols; i++) {
        if (grid->owners[i] == ai_player) ai_boxes++;
        else if (grid->owners[i] == 1 - ai_player) opp_boxes++;
    }
    return ai_boxes - opp_boxes;  // Positive favors AI, negative favors opponent
}

// Minimax with limited depth
static int Minimax(Grid *grid, int depth, bool is_maximizing, int ai_player, int max_depth) {
    if (depth == 0 || Game_IsOver(grid)) {
        return EvaluateState(grid, ai_player);
    }

    int best_score;
    if (is_maximizing) {
        best_score = -100;  // Arbitrary low value
        int h_moves[100], v_moves[100];
        int h_count = 0, v_count = 0;
        GetValidMoves(grid, h_moves, v_moves, &h_count, &v_count);

        for (int i = 0; i < h_count; i++) {
            int r = h_moves[i] / grid->cols;
            int c = h_moves[i] % grid->cols;
            Grid temp = *grid;
            temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
            temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
            temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
            memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
            memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
            memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));
            temp.h_edges[Grid_index_h(&temp, r, c)] = true;
            int claimed = SimulateClaimHorizontal(&temp, r, c, ai_player);
            for (int br = 0; br < temp.rows; br++) {
                for (int bc = 0; bc < temp.cols; bc++) {
                    if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                        temp.owners[br * temp.cols + bc] = ai_player;
                    }
                }
            }
            int score = Minimax(&temp, depth - 1, false, ai_player, max_depth);
            best_score = (score > best_score) ? score : best_score;
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }

        for (int i = 0; i < v_count; i++) {
            int r = v_moves[i] / (grid->cols + 1);
            int c = v_moves[i] % (grid->cols + 1);
            Grid temp = *grid;
            temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
            temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
            temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
            memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
            memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
            memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));
            temp.v_edges[Grid_index_v(&temp, r, c)] = true;
            int claimed = SimulateClaimVertical(&temp, r, c, ai_player);
            for (int br = 0; br < temp.rows; br++) {
                for (int bc = 0; bc < temp.cols; bc++) {
                    if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                        temp.owners[br * temp.cols + bc] = ai_player;
                    }
                }
            }
            int score = Minimax(&temp, depth - 1, false, ai_player, max_depth);
            best_score = (score > best_score) ? score : best_score;
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }
    } else {
        best_score = 100;  // Arbitrary high value
        int h_moves[100], v_moves[100];
        int h_count = 0, v_count = 0;
        GetValidMoves(grid, h_moves, v_moves, &h_count, &v_count);

        for (int i = 0; i < h_count; i++) {
            int r = h_moves[i] / grid->cols;
            int c = h_moves[i] % grid->cols;
            Grid temp = *grid;
            temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
            temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
            temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
            memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
            memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
            memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));
            temp.h_edges[Grid_index_h(&temp, r, c)] = true;
            int claimed = SimulateClaimHorizontal(&temp, r, c, 1 - ai_player);
            for (int br = 0; br < temp.rows; br++) {
                for (int bc = 0; bc < temp.cols; bc++) {
                    if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                        temp.owners[br * temp.cols + bc] = 1 - ai_player;
                    }
                }
            }
            int score = Minimax(&temp, depth - 1, true, ai_player, max_depth);
            best_score = (score < best_score) ? score : best_score;
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }

        for (int i = 0; i < v_count; i++) {
            int r = v_moves[i] / (grid->cols + 1);
            int c = v_moves[i] % (grid->cols + 1);
            Grid temp = *grid;
            temp.h_edges = malloc((grid->rows + 1) * grid->cols * sizeof(bool));
            temp.v_edges = malloc(grid->rows * (grid->cols + 1) * sizeof(bool));
            temp.owners = malloc(grid->rows * grid->cols * sizeof(int));
            memcpy(temp.h_edges, grid->h_edges, (grid->rows + 1) * grid->cols * sizeof(bool));
            memcpy(temp.v_edges, grid->v_edges, grid->rows * (grid->cols + 1) * sizeof(bool));
            memcpy(temp.owners, grid->owners, grid->rows * grid->cols * sizeof(int));
            temp.v_edges[Grid_index_v(&temp, r, c)] = true;
            int claimed = SimulateClaimVertical(&temp, r, c, 1 - ai_player);
            for (int br = 0; br < temp.rows; br++) {
                for (int bc = 0; bc < temp.cols; bc++) {
                    if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                        temp.owners[br * temp.cols + bc] = 1 - ai_player;
                    }
                }
            }
            int score = Minimax(&temp, depth - 1, true, ai_player, max_depth);
            best_score = (score < best_score) ? score : best_score;
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }
    }
    return best_score;
}

// Random AI: Picks any valid move
static void AI_Random(Game *game) {
    int h_moves[100], v_moves[100];
    int h_count = 0, v_count = 0;
    GetValidMoves(&game->grid, h_moves, v_moves, &h_count, &v_count);
    int total_moves = h_count + v_count;
    if (total_moves == 0) return;

    int move_idx = rand() % total_moves;
    if (move_idx < h_count) {
        int r = h_moves[move_idx] / game->grid.cols;
        int c = h_moves[move_idx] % game->grid.cols;
        Grid_set_horizontal(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, r, c, game->current_player);
    } else {
        int move_idx_v = move_idx - h_count;
        int r = v_moves[move_idx_v] / (game->grid.cols + 1);
        int c = v_moves[move_idx_v] % (game->grid.cols + 1);
        Grid_set_vertical(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, r, c, game->current_player);
    }
}

// Easy AI: Greedy - Prioritize completing own boxes
static void AI_Easy(Game *game) {
    int h_moves[100], v_moves[100];
    int h_count = 0, v_count = 0;
    GetValidMoves(&game->grid, h_moves, v_moves, &h_count, &v_count);
    int best_score = -1;
    int best_move_type = 0;  // 0 for h, 1 for v
    int best_move_idx = -1;

    for (int i = 0; i < h_count; i++) {
        int r = h_moves[i] / game->grid.cols;
        int c = h_moves[i] % game->grid.cols;
        int score = SimulateClaimHorizontal(&game->grid, r, c, game->current_player);
        if (score > best_score) {
            best_score = score;
            best_move_type = 0;
            best_move_idx = i;
        }
    }

    for (int i = 0; i < v_count; i++) {
        int r = v_moves[i] / (game->grid.cols + 1);
        int c = v_moves[i] % (game->grid.cols + 1);
        int score = SimulateClaimVertical(&game->grid, r, c, game->current_player);
        if (score > best_score) {
            best_score = score;
            best_move_type = 1;
            best_move_idx = i;
        }
    }

    if (best_move_type == 0 && best_move_idx != -1) {
        int r = h_moves[best_move_idx] / game->grid.cols;
        int c = h_moves[best_move_idx] % game->grid.cols;
        Grid_set_horizontal(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, r, c, game->current_player);
    } else if (best_move_idx != -1) {
        int r = v_moves[best_move_idx] / (game->grid.cols + 1);
        int c = v_moves[best_move_idx] % (game->grid.cols + 1);
        Grid_set_vertical(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, r, c, game->current_player);
    } else {
        AI_Random(game);
    }
}

// Medium AI: Lookahead - Prioritize own boxes, then block opponent
static void AI_Medium(Game *game) {
    int h_moves[100], v_moves[100];
    int h_count = 0, v_count = 0;
    GetValidMoves(&game->grid, h_moves, v_moves, &h_count, &v_count);
    int best_score = -1;
    int best_move_type = 0;
    int best_move_idx = -1;

    int opponent = 1 - game->current_player;

    for (int i = 0; i < h_count; i++) {
        int r = h_moves[i] / game->grid.cols;
        int c = h_moves[i] % game->grid.cols;
        int own_score = SimulateClaimHorizontal(&game->grid, r, c, game->current_player);
        int block_score = 0;
        if (own_score == 0) {
            Grid temp = *game->grid;
            temp.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
            temp.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
            temp.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
            memcpy(temp.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
            memcpy(temp.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
            memcpy(temp.owners, game->grid.owners, game->grid.rows * game->grid.cols * sizeof(int));
            temp.h_edges[Grid_index_h(&temp, r, c)] = true;
            for (int or = 0; or <= temp.rows; or++) {
                for (int oc = 0; oc < temp.cols; oc++) {
                    if (SimulateClaimHorizontal(&temp, or, oc, opponent) > 0) block_score = 1;
                }
            }
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }
        int total_score = own_score * 2 + block_score;
        if (total_score > best_score) {
            best_score = total_score;
            best_move_type = 0;
            best_move_idx = i;
        }
    }

    for (int i = 0; i < v_count; i++) {
        int r = v_moves[i] / (game->grid.cols + 1);
        int c = v_moves[i] % (game->grid.cols + 1);
        int own_score = SimulateClaimVertical(&game->grid, r, c, game->current_player);
        int block_score = 0;
        if (own_score == 0) {
            Grid temp = *game->grid;
            temp.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
            temp.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
            temp.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
            memcpy(temp.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
            memcpy(temp.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
            memcpy(temp.owners, game->grid.owners, game->grid.rows * grid->grid.cols * sizeof(int));
            temp.v_edges[Grid_index_v(&temp, r, c)] = true;
            for (int or = 0; or < temp.rows; or++) {
                for (int oc = 0; oc <= temp.cols; oc++) {
                    if (SimulateClaimVertical(&temp, or, oc, opponent) > 0) block_score = 1;
                }
            }
            free(temp.h_edges);
            free(temp.v_edges);
            free(temp.owners);
        }
        int total_score = own_score * 2 + block_score;
        if (total_score > best_score) {
            best_score = total_score;
            best_move_type = 1;
            best_move_idx = i;
        }
    }

    if (best_move_type == 0 && best_move_idx != -1) {
        int r = h_moves[best_move_idx] / game->grid.cols;
        int c = h_moves[best_move_idx] % game->grid.cols;
        Grid_set_horizontal(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, r, c, game->current_player);
    } else if (best_move_idx != -1) {
        int r = v_moves[best_move_idx] / (game->grid.cols + 1);
        int c = v_moves[best_move_idx] % (game->grid.cols + 1);
        Grid_set_vertical(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, r, c, game->current_player);
    } else {
        AI_Random(game);
    }
}

// Hard AI: Minimax with depth limit
static void AI_Hard(Game *game) {
    int h_moves[100], v_moves[100];
    int h_count = 0, v_count = 0;
    GetValidMoves(&game->grid, h_moves, v_moves, &h_count, &v_count);
    int best_score = -100;  // Arbitrary low value
    int best_move_type = 0;
    int best_move_idx = -1;

    for (int i = 0; i < h_count; i++) {
        int r = h_moves[i] / game->grid.cols;
        int c = h_moves[i] % game->grid.cols;
        Grid temp = *game->grid;
        temp.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        temp.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        temp.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
        memcpy(temp.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        memcpy(temp.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        memcpy(temp.owners, game->grid.owners, game->grid.rows * game->grid.cols * sizeof(int));
        temp.h_edges[Grid_index_h(&temp, r, c)] = true;
        int claimed = SimulateClaimHorizontal(&temp, r, c, game->current_player);
        for (int br = 0; br < temp.rows; br++) {
            for (int bc = 0; bc < temp.cols; bc++) {
                if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                    temp.owners[br * temp.cols + bc] = game->current_player;
                }
            }
        }
        int score = Minimax(&temp, 1, false, game->current_player, 2);  // Depth 1, max depth 2
        if (score > best_score) {
            best_score = score;
            best_move_type = 0;
            best_move_idx = i;
        }
        free(temp.h_edges);
        free(temp.v_edges);
        free(temp.owners);
    }

    for (int i = 0; i < v_count; i++) {
        int r = v_moves[i] / (game->grid.cols + 1);
        int c = v_moves[i] % (game->grid.cols + 1);
        Grid temp = *game->grid;
        temp.h_edges = malloc((game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        temp.v_edges = malloc(game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        temp.owners = malloc(game->grid.rows * game->grid.cols * sizeof(int));
        memcpy(temp.h_edges, game->grid.h_edges, (game->grid.rows + 1) * game->grid.cols * sizeof(bool));
        memcpy(temp.v_edges, game->grid.v_edges, game->grid.rows * (game->grid.cols + 1) * sizeof(bool));
        memcpy(temp.owners, game->grid.owners, game->grid.rows * game->grid.cols * sizeof(int));
        temp.v_edges[Grid_index_v(&temp, r, c)] = true;
        int claimed = SimulateClaimVertical(&temp, r, c, game->current_player);
        for (int br = 0; br < temp.rows; br++) {
            for (int bc = 0; bc < temp.cols; bc++) {
                if (temp.owners[br * temp.cols + bc] == -1 && Box_IsComplete(&temp, br, bc)) {
                    temp.owners[br * temp.cols + bc] = game->current_player;
                }
            }
        }
        int score = Minimax(&temp, 1, false, game->current_player, 2);
        if (score > best_score) {
            best_score = score;
            best_move_type = 1;
            best_move_idx = i;
        }
        free(temp.h_edges);
        free(temp.v_edges);
        free(temp.owners);
    }

    if (best_move_type == 0 && best_move_idx != -1) {
        int r = h_moves[best_move_idx] / game->grid.cols;
        int c = h_moves[best_move_idx] % game->grid.cols;
        Grid_set_horizontal(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterHorizontal(&game->grid, r, c, game->current_player);
    } else if (best_move_idx != -1) {
        int r = v_moves[best_move_idx] / (game->grid.cols + 1);
        int c = v_moves[best_move_idx] % (game->grid.cols + 1);
        Grid_set_vertical(&game->grid, r, c);
        game->scores[game->current_player] += Box_CheckAndClaimAfterVertical(&game->grid, r, c, game->current_player);
    } else {
        AI_Random(game);
    }
}

void AI_MakeMove(Game *game, AIDifficulty difficulty) {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
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
    if (!Player_ShouldSwitch(0)) Player_Switch(game);  // Switch if no claim
}