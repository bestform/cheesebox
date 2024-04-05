//
// Created by mderer on 05.04.24.
//

#include <malloc.h>
#include <raylib.h>
#include "grid.h"

typedef struct Cell {
    bool top;
    bool right;
    bool bottom;
    bool left;
    bool hTop;
    bool hRight;
    bool hBottom;
    bool hLeft;
    bool active;
    int owner;
    int lastHighlightBy;
} Cell;

typedef struct Grid {
    int width;
    int height;
    Cell **cells;
} Grid;

Grid *grid_create(int width, int height) {
    Grid *grid = malloc(sizeof(Grid));
    grid->width = width;
    grid->height = height;
    grid->cells = malloc(sizeof(Cell *) * width);
    for (int i = 0; i < width; i++) {
        grid->cells[i] = malloc(sizeof(Cell) * height);
        for (int j = 0; j < height; j++) {
            Cell *cell = &grid->cells[i][j];
            *cell = (Cell) {
                .top = false,
                .right = false,
                .bottom = false,
                .left = false,
                .hTop = false,
                .hRight = false,
                .hBottom = false,
                .hLeft = false,
                .active = true,
                .owner = -1,
                .lastHighlightBy = -1
            };
        }
    }

    // mark random islands of cells in the middle as inactive
    for (int n = 0; n < 4; n++) {
        int islandWidth = GetRandomValue(1, width / 2);
        int islandHeight = GetRandomValue(1, height / 2);
        int islandX = GetRandomValue(1, width - islandWidth - 2);
        int islandY = GetRandomValue(1, height - islandHeight - 2);
        for (int i = islandX; i < islandX + islandWidth; i++) {
            for (int j = islandY; j < islandY + islandHeight; j++) {
                grid->cells[i][j].active = false;
                grid->cells[i][j].top = true;
                grid->cells[i][j].right = true;
                grid->cells[i][j].bottom = true;
                grid->cells[i][j].left = true;

                // mark the edges of adjacent cells as true
                if (i > 0) {
                    grid->cells[i - 1][j].right = true;
                }
                if (i < width - 1) {
                    grid->cells[i + 1][j].left = true;
                }
                if (j > 0) {
                    grid->cells[i][j - 1].bottom = true;
                }
                if (j < height - 1) {
                    grid->cells[i][j + 1].top = true;
                }
            }
        }
    }

    // mark border cells
    for (int i = 0; i < width; i++) {
        grid->cells[i][0].top = true;
        grid->cells[i][height - 1].bottom = true;
    }

    for (int i = 0; i < height; i++) {
        grid->cells[0][i].left = true;
        grid->cells[width - 1][i].right = true;
    }

    return grid;
}

void grid_destroy(Grid *grid) {
    for (int i = 0; i < grid->width; i++) {
        free(grid->cells[i]);
    }

    free(grid->cells);
}

