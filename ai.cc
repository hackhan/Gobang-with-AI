/*
 * 用于计算机博弈的极大极小搜索（加入了 α-β 剪枝）以及估值函数
 * 作者：易水寒
 * E-mail: 604726221@qq.com
 */
#include "gobang.h"
#include "chess_library.h"
#include <cstdlib>
#include <climits>
#include <cstring>
#include <algorithm>

#define MAX_LAY(n) ((n) % 2 == 0)
#define MIN_LAY(n) ((n) % 2 == 1)

int evaluation(const int _y, const int _x, 
               int (*chessboard)[CK_SIZE], int color);
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

/*
 * 极大极小算法
 */
void minimax_search(gametree_node *&root, 
                    int ln,     // 表示层数，第一次调用时传入 0
                    int &y, 
                    int &x) {

    if (ln == LAYER_NUM + 1) return;
    
    int our_color = setblack ? BLACK : WHITE;
    int enemy_color = setblack ? WHITE : BLACK;    

    /*
     * 初始化节点
     */
    root = (gametree_node *)malloc(sizeof(gametree_node));
    root->x = root->y = INT_MIN;
    root->_x = root->_y = INT_MIN;
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

    if (ln == 0) minimax_search(root->next, ln + 1, y, x);

    /*
     * 遍历每一个空位，产生所有可能的走法
     */
    for (int i = 0; i < CK_SIZE; i++) {
        for (int j = 0; j < CK_SIZE; j++) {
            if (root->chessboard[i][j] == EMPTY && ln != 0) {
                root->y = i;
                root->x = j;
                root->chessboard[i][j] = ln % 2 ? our_color : enemy_color;

                int eval_score;

                /*
                 * 如果搜索到最后一层，则对每该局面估值，
                 * 否则选取下一层节点反馈给该层的分数
                 */
                if (ln == LAYER_NUM) {
                    eval_score = evaluation(i, j, root->chessboard, 
                                            ln % 2 ? our_color : enemy_color) - 
                                 evaluation(root->prev->y, root->prev->x, 
                                            root->chessboard, 
                                            ln % 2 ? enemy_color : our_color);
                    
                    if (MAX_LAY(ln - 1) && (root->prev->_score == INT_MIN ||
                        eval_score > root->prev->_score)) {
                        root->prev->_score = eval_score;
                        root->prev->_y = root->y;
                        root->prev->_x = root->x;
                    } else if (MIN_LAY(ln - 1) && (root->prev->_score == INT_MIN ||
                        eval_score < root->prev->_score)) {
                        root->prev->_score = eval_score;
                        root->prev->_y = root->y;
                        root->prev->_x = root->x;
                    }
                } else {
                    if (MAX_LAY(ln - 1) && (root->prev->_score == INT_MIN ||
                        root->_score > root->prev->_score)) {
                        root->prev->_score = root->_score;
                        root->prev->_y = root->y;
                        root->prev->_x = root->x;
                    } else if (MIN_LAY(ln - 1) && (root->prev->_score == INT_MIN ||
                        root->_score < root->prev->_score)) {
                        root->prev->_score = root->_score;
                        root->prev->_y = root->y;
                        root->prev->_x = root->x;
                    }
                }

                /*
                 * alpha-beta 剪枝
                 */
                if (ln >= 2 && root->prev->prev->_score != INT_MIN) {
                    if (MAX_LAY(ln - 1) && 
                        root->prev->_score >= root->prev->prev->_score) {
                        i = CK_SIZE;
                        break;
                    } else if (MIN_LAY(ln - 1) &&
                        root->prev->_score <= root->prev->prev->_score) {
                        i = CK_SIZE;
                        break;
                    }
                }

                root->chessboard[i][j] = EMPTY;

            }
        }
    }
    
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
int evaluation(const int _y, const int _x, 
               int (*chessboard)[CK_SIZE], int color) {
    char buf[8];
    int max_score = 0, dummy_score = INT_MIN;
    
    /*
     * 先横向分析
     */
    for (int i = 0; i < CK_SIZE - MIN_SIZE + 1; i++) {
        memset(buf, 0, sizeof(buf));
        // 将待匹配的区段复制到 buf 中
        for (int j = 0, k = i; j < MIN_SIZE; j++, k++) {
            if (chessboard[_y][k] == EMPTY) buf[j] = NO_PIECE;
            else if (chessboard[_y][k] == color) buf[j] = OUR;
            else buf[j] = ENEMY;
        }

        for (int j = 0; j <= MAX_SIZE - MIN_SIZE; j++) {
            if ((dummy_score = chess_type[buf]) != 0) 
                break;
            if (chessboard[_y][i + MIN_SIZE + j] == EMPTY && 
                i + MIN_SIZE + j < CK_SIZE &&
                j < MAX_SIZE - MIN_SIZE) 
                buf[MIN_SIZE + j] = NO_PIECE;

            else if (chessboard[_y][i + MIN_SIZE + j] == color && 
                    i + MIN_SIZE + j < CK_SIZE && 
                    j < MAX_SIZE - MIN_SIZE) 
                buf[MIN_SIZE + j] = OUR;

            else if (chessboard[_y][i + MIN_SIZE + j] != color &&
                    chessboard[_y][i + MIN_SIZE + j] != EMPTY && 
                    i + MIN_SIZE + j < CK_SIZE &&
                    j < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + j] = ENEMY;
        }

        if (dummy_score > max_score) max_score = dummy_score;
    }

    /*
     * 纵向分析
     */
    for (int i = 0; i < CK_SIZE - MIN_SIZE + 1; i++) {
        memset(buf, 0, sizeof(buf));

        for (int j = 0, k = i; j < MIN_SIZE; j++, k++) {
            if (chessboard[k][_x] == EMPTY) buf[j] = NO_PIECE;
            else if (chessboard[k][_x] == color) buf[j] = OUR;
            else buf[j] = ENEMY;
        }

        for (int j = 0; j <= MAX_SIZE - MIN_SIZE; j++) {
            if ((dummy_score = chess_type[buf]) != 0) 
                break;
            if (chessboard[i + MIN_SIZE + j][_x] == EMPTY &&
                i + MIN_SIZE + j < CK_SIZE &&
                j < MAX_SIZE - MIN_SIZE) 
                buf[MIN_SIZE + j] = NO_PIECE;

            else if (chessboard[i + MIN_SIZE + j][_x] == color &&
                    i + MIN_SIZE + j < CK_SIZE &&
                    j < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + j] = OUR;

            else if (chessboard[i + MIN_SIZE + j][_x] != color &&
                    chessboard[i + MIN_SIZE + j][_x] != EMPTY &&
                    i + MIN_SIZE + j < CK_SIZE &&
                    j < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + j] = ENEMY;
        }
        
        if (dummy_score > max_score) max_score = dummy_score;
    }

    /*
     * 左斜方向分析
     */
    int diff = _x < _y ? _x : _y;
    for (int i = _y - diff, j = _x - diff; 
         i <= CK_SIZE - MIN_SIZE + 1 && j <= CK_SIZE - MIN_SIZE + 1;
         i++, j++) {
        memset(buf, 0, sizeof(buf));

        for (int k = 0, l = i, m = j; k < MIN_SIZE; k++, l++, m++) {
            if (chessboard[i][j] == EMPTY) buf[k] = NO_PIECE;
            else if (chessboard[i][j] == color) buf[j] = OUR;
            else buf[j] = ENEMY;
        }

        for (int k = 0; k < MAX_SIZE - MIN_SIZE; j++) {
            if ((dummy_score = chess_type[buf]) != 0)
                break;
            if (chessboard[i + MIN_SIZE + k][j + MIN_SIZE + k] == EMPTY &&
                i + MIN_SIZE + k < CK_SIZE && 
                j + MIN_SIZE + k < CK_SIZE && 
                k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + 1] = NO_PIECE;

            else if (chessboard[i + MIN_SIZE + k][j + MIN_SIZE + k] == color &&
                    i + MIN_SIZE + k < CK_SIZE &&
                    j + MIN_SIZE + k < CK_SIZE &&
                    k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + k] = OUR;

            else if (chessboard[i + MIN_SIZE + k][j + MIN_SIZE + k] != color &&
                    chessboard[i + MIN_SIZE + k][j + MIN_SIZE + k] != EMPTY &&
                    i + MIN_SIZE + k < CK_SIZE && 
                    j + MIN_SIZE + k < CK_SIZE &&
                    k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + k] = ENEMY;
        }

        if (dummy_score > max_score) max_score = dummy_score;
    }

    /*
     * 右斜方向分析
     */
    int i, j;
    for (i = _y, j = _x; i < CK_SIZE - 1 && j > 0; i++, j--);

    for ( ; i != MIN_SIZE - 2 && j != CK_SIZE - MIN_SIZE; i--, j++) {
        memset(buf, 0, sizeof(buf));
        for (int k = 0, l = i, m = j; k < MIN_SIZE; k++, l--, m++) {
            if (chessboard[l][m] == EMPTY) buf[j] = NO_PIECE;
            else if (chessboard[l][m] == color) buf[j] = OUR;
            else buf[j] = ENEMY;
        }

        for (int k = 0; k < MAX_SIZE - MIN_SIZE; k++) {
            if ((dummy_score = chess_type[buf]) != 0)
                break;
            if (chessboard[i - MIN_SIZE - k][j + MIN_SIZE + k] == EMPTY &&
                i - MIN_SIZE - k >= 0 && 
                j + MIN_SIZE + k < CK_SIZE &&
                k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + k] = NO_PIECE;

            else if (chessboard[i - MIN_SIZE - k][j + MIN_SIZE + k] == color &&
                    i - MIN_SIZE - k >= 0 && 
                    j + MIN_SIZE + k < CK_SIZE &&
                    k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + k] = OUR;

            else if (chessboard[i - MIN_SIZE - k][j + MIN_SIZE + k] != color && 
                    chessboard[i - MIN_SIZE - k][j + MIN_SIZE + k] != EMPTY &&
                    i - MIN_SIZE - k >= 0 && 
                    j + MIN_SIZE + k < CK_SIZE &&
                    k < MAX_SIZE - MIN_SIZE)
                buf[MIN_SIZE + k] = ENEMY;
        }

        if (dummy_score > max_score) max_score = dummy_score;
    }

    return max_score;
}


void ai(int &x, int &y) {
    gametree_node *root = nullptr;
    minimax_search(root, 0, y, x);
}

