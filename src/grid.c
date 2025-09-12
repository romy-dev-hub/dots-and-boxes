#include "grid.h"
#include <stdlib.h>
#include <string.h>

void Grid_Init(Grid *g, int rows, int cols) {
    g->rows = rows;
    g->cols = cols;
    g->h_edges = calloc((rows + 1) * cols, sizeof(bool));
    g->v_edges = calloc(rows * (cols + 1), sizeof(bool));
    g->owners = malloc(rows * cols * sizeof(int));
    for (int i = 0; i < rows * cols; i++) {
        g->owners[i] = -1;
    }
}

void Grid_Free(Grid *g) {
    free(g->h_edges);
    free(g->v_edges);
    free(g->owners);
}

int Grid_index_h(const Grid *g, int r, int c) {
    return r * g->cols + c;
}

int Grid_index_v(const Grid *g, int r, int c) {
    return r * (g->cols + 1) + c;
}

bool Grid_set_horizontal(Grid *g, int r, int c) {
    int ind = Grid_index_h(g, r, c);
    if (g->h_edges[ind]) return false;
    g->h_edges[ind] = true;
    return true;
}

bool Grid_set_vertical(Grid *g, int r, int c) {
    int ind = Grid_index_v(g, r, c);
    if (g->v_edges[ind]) return false;
    g->v_edges[ind] = true;
    return true;
}

int Grid_claimed_box(const Grid *g, int r, int c) {
    int h_top = Grid_index_h(g, r, c);
    int h_bot = Grid_index_h(g, r + 1, c);
    int v_left = Grid_index_v(g, r, c);
    int v_right = Grid_index_v(g, r, c + 1);

    if (g->h_edges[h_top] && g->h_edges[h_bot] && g->v_edges[v_left] && g->v_edges[v_right]) {
        return 1;
    }
    return 0;
}