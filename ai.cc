/*
 * 用于计算机博弈的极大极小搜索（加入了 α-β 剪枝）以及估值函数
 * 作者：易水寒
 * E-mail: 604726221@qq.com
 */
#include "gobang.h"
#include "chess_library.h"
#include "openbook.h"
#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <climits>
#include <cstring>
#include <algorithm>

#define MAX_LAY(n) ((n) % 2 == 0)
#define MIN_LAY(n) ((n) % 2 == 1)

int evaluation(const int _y, 
               const int _x, 
               const int (*chessboard) [CK_SIZE], 
               const int color);

extern int step_count;
extern bool ai_is_sente;

/*
 * 博弈树的节点
 */
typedef struct gametree_node {
    int x, y;                           // 记录该节点当前落子的 x、y 坐标
    int _x, _y;                         // 记录下一层节点最终反馈给本层的落子点的 x、y 坐标
    int _score;                         // 记录下一层节点最终反馈给本层的估值分数
    int layer_number;                   // 记录当前节点的层数
    struct gametree_node *next;         // 指向下一层的节点
    struct gametree_node *prev;         // 指向上一层的节点
    int chessboard[CK_SIZE][CK_SIZE];   // 表示当前节点的棋盘
} gametree_node;

int our_color; //= ai_is_sente ? BLACK : WHITE;
int enemy_color; // = ai_is_sente ? WHITE : BLACK;

/*
 * 极大极小算法
 */
void minimax_search(gametree_node *&root, 
                    int ln,     // 表示层数，第一次调用时传入 0
                    int &y, 
                    int &x) {

    if (ln == LAYER_NUM + 1) return;
    our_color = ai_is_sente ? BLACK : WHITE;
    enemy_color = ai_is_sente ? WHITE : BLACK;
    /*
     * 初始化节点
     */
    root = (gametree_node *)malloc(sizeof(gametree_node));
    root->_x = root->_y = INT_MIN;
    root->x = ln ? INT_MIN : x;
    root->y = ln ? INT_MIN : y;
    root->_score = INT_MIN;
    root->layer_number = ln;
    root->prev = ln ? container_of(&root, gametree_node, next) : nullptr;
    root->next = nullptr;

    /*
     * 将上一级的 chessboard 复制到当前节点的 chessboard 中，
     * 如果当前节点为第 0 层，则复制 position 的内容
     */
    int (*p)[CK_SIZE] = ln ? root->prev->chessboard : position;
    for (int i = 0; i < CK_SIZE; i++)
        copy(p[i], p[i] + CK_SIZE, root->chessboard[i]);

    auto search = [ln, root](int &cscore, int &uscore) {
        if (MAX_LAY(ln - 1) &&
            (uscore == INT_MIN || cscore > uscore)) {
            uscore = cscore;
            root->prev->_y = root->y;
            root->prev->_x = root->x;
        } else if (MIN_LAY(ln - 1) &&
            (uscore == INT_MIN || cscore < uscore)) {
            uscore = cscore;
            root->prev->_y = root->y;
            root->prev->_x = root->x;
        }
    };
#ifdef DEBUG
    int xx = 0;
#endif

    if (ln == 0) {
        minimax_search(root->next, ln + 1, y, x);
        goto end;
    }
    /*
     * 遍历每一个空位，产生所有可能的走法
     */
    for (int i = 0; i < CK_SIZE; i++) {
        for (int j = 0; j < CK_SIZE; j++) {
            if (root->chessboard[i][j] == EMPTY && ln != 0) {
                root->y = i;
                root->x = j;
                root->chessboard[i][j] = ln % 2 ? our_color : enemy_color;

                minimax_search(root->next, ln + 1, y, x);

                int eval_score;
                /*
                 * 如果搜索到最后一层，则对每该局面估值，
                 * 否则选取下一层节点反馈给该层的分数
                 */
                if (ln == LAYER_NUM) {
                    if (root->chessboard[i][j] == our_color) {
                        eval_score = evaluation(i, j, root->chessboard, our_color) -
                                     evaluation(root->prev->y, root->prev->x, 
                                                root->chessboard, enemy_color);
                    } else if (root->chessboard[i][j] == enemy_color) {
                        eval_score = evaluation(root->prev->y, root->prev->x, 
                                                root->chessboard, our_color) -
                                     evaluation(i, j, root->chessboard, enemy_color);
                    }
#ifdef DEBUG
                    cout << eval_score << "\t";
                    cout << i << "\t" << j << "\t" << xx << endl;
                    xx++;
#endif
                    search(eval_score, root->prev->_score);
                } else {
                    search(root->_score, root->prev->_score);
                }

                /*
                 * alpha-beta 剪枝
                 */
                // if (ln >= 2 && root->prev->prev->_score != INT_MIN) {
                //     if (MAX_LAY(ln - 1) && 
                //         root->prev->_score >= root->prev->prev->_score) {
                //         goto end;
                //     } else if (MIN_LAY(ln - 1) &&
                //         root->prev->_score <= root->prev->prev->_score) {
                //         goto end;
                //     }
                // }
                root->chessboard[i][j] = EMPTY;
            }
        }
    }
end:
    if (ln == 0) {
        y = root->_y;
        x = root->_x;
    }
    free(root);
    root = nullptr;
}   

