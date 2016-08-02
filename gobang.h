#ifndef GOBANG_H
#define GOBANG_H

#include <graphics.h>

#define CK_SIZE 15
#define CELL_SIZE 30
#define SHORTBAR_SIZE 1
#define UNIT_SIZE (CELL_SIZE + SHORTBAR_SIZE)
#define WINDOW_SIZE ((CELL_SIZE + SHORTBAR_SIZE) * (CK_SIZE - 1) + CELL_SIZE)

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define LAYER_NUM 1

#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

void draw_board();
bool set_chess(int x, int y);
bool select_order();
bool is_gameover(int x, int y);
void ai(int &x, int &y);
void message_box(const char *s1, const char *s2, int font_size);
int evaluation(const int _y, 
               const int _x, 
               const int (*chessboard) [CK_SIZE], 
               const int color);

bool judge_forbidden(const int _y, 
                     const int _x,
                     const int (*chessboard)[CK_SIZE]);

#endif
