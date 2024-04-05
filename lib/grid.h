//
// Created by mderer on 05.04.24.
//

#ifndef CHEESEBOX_GRID_H
#define CHEESEBOX_GRID_H

typedef struct Grid Grid;

Grid *grid_create(int width, int height);

void grid_render(Grid *grid, int cellSize, int offsetX, int offsetY, Color playerColors[], int numPlayers, int activePlayer);

void grid_highlight_edge(Grid *grid, int mouseX, int mouseY, int cellSize, int offsetX, int offsetY);

bool grid_update_cells(Grid *grid);

bool grid_mark_highlighted_edge(Grid *grid, int player);

void grid_destroy(Grid *grid);

#endif //CHEESEBOX_GRID_H
