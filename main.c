#include <stdio.h>
#include <raylib.h>
#include "lib/grid.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CELL_SIZE 40
#define NUM_CELLS_X 15
#define NUM_CELLS_Y 15
#define NUM_PLAYERS 2
#define PLAYER_COLORS (Color[]){RED, BLUE}

void GameStartup();

void GameUpdate();

void GameRender();

void GameShutdown();

Grid *grid;
int activePlayer;

int offsetX = 0;
int offsetY = 0;

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RPG");
    SetTargetFPS(120);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    GameStartup();

    activePlayer = 0;

    while(!WindowShouldClose()) {
        GameUpdate();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        GameRender();
        EndDrawing();
    }

    GameShutdown();

    CloseWindow();

    return 0;
}

void GameStartup() {
    grid = grid_create(NUM_CELLS_X, NUM_CELLS_Y);
    offsetX = (SCREEN_WIDTH - (NUM_CELLS_X * CELL_SIZE)) / 2;
    offsetY = (SCREEN_HEIGHT - (NUM_CELLS_Y * CELL_SIZE)) / 2;
}

void GameUpdate() {
    grid_highlight_edge(grid, GetMouseX(), GetMouseY(), CELL_SIZE, offsetX, offsetY);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool markedEdge = grid_mark_highlighted_edge(grid, activePlayer);
        if (markedEdge) {
            bool markedACell = grid_update_cells(grid);
            if (!markedACell) {
                activePlayer = (activePlayer + 1) % NUM_PLAYERS;
            }
        }
    }
}

void GameRender() {
    grid_render(grid, CELL_SIZE, offsetX, offsetY, PLAYER_COLORS, NUM_PLAYERS, activePlayer);
    DrawCircle(20, 20, 10, PLAYER_COLORS[activePlayer]);

}

void GameShutdown() {
    grid_destroy(grid);
}