void grid_render(Grid *grid, int cellSize, int offsetX, int offsetY, Color playerColors[], int numPlayers, int activePlayer) {

    // render a background for the whole grid first
    DrawRectangle(offsetX, offsetY, grid->width * cellSize, grid->height * cellSize, LIGHTGRAY);

    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {

            if (!grid->cells[i][j].active) {
                DrawRectangle(i * cellSize + offsetX, j * cellSize + offsetY, cellSize, cellSize, BLACK);
                continue;
            }

            if (grid->cells[i][j].owner < numPlayers && grid->cells[i][j].owner >= 0) {
                DrawRectangle(i * cellSize + offsetX, j * cellSize + offsetY, cellSize, cellSize, playerColors[grid->cells[i][j].owner]);
            }
            DrawLine(i * cellSize + offsetX, j * cellSize + offsetY, (i + 1) * cellSize + offsetX,
                     j * cellSize + offsetY, GRAY);
            DrawLine((i + 1) * cellSize + offsetX, j * cellSize + offsetY, (i + 1) * cellSize + offsetX,
                     (j + 1) * cellSize + offsetY, GRAY);
            DrawLine((i + 1) * cellSize + offsetX, (j + 1) * cellSize + offsetY, i * cellSize + offsetX,
                     (j + 1) * cellSize + offsetY, GRAY);
            DrawLine(i * cellSize + offsetX, (j + 1) * cellSize + offsetY, i * cellSize + offsetX,
                     j * cellSize + offsetY, GRAY);
        }
    }

    const int lineWidth = 6;

    // draw highlighted edges
    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            if (grid->cells[i][j].hTop && !grid->cells[i][j].top) {
                int x = i * cellSize + offsetX;
                int y = j * cellSize - lineWidth / 2 + offsetY;
                DrawRectangle(x, y, cellSize, lineWidth, playerColors[activePlayer]);
                DrawCircle(x, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, playerColors[activePlayer]);
                DrawCircle(x + cellSize, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, playerColors[activePlayer]);
            }
            if (grid->cells[i][j].hRight && !grid->cells[i][j].right) {
                int x = (i + 1) * cellSize - lineWidth / 2 + offsetX;
                int y = j * cellSize + offsetY;
                DrawRectangle(x, y, lineWidth, cellSize,playerColors[activePlayer]);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y, (float) lineWidth / 2, playerColors[activePlayer]);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y + cellSize, (float) lineWidth / 2, playerColors[activePlayer]);
            }
            if (grid->cells[i][j].hBottom && !grid->cells[i][j].bottom) {
                int x = i * cellSize + offsetX;
                int y = (j + 1) * cellSize - lineWidth / 2 + offsetY;
                DrawRectangle(x, y, cellSize, lineWidth,playerColors[activePlayer]);
                DrawCircle(x, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, playerColors[activePlayer]);
                DrawCircle(x + cellSize, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, playerColors[activePlayer]);
            }
            if (grid->cells[i][j].hLeft && !grid->cells[i][j].left) {
                int x = i * cellSize - lineWidth / 2 + offsetX;
                int y = j * cellSize + offsetY;
                DrawRectangle(x, y, lineWidth, cellSize, playerColors[activePlayer]);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y, (float) lineWidth / 2, playerColors[activePlayer]);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y + cellSize, (float) lineWidth / 2, playerColors[activePlayer]);
            }
        }
    }

    // draw marked edges
    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            if (grid->cells[i][j].top) {
                int x = i * cellSize + offsetX;
                int y = j * cellSize - lineWidth / 2 + offsetY;
                DrawRectangle(x, y, cellSize, lineWidth,BLACK);
                DrawCircle(x, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, BLACK);
                DrawCircle(x + cellSize, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, BLACK);
            }
            if (grid->cells[i][j].right) {
                int x = (i + 1) * cellSize - lineWidth / 2 + offsetX;
                int y = j * cellSize + offsetY;
                DrawRectangle(x, y, lineWidth, cellSize,BLACK);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y, (float) lineWidth / 2, BLACK);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y + cellSize, (float) lineWidth / 2, BLACK);
            }
            if (grid->cells[i][j].bottom) {
                int x = i * cellSize + offsetX;
                int y = (j + 1) * cellSize - lineWidth / 2 + offsetY;
                DrawRectangle(x, y, cellSize, lineWidth,BLACK);
                DrawCircle(x, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, BLACK);
                DrawCircle(x + cellSize, (int) ((float) y + (float) lineWidth / 2), (float) lineWidth / 2, BLACK);
            }
            if (grid->cells[i][j].left) {
                int x = i * cellSize - lineWidth / 2 + offsetX;
                int y = j * cellSize + offsetY;
                DrawRectangle(x, y, lineWidth, cellSize,BLACK);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y, (float) lineWidth / 2, BLACK);
                DrawCircle((int) ((float) x + (float) lineWidth / 2), y + cellSize, (float) lineWidth / 2, BLACK);
            }
        }
    }
}

