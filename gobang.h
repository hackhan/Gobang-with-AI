#ifndef GOBANG_H
#define GOBANG_H

#define CK_SIZE 15
#define CELL_SIZE 30
#define SHORTBAR_SIZE 1
#define UNIT_SIZE (CELL_SIZE + SHORTBAR_SIZE)
#define WINDOW_SIZE ((CELL_SIZE + SHORTBAR_SIZE) * (CK_SIZE - 1) + CELL_SIZE)

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define LAYER_NUM 3

#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

void draw_board();
bool set_chess(int x, int y);
bool select_order();
bool is_gameover(int x, int y);
void ai(int &x, int &y);

#endif
