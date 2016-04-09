#ifndef GOBANG_H
#define GOBANG_H

#include <vector>
using namespace std;

#define CK_SIZE 15
#define CELL_SIZE 30
#define SHORTBAR_SIZE 1
#define UNIT_SIZE (CELL_SIZE + SHORTBAR_SIZE)
#define WINDOW_SIZE ((CELL_SIZE + SHORTBAR_SIZE) * \
					 (CK_SIZE - 1) + CELL_SIZE)

#define EMPTY 0
#define BLACK 1
#define WHITE 2
 
void draw_board();
bool set_chess(int x, int y);
bool select_order();
bool is_gameover(int x, int y);
void ai(int &x, int &y);

template <typename pTemplate>
void scan_line_and_row(pTemplate pFirst,
					   int &count,
					   int &max,
					   int &enemy,
					   vector<int *> &candidateVec);

#endif