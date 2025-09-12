#ifndef GRID_H
#define GRID_H

#include <stdbool.h>

typedef struct Grid {
    int rows;                                   // boxes vertical
    int cols;                                   // boxes horizontal
    // horizontal edges: (rows+1) x cols
    bool *h_edges;                              // size (rows+1)*cols
    // vertical edges: rows x (cols+1)
    bool *v_edges;                              // size rows*(cols+1)
    int *owners;                                // rows*cols, -1 none, 0 player, 1 ai
} Grid;

void Grid_Init(Grid *g, int rows, int cols);
void Grid_Free(Grid *g);
int Grid_index_h(Grid *g, int r, int c);
int Grid_index_v(Grid *g, int r, int c);
bool Grid_set_horizontal(Grid *g, int r, int c);
bool Grid_set_vertical(Grid *g, int r, int c);

#endif // GRID_H
