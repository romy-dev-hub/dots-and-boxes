#ifndef BOX_H
#define BOX_H

#include "grid.h"
#include <stdbool.h>

bool Box_IsComplete(const Grid *g, int row, int col);
void Box_Claim(Grid *g, int row, int col, int player_id);
int Box_CheckAndClaimAfterHorizontal(Grid *g, int edge_r, int edge_c, int player_id);
int Box_CheckAndClaimAfterVertical(Grid *g, int edge_r, int edge_c, int player_id);

#endif // BOX_H