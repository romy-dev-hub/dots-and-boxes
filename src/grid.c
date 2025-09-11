#include "grid.h"
#include <stdlib.h>

void Grid_Init(Grid *g, int rows, int cols){
    g->rows = rows;
    g->cols = cols;
    g->h_edges = calloc((rows + 1) * cols, sizeof(bool));
    g->v_edges = calloc(rows * (cols + 1), sizeof(bool));
    g->owners = malloc(rows * cols * sizeof(int));
    
    for (int i = 0; i < rows * cols; i++){
        g->owners = -1; // no owner yet
    }
}

/*
rows nd cols = number of boxes (so dots = rows + 1 * cols + 1)
h_edges: horizontal edges -> imagine the lines between dots horizontally 
     there are (rows + 1) rows of horizontal lines, each with cols possible edges.
v_edges: vertical edges -> lines between dots vertically
     there are rows rows of boxes, each with (cols + 1) vertical line.
owners: keeps track of who owns each box (size = rows x cols)
*/


void Grid_Free(Grid *g){
    free(g->h_edges);
    free(g->v_edges);
    free(g->owners);
}

//when the game ends, we free the memory 

int Grid_index_h(Grid *g, int r, int c){
    return r * g->cols + c;
}

int Grid_index_v(Grid *g, int r, int c){
    return r * (g->cols + 1) + c;
}

//these are helpers functions. since we store edges in 1d array we need a formula to find indexes.

/*
example: 
horizontal edge at row r = 2, col c = 1, index = 2 * cols + 1
vertical edge at row = 1, col c = 3, index = 1 * (cols + 1) + 3
*/

bool Grid_set_horizontal(Grid *g, int r, int c){
    int ind = Grid_index_h(g, r, c);
    if (g->h_edges[ind]) return false;  //already taken
    g->h_edges[ind] = true;
    return true;
}

bool Grid_set_vertical(Grid *g, int r, int c){
    int ind = Grid_index_v(g, r, c);
    if (g->v_edges[ind]) return false;  //already taken
    g->v_edges[ind] = true;
    return true;
}

/*
these funtions are used when a player clicks on an edge:
   if the edge is already taken -> returns false
   if it's free -> mark it as drawn (true) and return true
*/

int Grid_claimed_box(Grid *g, int r, int c){
    int h_top = Grid_index_h(g, r, c);
    int h_bot =  Grid_index_h(g, r + 1, c);
    int v_left = Grid_index_v(g, r, c);
    int v_right = Grid_index_v(g, r, c + 1);

    if (g->h_edges[h_bot] && g->h_edges[h_bot] && g->v_edges[v_left] && g->v_edges[v_right]){
        return 1;  //box is completed
    }
    
    return 0;
}

/*
to check if a box (r, c) is claimed:

   we look at 4 edges:
      top horizontal 
      bottom horizontal 
      left vertical
      right vertical
   if all 4 are true -> that box is complete -> return 1
   otherwise -> return 0

*/