/*
 * 针对某一方估值
 */
int evaluation(const int _y, 
               const int _x, 
               const int (*chessboard)[CK_SIZE], 
               const int color) {
    
    char sub_segment[MAX_SIZE + 1];
    char segment[CK_SIZE + 1];
    memset(sub_segment, 0, sizeof(sub_segment));
    memset(segment, 0, sizeof(segment));

    int score = 0;

    /*
     * 用于判断组合棋型的一组计数
     */
    // int _count[5];   // [冲四, 活三, 眠三, 活二, 眠二]
    // memset(_count, 0, sizeof(_count));

    /*
     * 返回棋盘落子点状态
     */
    auto copy_seg = [&color] (int status) {
        if (status == EMPTY) return NO_PIECE;
        else if (status == color) return OUR;
        else return ENEMY;
    };

    /*
     * 匹配 chess_type 中的棋型
     */
    auto match = [&sub_segment, &segment, /*&_count,*/ &score, color] 
                 (const int size, const int sub_size) {
        for (int i = 0; sub_size <= size && 
            i <= size - sub_size; i++) {
            memset(sub_segment, 0, sizeof(sub_segment));
            for (int j = 0, k = i; j < sub_size; j++, k++) {
                sub_segment[j] = segment[k];
            }

            int _score = chess_type[sub_segment];
            if (color == enemy_color && _score == 200)
                _score += 10000;
            if (color == enemy_color && _score == 500)
                _score += 10000;
            score += _score;
            
            // switch(_score) {
            //     case 500: _count[0]++; break;
            //     case 200: _count[1]++; break;
            //     case  50: _count[2]++; break;
            //     case   5: _count[3]++; break;
            //     case   3: _count[4]++; break;
            // }
        }
    };

    /******************** 横向分析 ********************/

    /*
     * 将对应落子点的“横向”拷贝到 segment 中
     */
    for (int i = 0; i < CK_SIZE; i++) {
        segment[i] = copy_seg(chessboard[_y][i]);
    }

    match(CK_SIZE, MIN_SIZE);
    match(CK_SIZE, MIN_SIZE + 1);
    match(CK_SIZE, MIN_SIZE + 2);
    match(CK_SIZE, MIN_SIZE + 3);

    memset(segment, 0, sizeof(segment));

    /******************** 纵向分析 ********************/

    /*
     * 将对应落子点的“纵向”拷贝到 segment 中
     */
    for (int i = 0; i < CK_SIZE; i++) {
        segment[i] = copy_seg(chessboard[i][_x]);
    }

    match(CK_SIZE, MIN_SIZE);
    match(CK_SIZE, MIN_SIZE + 1);
    match(CK_SIZE, MIN_SIZE + 2);
    match(CK_SIZE, MIN_SIZE + 3);

    memset(segment, 0, sizeof(segment));

    /******************** '\'向分析 ********************/
    int dif = _x < _y ? _x : _y;
    for (int i = _y - dif, j = _x - dif, k = 0;
        i < CK_SIZE && j < CK_SIZE; i++, j++, k++) {
        segment[k] = copy_seg(chessboard[i][j]);
    }

    match(CK_SIZE - (_x < _y ? _y - _x : _x - _y), MIN_SIZE);
    match(CK_SIZE - (_x < _y ? _y - _x : _x - _y), MIN_SIZE + 1);
    match(CK_SIZE - (_x < _y ? _y - _x : _x - _y), MIN_SIZE + 2);
    match(CK_SIZE - (_x < _y ? _y - _x : _x - _y), MIN_SIZE + 3);

    memset(segment, 0, sizeof(segment));

    /******************** '/'向分析 ********************/

    /*
     * 获取初始 x、y 的坐标
     */
    int ix, iy;
    for (iy = _y, ix = _x; iy < CK_SIZE - 1 && ix > 0; 
        iy++, ix--);

    int len = 0;
    for (int i = iy, j = ix, k = 0; i >= 0 && j < CK_SIZE; 
        i--, j++, k++, len++) {
        segment[k] = copy_seg(chessboard[i][j]);
    }

    match(len, MIN_SIZE);
    match(len, MIN_SIZE + 1);
    match(len, MIN_SIZE + 2);
    match(len, MIN_SIZE + 3);

    memset(segment, 0, sizeof(segment));

    /*
     * 判断组合棋型
     */
    // if (_count[0] > 1) score += 10000;                      // 双冲四
    // if (_count[0] > 0 && _count[1] > 0) score += 10000;     // 冲四活三
    // if (_count[1] > 1) score += 5000;                       // 双活三
    // if (_count[1] > 0 && _count[2] > 0) score += 1000;      // 活三眠三
    // if (_count[3] > 1) score += 100;                        // 双活二
    // if (_count[3] > 0 && _count[4] > 0) score += 10;        // 活二眠二

    // memset(_count, 0, sizeof(_count));

    return score ? score : position_score[_y][_x];
}


