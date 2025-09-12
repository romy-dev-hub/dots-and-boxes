#ifndef GRID_H
#define GRID_H

#include <stdbool.h>

typedef struct Grid {
    int rows;
    int cols;
    bool *h_edges;
    bool *v_edges;
    int *owners;
} Grid;

void Grid_Init(Grid *g, int rows, int cols);
void Grid_Free(Grid *g);
int Grid_index_h(const Grid *g, int r, int c);
int Grid_index_v(const Grid *g, int r, int c);
bool Grid_set_horizontal(Grid *g, int r, int c);
bool Grid_set_vertical(Grid *g, int r, int c);
int Grid_claimed_box(const Grid *g, int r, int c);

#endif // GRID_H