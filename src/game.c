#include "raylib.h"
#include "game.h"
#include <stdlib.h>  
#include <string.h>  // For memset
#include <stdio.h>   // For sprintf

Game game = {0};  // Global init

void InitGame(GameMode mode){
    game.mode = mode;
    game.state = STATE_MENU;
    game.current_player = 0;
    game.scores[0] = game.scores[1] = 0;
    game.extra_turn = false;
    game.cell_size = 80;  // For 3x3 grid: ~240px wide
    game.offset_x = 100;
    game.offset_y = 150;
    ResetGrid();
}

void ResetGrid(void) {
    Grid_Free(&game.grid);
    Grid_Init(&game.grid, 3, 3);  // 3x3 boxes for testing
    game.scores[0] = game.scores[1] = 0;
    game.current_player = 0;
    game.extra_turn = false;
}

void UpdateGame(void){
    switch (game.state) {
        case STATE_MENU:
            if (IsKeyPressed(KEY_ENTER)) {  // Press Enter to start
                game.state = STATE_PLAYING;
                ResetGrid();
            }
            break;
        case STATE_PLAYING:
            // For now, only human input (MODE_PVP or solo as PVP)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                int claimed = 0;

                // Check horizontal edges (tolerance: click near the line)
                for (int r = 0; r <= game.grid.rows; r++) {
                    for (int c = 0; c < game.grid.cols; c++) {
                        float x = game.offset_x + c * game.cell_size;
                        float y = game.offset_y + r * game.cell_size;
                        Rectangle hitbox = {x, y - 5, game.cell_size, 10};
                        if (CheckCollisionPointRec(mouse, hitbox)) {
                            if (Grid_set_horizontal(&game.grid, r, c)) {
                                // Check adjacent boxes for claims
                                if (r > 0) {  // Box above
                                    int box_r = r - 1, box_c = c;
                                    if (game.grid.owners[box_r * game.grid.cols + box_c] == -1 &&
                                        Grid_claimed_box(&game.grid, box_r, box_c)) {
                                        game.grid.owners[box_r * game.grid.cols + box_c] = game.current_player;
                                        claimed++;
                                    }
                                }
                                if (r < game.grid.rows) {  // Box below
                                    int box_r = r, box_c = c;
                                    if (game.grid.owners[box_r * game.grid.cols + box_c] == -1 &&
                                        Grid_claimed_box(&game.grid, box_r, box_c)) {
                                        game.grid.owners[box_r * game.grid.cols + box_c] = game.current_player;
                                        claimed++;
                                    }
                                }
                                game.scores[game.current_player] += claimed;
                                if (claimed == 0) {
                                    game.current_player = 1 - game.current_player;
                                }
                            }
                            break;  // One move per click
                        }
                    }
                }

                // Check vertical edges
                for (int r = 0; r < game.grid.rows; r++) {
                    for (int c = 0; c <= game.grid.cols; c++) {
                        float x = game.offset_x + c * game.cell_size;
                        float y = game.offset_y + r * game.cell_size;
                        Rectangle hitbox = {x - 5, y, 10, game.cell_size};
                        if (CheckCollisionPointRec(mouse, hitbox)) {
                            if (Grid_set_vertical(&game.grid, r, c)) {
                                int claimed = 0;
                                // Check box left (if exists)
                                if (c > 0) {
                                    int box_r = r, box_c = c - 1;
                                    if (game.grid.owners[box_r * game.grid.cols + box_c] == -1 &&
                                        Grid_claimed_box(&game.grid, box_r, box_c)) {
                                        game.grid.owners[box_r * game.grid.cols + box_c] = game.current_player;
                                        claimed++;
                                    }
                                }
                                // Check box right (if exists)
                                if (c < game.grid.cols) {
                                    int box_r = r, box_c = c;
                                    if (game.grid.owners[box_r * game.grid.cols + box_c] == -1 &&
                                        Grid_claimed_box(&game.grid, box_r, box_c)) {
                                        game.grid.owners[box_r * game.grid.cols + box_c] = game.current_player;
                                        claimed++;
                                    }
                                }
                                game.scores[game.current_player] += claimed;
                                if (claimed == 0) {
                                    game.current_player = 1 - game.current_player;
                                }
                            }
                            break;  // One move per click
                        }
                    }
                }

                // Check game over: all edges drawn
                bool all_h = true, all_v = true;
                for (int i = 0; i < (game.grid.rows + 1) * game.grid.cols; i++) if (!game.grid.h_edges[i]) all_h = false;
                for (int i = 0; i < game.grid.rows * (game.grid.cols + 1); i++) if (!game.grid.v_edges[i]) all_v = false;
                if (all_h && all_v) {
                    game.state = STATE_GAME_OVER;
                }
            }
            break;
        case STATE_GAME_OVER:
            if (IsKeyPressed(KEY_R)) {  // R to restart
                game.state = STATE_MENU;
            }
            break;
    }
}

