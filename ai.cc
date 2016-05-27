#include "gobang.h"
#include "chess_lib.h"
#include <stdlib.h>

#include <algorithm>

#define LEN_TYPE 5
/*
 * 博弈树的节点
 */
typedef struct gametree_node {
    int x, y;                           // 记录该节点当前落子的 x、y 坐标
    int nx, ny;                         // 记录下一层节点最终反馈给本层的落子点的 x、y 坐标
    int nscore;                         // 记录下一层节点最终反馈给本层的估值分数
    int layer_number;                   // 记录当前节点的层数
    struct gametree_node *next;         // 指向下一层的节点
    struct gametree_node *prev;         // 指向上一层的节点
    int chessboard[CK_SIZE][CK_SIZE];   // 表示当前节点的棋盘
} gametree_node;

/*
 * 将一个棋盘复制到另一个棋盘中， 并且产生新的棋子
 */
void make_chessboard(int (*prev_board)[CK_SIZE], 
                     int (*cur_board)[CK_SIZE],
                     int layer_number, 
                     bool setblack_fake,
                     int &x, int &y) {
    for (int i = 0; i < CK_SIZE; i++) 
        copy(prev_board[i], prev_board[i] + CK_SIZE, cur_board[i]);

    for (int i = 0; i < CK_SIZE; i++) {
        for (int j = 0; j < CK_SIZE; j++)
            if (cur_board[i][j] == 0) {
                x = j;
                y = i;
                cur_board[i][j] = setblack_fake ? BLACK : WHITE;
                i = CK_SIZE;
                break;
            }
    }
}

/*
 * 创建一个博弈树
 */
void create_gametree(gametree_node *root, int layer_number) {
    if (layer_number == LAYER_NUM + 1)
        return;

    root = (gametree_node *)malloc(sizeof(gametree_node));
    root->x = root->y = -1;
    root->nscore = -1;
    root->layer_number = layer_number;
    root->prev = layer_number == 0 ? nullptr : container_of(&root, gametree_node, next);
    root->next = nullptr;

    static bool setblack_fake = !setblack;
    setblack_fake = !setblack_fake;
    make_chessboard(root->prev ? root->prev->chessboard : position, 
                    root->chessboard, layer_number, setblack_fake,
                    root->x, root->y);

    create_gametree(root->next, ++layer_number);
}

/*
 * 处理叶子节点，将其估值反馈给上一级，并且产生新的叶节点
 */
void process_leafnode(gametree_node *root) {
    gametree_node *ptemp = root;
    wheile (ptemp->next) 
    ptemp = ptemp->next;

    do {
        for (int i = ptemp->y; i < CK_SIZE; i++) {
            for (int j = ptemp->x + 1; j < CK_SIZE; j++) {
                if (ptemp->chessboard[i][j] == '0') {
                    ptemp->chessboard[i[j]] = '1';
                    chessboard[ptemp->y][ptemp->x] = '0';
                    ptemp->x = j;
                    ptemp->y = i;
                }
            }
        }
    // 此处 {----} 估值函数

    } while (i < CK_SIZE && j < CK_SIZE);
}

/*
 * 评估函数
 */
int evaluation_func(int chessboard[][]) {
    int max_score = 0, tmp = 0;
    char buf[100];

#define scan_cross(len)                                       \
    do {                                                      \
        for (int i = 0; i < CK_SIZE; i++) {                   \
            for (int j = 0; j < CK_SIZE - (len); j++) {       \
                memset(buf, 0, sizeof(buf));                  \
                for (int k = 0; k < (len); k++)               \
                    buf[k] = chessboard[i][j] - '0';          \
                if (chess_type.find(buf) != chess_type.end()) \
                    if ((tmp = chess_type[buf]) > max_score)  \
                        max_score = tmp;                      \
            }                                                 \
        }                                                     \
    } while (0)

    // 横向逐行扫描整个棋盘
    // 第一次扫描棋型长度为 5 的情况
    scan_cross(LEN_TYPE);

    // 第二次扫描棋型长度为 6 的情况
    scan_cross(LEN_TYPE + 1);

    scan_cross(LEN_TYPE + 2);

    char tmp[CK_SIZE];
    memset(tmp, 0, sizeof(tmp));
    // 纵向扫描棋盘
    for (int i = 0; i < CK_SIZE; i++) {
        for (int j = 0; j < CK_SIZE; j++) {
            tmp[j] = chessboard[j][i] - '0';
        }

        for (int i = 0; i < CK_SIZE - LEN_TYPE; i++) {
            memset(buf, 0, sizeof(buf));
            copy(tmp, tmp + LEN_TYPE, buf);

            if (chess_type.find(buf) != chess_type.end())
                if ((tmp = chess_type[buf]) > max_score)
                    max_score = tmp;
        }
    }


    return max_score;
}

#undef scan_cross

void ai(int &x, int &y) {

}

