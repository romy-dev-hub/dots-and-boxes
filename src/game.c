#include "game.h"
#include "raylib.h"
#include "ai.h"
#include <stdlib.h>
#include <stdio.h>

Game game;

void InitGame(GameMode mode) {
    game.mode = mode;
    game.state = STATE_PLAYING;
    game.current_player = 0;
    game.scores[0] = 0;
    game.scores[1] = 0;
    game.extra_turn = false;
    game.cell_size = 40;
    game.offset_x = 100;
    game.offset_y = 100;
    
    Grid_Init(&game.grid, 5, 5);
    Players_Init(&game);
}

void UpdateGame(void) {
    if (game.state != STATE_PLAYING) return;
    
    if (Game_IsOver(&game.grid)) {
        game.state = STATE_GAME_OVER;
        return;
    }
    
    if (game.players[game.current_player].is_ai) {
        AIDifficulty difficulty = AI_DIFFICULTY_MEDIUM;
        AI_MakeMove(&game, difficulty);
        
        if (Player_ShouldSwitch(0)) {
            Player_Switch(&game);
        }
    } else {
        // Handle player input
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            
            // Convert mouse position to grid coordinates
            int grid_x = (mousePos.x - game.offset_x) / game.cell_size;
            int grid_y = (mousePos.y - game.offset_y) / game.cell_size;
            float rel_x = (mousePos.x - game.offset_x) - grid_x * game.cell_size;
            float rel_y = (mousePos.y - game.offset_y) - grid_y * game.cell_size;
            
            // Determine if click is closer to horizontal or vertical edge
            bool is_horizontal = rel_y < rel_x;
            if (rel_y > game.cell_size - rel_x) {
                is_horizontal = rel_y > rel_x;
            }
            
            int claimed = 0;
            if (is_horizontal) {
                // Click is on a horizontal edge
                if (grid_y >= 0 && grid_y <= game.grid.rows && 
                    grid_x >= 0 && grid_x < game.grid.cols) {
                    if (Grid_set_horizontal(&game.grid, grid_y, grid_x)) {
                        claimed = Box_CheckAndClaimAfterHorizontal(&game.grid, grid_y, grid_x, game.current_player);
                        game.scores[game.current_player] += claimed;
                    }
                }
            } else {
                // Click is on a vertical edge
                if (grid_y >= 0 && grid_y < game.grid.rows && 
                    grid_x >= 0 && grid_x <= game.grid.cols) {
                    if (Grid_set_vertical(&game.grid, grid_y, grid_x)) {
                        claimed = Box_CheckAndClaimAfterVertical(&game.grid, grid_y, grid_x, game.current_player);
                        game.scores[game.current_player] += claimed;
                    }
                }
            }
            
            // Switch player if no boxes were claimed
            if (Player_ShouldSwitch(claimed)) {
                Player_Switch(&game);
            }
        }
    }
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Draw grid
    for (int r = 0; r <= game.grid.rows; r++) {
        for (int c = 0; c < game.grid.cols; c++) {
            int x1 = game.offset_x + c * game.cell_size;
            int y1 = game.offset_y + r * game.cell_size;
            int x2 = x1 + game.cell_size;
            int y2 = y1;
            
            if (r < game.grid.rows && Grid_index_h(&game.grid, r, c) < (game.grid.rows + 1) * game.grid.cols) {
                if (game.grid.h_edges[Grid_index_h(&game.grid, r, c)]) {
                    DrawLine(x1, y1, x2, y2, BLACK);
                } else {
                    DrawLine(x1, y1, x2, y2, LIGHTGRAY);
                }
            }
        }
    }
    
    for (int r = 0; r < game.grid.rows; r++) {
        for (int c = 0; c <= game.grid.cols; c++) {
            int x1 = game.offset_x + c * game.cell_size;
            int y1 = game.offset_y + r * game.cell_size;
            int x2 = x1;
            int y2 = y1 + game.cell_size;
            
            if (c < game.grid.cols && Grid_index_v(&game.grid, r, c) < game.grid.rows * (game.grid.cols + 1)) {
                if (game.grid.v_edges[Grid_index_v(&game.grid, r, c)]) {
                    DrawLine(x1, y1, x2, y2, BLACK);
                } else {
                    DrawLine(x1, y1, x2, y2, LIGHTGRAY);
                }
            }
        }
    }
    
    // Draw dots
    for (int r = 0; r <= game.grid.rows; r++) {
        for (int c = 0; c <= game.grid.cols; c++) {
            int x = game.offset_x + c * game.cell_size;
            int y = game.offset_y + r * game.cell_size;
            DrawCircle(x, y, 3, BLACK);
        }
    }
    
    // Draw boxes
    for (int r = 0; r < game.grid.rows; r++) {
        for (int c = 0; c < game.grid.cols; c++) {
            if (game.grid.owners[r * game.grid.cols + c] != -1) {
                int x = game.offset_x + c * game.cell_size + game.cell_size / 2;
                int y = game.offset_y + r * game.cell_size + game.cell_size / 2;
                Color color = game.players[game.grid.owners[r * game.grid.cols + c]].color;
                DrawRectangle(x - game.cell_size / 2 + 2, y - game.cell_size / 2 + 2, 
                             game.cell_size - 4, game.cell_size - 4, 
                             Fade(color, 0.3f));
            }
        }
    }
    
    // Draw scores
    DrawText(TextFormat("Player 1: %d", game.scores[0]), 10, 10, 20, RED);
    DrawText(TextFormat("Player 2: %d", game.scores[1]), 10, 40, 20, BLUE);
    
    // Draw current player indicator
    if (game.state == STATE_PLAYING) {
        const char *player_text = TextFormat("Current Player: %d", game.current_player + 1);
        DrawText(player_text, 10, 70, 20, game.players[game.current_player].color);
    }
    
    if (game.state == STATE_GAME_OVER) {
        int winner = Game_GetWinner(game.scores);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
        if (winner == -1) {
            DrawText("Game Over: It's a tie!", 250, 250, 30, BLACK);
        } else {
            DrawText(TextFormat("Game Over: Player %d wins!", winner + 1), 250, 250, 30, 
                    game.players[winner].color);
        }
        DrawText("Press R to restart", 280, 300, 20, DARKGRAY);
        
        // Check for restart
        if (IsKeyPressed(KEY_R)) {
            ResetGrid();
        }
    }
    
    EndDrawing();
}

void ResetGrid(void) {
    Grid_Free(&game.grid);
    Grid_Init(&game.grid, 5, 5);
    game.scores[0] = 0;
    game.scores[1] = 0;
    game.current_player = 0;
    game.state = STATE_PLAYING;
}