void DrawGame(void){
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (game.state) {
        case STATE_MENU:
            DrawText("Dots and Boxes", 300, 200, 30, DARKGRAY);
            DrawText("Press ENTER to start (3x3 grid)", 200, 250, 20, GRAY);
            break;
        case STATE_PLAYING: {
            // Draw grid dots
            for (int r = 0; r <= game.grid.rows; r++) {
                for (int c = 0; c <= game.grid.cols; c++) {
                    float x = game.offset_x + c * game.cell_size;
                    float y = game.offset_y + r * game.cell_size;
                    DrawCircle(x, y, 4, BLACK);
                }
            }

            // Draw horizontal edges
            for (int r = 0; r <= game.grid.rows; r++) {
                for (int c = 0; c < game.grid.cols; c++) {
                    if (game.grid.h_edges[Grid_index_h(&game.grid, r, c)]) {
                        float x1 = game.offset_x + c * game.cell_size;
                        float y = game.offset_y + r * game.cell_size;
                        DrawLine(x1, y, x1 + game.cell_size, y, BLACK);
                    }
                }
            }

            // Draw vertical edges
            for (int r = 0; r < game.grid.rows; r++) {
                for (int c = 0; c <= game.grid.cols; c++) {
                    if (game.grid.v_edges[Grid_index_v(&game.grid, r, c)]) {
                        float x = game.offset_x + c * game.cell_size;
                        float y1 = game.offset_y + r * game.cell_size;
                        DrawLine(x, y1, x, y1 + game.cell_size, BLACK);
                    }
                }
            }

            // Draw claimed boxes
            for (int r = 0; r < game.grid.rows; r++) {
                for (int c = 0; c < game.grid.cols; c++) {
                    int owner = game.grid.owners[r * game.grid.cols + c];
                    if (owner != -1) {
                        Color col = (owner == 0) ? RED : BLUE;
                        float x = game.offset_x + c * game.cell_size;
                        float y = game.offset_y + r * game.cell_size;
                        DrawRectangle(x, y, game.cell_size, game.cell_size, Fade(col, 0.3f));
                        DrawRectangleLines(x, y, game.cell_size, game.cell_size, col);
                    }
                }
            }

            // UI
            char score_text[64];
            sprintf(score_text, "Player %d: %d | Player %d: %d", 0, game.scores[0], 1, game.scores[1]);
            DrawText(score_text, 10, 10, 20, BLACK);
            DrawText("Current: Player 0", 10, 40, 20, (game.current_player == 0 ? RED : GRAY));
            DrawText("Click near lines to draw", 10, 70, 15, GRAY);
            break;
        }
        case STATE_GAME_OVER: {
            char winner[64];
            if (game.scores[0] > game.scores[1]) strcpy(winner, "Player 0 Wins!");
            else if (game.scores[1] > game.scores[0]) strcpy(winner, "Player 1 Wins!");
            else strcpy(winner, "Tie!");
            DrawText(winner, 300, 200, 30, DARKGREEN);
            DrawText("Press R to restart", 300, 250, 20, GRAY);
            break;
        }
    }

    EndDrawing();
}