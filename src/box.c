#include "box.h"
#include <stdbool.h>

bool Box_IsComplete(const Grid *g, int row, int col) {
    if (row < 0 || row >= g->rows || col < 0 || col >= g->cols) return false;
    int h_top = Grid_index_h(g, row, col);
    int h_bot = Grid_index_h(g, row + 1, col);
    int v_left = Grid_index_v(g, row, col);
    int v_right = Grid_index_v(g, row, col + 1);
    return g->h_edges[h_top] && g->h_edges[h_bot] && g->v_edges[v_left] && g->v_edges[v_right];
}

void Box_Claim(Grid *g, int row, int col, int player_id) {
    if (row < 0 || row >= g->rows || col < 0 || col >= g->cols) return;
    g->owners[row * g->cols + col] = player_id;
}

int Box_CheckAndClaimAfterHorizontal(Grid *g, int edge_r, int edge_c, int player_id) {
    int claimed = 0;
    if (edge_r > 0) {
        int box_r = edge_r - 1;
        int box_c = edge_c;
        if (g->owners[box_r * g->cols + box_c] == -1 && Box_IsComplete(g, box_r, box_c)) {
            Box_Claim(g, box_r, box_c, player_id);
            claimed++;
        }
    }
    if (edge_r < g->rows) {
        int box_r = edge_r;
        int box_c = edge_c;
        if (g->owners[box_r * g->cols + box_c] == -1 && Box_IsComplete(g, box_r, box_c)) {
            Box_Claim(g, box_r, box_c, player_id);
            claimed++;
        }
    }
    return claimed;
}

int Box_CheckAndClaimAfterVertical(Grid *g, int edge_r, int edge_c, int player_id) {
    int claimed = 0;
    if (edge_c > 0) {
        int box_r = edge_r;
        int box_c = edge_c - 1;
        if (g->owners[box_r * g->cols + box_c] == -1 && Box_IsComplete(g, box_r, box_c)) {
            Box_Claim(g, box_r, box_c, player_id);
            claimed++;
        }
    }
    if (edge_c < g->cols) {
        int box_r = edge_r;
        int box_c = edge_c;
        if (g->owners[box_r * g->cols + box_c] == -1 && Box_IsComplete(g, box_r, box_c)) {
            Box_Claim(g, box_r, box_c, player_id);
            claimed++;
        }
    }
    return claimed;
}