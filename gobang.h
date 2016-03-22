#ifndef GOBANG_H
#define GOBANG_H

#define CK_SIZE 15
#define CELL_SIZE 30
#define SHORTBAR_SIZE 1
#define UNIT_SIZE (CELL_SIZE + SHORTBAR_SIZE)
#define WINDOW_SIZE ((CELL_SIZE + SHORTBAR_SIZE) * \
					 (CK_SIZE - 1) + CELL_SIZE)


void draw_board();

#endif