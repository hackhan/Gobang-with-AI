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

int evaluation(const int _y, 
               const int _x, 
               const int (*chessboard) [CK_SIZE], 
               const int color);
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
                    search(eval_score, root->prev->_score);
                } else {
                    search(root->_score, root->prev->_score);
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
int evaluation(const int _y, 
               const int _x, 
               const int (*chessboard) [CK_SIZE], 
               const int color) {
    
    char sub_segment[MAX_SIZE + 1];
    char segment[CK_SIZE + 1];
    memset(sub_segment, 0, sizeof(sub_segment));
    memset(segment, 0, sizeof(segment));

    int score;

    /*
     * 用于判断组合棋型的一组计数
     */
    int _count[5];   // [冲四, 活三, 眠三, 活二, 眠二]
    memset(_count, 0, sizeof(_count));

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
    auto match = [&sub_segment, &segment, &chess_type, &_count, &score] 
                 (const int size, const int sub_size) {
        for (int i = 0; sub_size <= size && 
            i <= size - sub_size; i++) {
            memset(sub_segment, 0, sizeof(sub_segment));
            for (int j = 0, k = i; j < sub_size; j++, k++) {
                sub_segment[j] = segment[k];
            }

            int _score = chess_type[sub_segment];
            score += _score;

            switch(_score) {
                case 500: _count[0]++; break;
                case 200: _count[1]++; break;
                case  50: _count[2]++; break;
                case   5: _count[3]++; break;
                case   3: _count[4]++; break;
            }
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
    if (_count[0] > 1) score += 10000;                      // 双冲四
    if (_count[0] > 0 && _count[1] > 0) score += 10000;     // 冲四活三
    if (_count[1] > 1) score += 5000;                       // 双活三
    if (_count[1] > 0 && _count[2] > 0) score += 1000;      // 活三眠三
    if (_count[3] > 1) score += 100;                        // 双活二
    if (_count[3] > 0 && _count[4] > 0) score += 10;        // 活二眠二

    memset(_count, 0, sizeof(_count));
    return score;
}

void ai(int &x, int &y) {
    gametree_node *root = nullptr;
    minimax_search(root, 0, y, x);
}