void ai(int &x, int &y) {
    /*
     * 给出一种开局
     */
    if (ai_is_sente && step_count <= 2) {
        if (step_count == 0) {
            y = x = CK_SIZE / 2;
        } else if (step_count == 1) {
            y = open_book[2][0];
            x = open_book[2][1];
        } else if (step_count == 2) {
            y = open_book[2][2];
            x = open_book[2][3];
        }
        return;
    } 
    
    if (!ai_is_sente && step_count == 3 && position[6][8] == EMPTY) {
        y = 6;
        x = 8;
        return;
    }
    /*
     * 五手 N 打规则
     */
    if (step_count == 4) {
        PIMAGE img = newimage();
        getimage(img, "res/black.ico");

        int mark;
        if (position[5][8] == EMPTY && position[6][6] == EMPTY) {
            putimage(8 * UNIT_SIZE, 5 * UNIT_SIZE, img);
            putimage(6 * UNIT_SIZE, 6 * UNIT_SIZE, img);
            mark = 1;
        } else if (position[5][8] != EMPTY && position[6][6] == EMPTY) {
            putimage(9 * UNIT_SIZE, 4 * UNIT_SIZE, img);
            putimage(9 * UNIT_SIZE, 7 * UNIT_SIZE, img);
            mark = 2;
        } else if (position[5][8] == EMPTY && position[6][6] != EMPTY) {
            putimage(7 * UNIT_SIZE, 4 * UNIT_SIZE, img);
            putimage(8 * UNIT_SIZE, 6 * UNIT_SIZE, img);
            mark = 3;
        }

        getimage(img, "res/+.png");

        for (mouse_msg msg = {0}; is_run(); delay_fps(60)) {
            while (mousemsg()) msg = getmouse();

            if (msg.is_up()) {
                switch (mark) {
                    case 1:
                        if (msg.x / UNIT_SIZE == 8 &&
                            msg.y / UNIT_SIZE == 5) {
                            putimage(6 * UNIT_SIZE, 6 * UNIT_SIZE, img);
                            x = 8;
                            y = 5;
                        } else if (msg.x / UNIT_SIZE == 6 &&
                            msg.y / UNIT_SIZE == 6) {
                            putimage(8 * UNIT_SIZE, 5 * UNIT_SIZE, img);
                            x = y = 6;
                        }
                        return;

                    case 2:
                        if (msg.x / UNIT_SIZE == 9 &&
                            msg.y / UNIT_SIZE == 4) {
                            putimage(9 * UNIT_SIZE, 4 * UNIT_SIZE, img);
                            x = 9;
                            y = 7;
                        } else if (msg.x / UNIT_SIZE == 9 &&
                            msg.y / UNIT_SIZE == 7) {
                            putimage(9 * UNIT_SIZE, 7 * UNIT_SIZE, img);
                            x = 9;
                            y = 4;
                        }
                        return;

                    case 3:
                        if (msg.x / UNIT_SIZE == 7 &&
                            msg.y / UNIT_SIZE == 4) {
                            putimage(7 * UNIT_SIZE, 4 * UNIT_SIZE, img);
                            x = 8;
                            y = 6;
                        } else if (msg.x / UNIT_SIZE == 8 &&
                            msg.y / UNIT_SIZE == 6) {
                            putimage(8 * UNIT_SIZE, 6 * UNIT_SIZE, img);
                            x = 7;
                            y = 4;
                        }
                        return;
                }
            }
        }
        delimage(img);
    }
    gametree_node *root = nullptr;
    minimax_search(root, 0, y, x);
}

