#include "gobang.h"
#include "chess_library.h"
#include <cstdlib>
#include <climits>

#include <algorithm>

#define LEN_TYPE 5
int evaluation(int _y, int _x);
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
void minimax_search(gametree_node *&root, int layer_number,
                    int &y, int &x) {
    if (layer_number == LAYER_NUM + 1)
        return;

    root = (gametree_node *)malloc(sizeof(gametree_node));
    root->x = root->y = -1;
    root->_x = root->_y = -1;
    root->_score = -1;
    root->layer_number = layer_number;
    root->prev = layer_number ? container_of(&root, gametree_node, next) : nullptr;
    root->next = nullptr;

    // 将上一级的 chessboard 复制到当前节点的 chessboard 中
    int (*ptmp)[CK_SIZE] = layer_number ? root->prev->chessboard : position;
    for (int i = 0; i < CK_SIZE; i++) {
        copy(ptmp[i], ptmp[i] + CK_SIZE, root->chessboard[i]);
    }

    // 记录每一层最终选出的落子点及分数
    int dummy_score = INT_MIN;
    int dummy_x = -1, dummy_y = -1;

    for (int i = 0; i < CK_SIZE; i++) {
        for (int j = 0; j < CK_SIZE; j++) {
            if (root->chessboard[i][j] == 0) {
                // 将有空位的点临时放置一个子
                root->x = j;
                root->y = i;
                root->chessboard[i][j] = layer_number % 2 ? BLACK : WHITE;

                minimax_search(root->next, layer_number + 1, y, x);
                int tmp_score = evaluation(i, j);

                if (LAYER_NUM == layer_number) {
                    if (tmp_score > dummy_score || INT_MIN == dummy_score) {
                        dummy_score = tmp_score;
                        dummy_x = root->x;
                        dummy_y = root->y;
                    }
                } else if (layer_number == 0) {
                    if (root->_score > dummy_score || INT_MIN == dummy_score) {
                        dummy_score = root->_score;
                        x = root->x;
                        y = root->y;
                    }
                } else {
                    if (layer_number % 2 == 0) {
                        if (root->_score > dummy_score || INT_MIN == dummy_score) {
                            dummy_score = root->_score;
                            dummy_x = root->x;
                            dummy_y = root->y;
                        }
                    } else {
                        if (root->_score < dummy_score || INT_MIN == dummy_score) {
                            dummy_score = root->_score;
                            dummy_x = root->x;
                            dummy_y = root->y;
                        }
                    }
                }
                
                root->chessboard[i][j] = 0;
            }
        }
    }

    if (layer_number != 0) {
        root->prev->_x = dummy_x;
        root->prev->_y = dummy_y;
        root->prev->_score = dummy_score;
    } else {
        x = dummy_x;
        y = dummy_y;
    }
    

    free(root);
    root = nullptr;
}

/*
 * 评估函数，只针对某一方估值
 */
int evaluation(int _y, int _x, int (*chessboard)[CK_SIZE], int color) {
    char buf[] = "*******";
    int max_score = 0, dummy_score = 0;
    
    /*
     * 先横向分析
     */
    for (int i = 0; i < CK_SIZE - CHESSTYPE_SIZE + 1; i++) {
        // 将待匹配的区段复制到 buf 中
        for (int j = 0, k = i; j < CHESSTYPE_SIZE; j++, k++) {
            if (chessboard[_y][k] == 0) 
                buf[j] = '0';
            else if (chessboard[_y][k] == color)
                buf[j] = '1';
            else
                buf[j] = '2';
        }
        
        /*
         * 默认棋型长度为 7，如果未匹配到，则逐步减小棋型长度，直到最小长度 4
         */
        for (int i = 0; i <= 3; i++) {
            if ((dummy_score = chess_type[buf]) != 0)
                break;
            buf[CHESSTYPE_SIZE - 1 - i] = '*';
        }

        if (dummy_score > max_score)
            max_score = dummy_score;
    }
    
    /*
     * 纵向分析
     */
    for (int i = 0; i < CK_SIZE - CHESSTYPE_SIZE + 1; i++) {
        for (int j = 0, k = i; j < CHESSTYPE_SIZE; j++, k++) {
            if (chessboard[k][_x] == 0) 
                buf[j] = '0';
            else if (chessboard[k][_x] == color) 
                buf[j] = '1';
            else
                buf[j] = '2';
        }

        for (int i = 0; i <= 3; i++) {
            if ((dummy_score = chess_type[buf]) != 0)
                break;
            buf[CHESSTYPE_SIZE - 1 - i] = '*';
        }

        if (dummy_score > max_score)
            max_score = dummy_score;
    }

    /*
     * 左斜方向分析
     */
    int diff =  _x < _y ? _x : _y;
    for (int i = _x - diff, j = _y - diff; 
        i < CK_SIZE && j < CK_SIZE; i++, j++) {
        
    }

    return max_score;
}


void ai(int &x, int &y) {
    gametree_node *root = nullptr;
    minimax_search(root, 0, y, x);
}