void grid_highlight_edge(Grid *grid, int mouseX, int mouseY, int cellSize, int offsetX, int offsetY) {

    const int threshold = 8;

    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            grid->cells[i][j].hTop = false;
            grid->cells[i][j].hRight = false;
            grid->cells[i][j].hBottom = false;
            grid->cells[i][j].hLeft = false;
        }
    }

    int cellX = (mouseX - offsetX) / cellSize;
    int cellY = (mouseY - offsetY) / cellSize;

    if (cellX >= 0 && cellX < grid->width && cellY >= 0 && cellY < grid->height) {
        int cellCenterX = cellX * cellSize + cellSize / 2 + offsetX;
        int cellCenterY = cellY * cellSize + cellSize / 2 + offsetY;

        Cell *cell = &grid->cells[cellX][cellY];

        bool shouldMark = true;

        if (!cell->active) {
            shouldMark = false;
        }

        // check if mouse is near the corners of the cell
        if (mouseX < cellCenterX - cellSize / 2 + threshold && mouseY < cellCenterY - cellSize / 2 + threshold) {
            shouldMark = false;
        }
        if (mouseX > cellCenterX + cellSize / 2 - threshold && mouseY < cellCenterY - cellSize / 2 + threshold) {
            shouldMark = false;
        }
        if (mouseX > cellCenterX + cellSize / 2 - threshold && mouseY > cellCenterY + cellSize / 2 - threshold) {
            shouldMark = false;
        }
        if (mouseX < cellCenterX - cellSize / 2 + threshold && mouseY > cellCenterY + cellSize / 2 - threshold) {
            shouldMark = false;
        }

        if (shouldMark) {
            if (mouseX < cellCenterX - cellSize / 2 + threshold && !cell->left) {
                cell->hLeft = true;
            } else if (mouseX > cellCenterX + cellSize / 2 - threshold && !cell->right) {
                cell->hRight = true;
            } else if (mouseY < cellCenterY - cellSize / 2 + threshold && !cell->top) {
                cell->hTop = true;
            } else if (mouseY > cellCenterY + cellSize / 2 - threshold && !cell->bottom) {
                cell->hBottom = true;
            }
        }

    }
}

bool grid_mark_highlighted_edge(Grid *grid, int player) {
    bool marked = false;
    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            if (grid->cells[i][j].hTop) {
                marked = true;
                grid->cells[i][j].top = true;
                grid->cells[i][j].lastHighlightBy = player;
                if (j > 0) {
                    grid->cells[i][j - 1].bottom = true;
                    grid->cells[i][j - 1].lastHighlightBy = player;
                }
            }
            if (grid->cells[i][j].hRight) {
                marked = true;
                grid->cells[i][j].right = true;
                grid->cells[i][j].lastHighlightBy = player;
                if (i < grid->width - 1) {
                    grid->cells[i + 1][j].left = true;
                    grid->cells[i + 1][j].lastHighlightBy = player;
                }
            }
            if (grid->cells[i][j].hBottom) {
                marked = true;
                grid->cells[i][j].bottom = true;
                grid->cells[i][j].lastHighlightBy = player;
                if (j < grid->height - 1) {
                    grid->cells[i][j + 1].top = true;
                    grid->cells[i][j + 1].lastHighlightBy = player;
                }
            }
            if (grid->cells[i][j].hLeft) {
                marked = true;
                grid->cells[i][j].left = true;
                grid->cells[i][j].lastHighlightBy = player;
                if (i > 0) {
                    grid->cells[i - 1][j].right = true;
                    grid->cells[i - 1][j].lastHighlightBy = player;
                }
            }
        }
    }

    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            grid->cells[i][j].hTop = false;
            grid->cells[i][j].hRight = false;
            grid->cells[i][j].hBottom = false;
            grid->cells[i][j].hLeft = false;
        }
    }

    return marked;
}

bool grid_update_cells(Grid *grid) {
    bool updated = false;
    for (int i = 0; i < grid->width; i++) {
        for (int j = 0; j < grid->height; j++) {
            if (grid->cells[i][j].active && grid->cells[i][j].top && grid->cells[i][j].right && grid->cells[i][j].bottom && grid->cells[i][j].left && grid->cells[i][j].owner == -1) {
                grid->cells[i][j].owner = grid->cells[i][j].lastHighlightBy;
                updated = true;
            }
        }
    }

    return updated;
}




