/*
 * 该程序的主要界面逻辑
 * 作者：易水寒
 * E-mail: 604726221@qq.com
 */
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "gobang.h"

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int position[CK_SIZE][CK_SIZE];        // 用一个二维数组记录棋盘上每个落子点的状态(0无子，1黑子，2白子)
bool setblack = true;                  // 值为 true 时该黑子下，否则该白子下
int step_count = 0;                    // 记录步数
bool ai_is_sente = true;               // 标记电脑是否为先手

int main() {
    initgraph(WINDOW_SIZE + UNIT_SIZE + 300, WINDOW_SIZE + UNIT_SIZE);
    setcaption("五子棋");
    setbkcolor(EGERGB(218, 165, 105));

    bool flag = select_order();
    draw_board();

    // 清空鼠标消息
    mouse_msg temp;
    while (mousemsg())
        temp = getmouse();

    int x = -1, y = -1;
    do {
        if (flag) {
            ai(x, y);
        } else {
            for (mouse_msg msg = {0}; is_run(); delay_fps(60)) {
                while (mousemsg())
                    msg = getmouse();
                if (msg.is_up()) {
                    x = msg.x / UNIT_SIZE;
                    y = msg.y / UNIT_SIZE;
                    break;
                }
            }
        }
        if (set_chess(x, y)) {
            step_count++;
            if (step_count > 2) 
                flag = !flag;
            /*
             * 三手可换规则
             */
            if (ai_is_sente && step_count == 3) {
                setfont(-19, 0, "宋体");
                outtextxy(18 * UNIT_SIZE + 10, 2 * UNIT_SIZE, "对方是否要求换子？");
                outtextxy(19 * UNIT_SIZE + 10, 3 * UNIT_SIZE, "是");
                outtextxy(22 * UNIT_SIZE, 3 * UNIT_SIZE, "否");

                for (mouse_msg msg = {0}; is_run(); delay_fps(60)) {
                    while (mousemsg()) msg = getmouse();
                    if (msg.is_up()) {
                        if (msg.x >= 19 * UNIT_SIZE + 10 && msg.x <= 23 * UNIT_SIZE &&
                            msg.y >= 3 * UNIT_SIZE && msg.y <= 3 * UNIT_SIZE + 20) {
                            if (msg.x >= 19 * UNIT_SIZE + 10 && msg.x <= 20 * UNIT_SIZE) {
                                ai_is_sente = false;
                                flag = true;
                                setfont(-16, 0, "宋体");
                                outtextxy(19 * UNIT_SIZE + 20, 20, "电脑执白子");
                            }
                            // 清空已无用的文字
                            setfont(-19, 0, "宋体");
                            outtextxy(18 * UNIT_SIZE + 10, 2 * UNIT_SIZE, "                ");
                            outtextxy(19 * UNIT_SIZE + 10, 3 * UNIT_SIZE, "                ");
                            break;
                        }
                    }
                }
            }
        }
    } while (!is_gameover(x, y));

    setfont(-50, 0, "黑体");
    outtextxy(17 * UNIT_SIZE + 15, 7 * UNIT_SIZE, setblack ? " 白子胜！" : " 黑子胜！");
    for (key_msg k = {0}; k = getkey(), k.key != key_esc;);
    closegraph();
    return 0;
}

/*
 * 选择对战次序(电脑先下 / 玩家先下)
 * 如果选择电脑先下则返回 true，否则返回 false
 */
bool select_order() {
    setcolor(EGERGB(0, 0, 0));
    setfont(-19, 0, "宋体");
    outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "● 电脑先下");
    outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "○ 玩家先下");
    setfont(-16, 0, "宋体");
    outtextxy(5 * UNIT_SIZE, 10 * UNIT_SIZE, "↑ ↓ 选择  Enter 确定");

    setfont(-19, 0, "宋体");
    // outtextrect(14 * UNIT_SIZE, UNIT_SIZE, 100, 100, "hello world.hello world.hello world.hello world.");
    // 清空键盘输入
    getkey();
    bool flag = true;
    for (key_msg k = {0}; k = getkey(), k.key != key_esc;) {
        switch (k.key) {
        case 38:
            outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "●");
            outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "○");
            flag = true;
            ai_is_sente = true;
            break;
        case 40:
            outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "○");
            outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "●");
            flag = false;
            ai_is_sente = false;
            break;
        case 13:
            cleardevice();
            return flag;
        }
    }
    exit(0);
}

/*
 * 绘制棋盘
 */
void draw_board() {
    setcolor(EGERGB(0, 0, 0));
    setfillcolor(EGERGB(0, 0, 0));
    char s[50];
    sprintf(s, "%s%s%s", "电脑执", ai_is_sente ? "黑" : "白", "子");
    setfont(-16, 0, "宋体");
    outtextxy(19 * UNIT_SIZE + 20, 20, s);

    // 绘制网格线
    for (int i = 0; i < CK_SIZE; i++) {
        bar(CELL_SIZE / 2 - 1, CELL_SIZE / 2 - 1 + UNIT_SIZE * i,
            CELL_SIZE / 2 + 1 + WINDOW_SIZE - CELL_SIZE,
            CELL_SIZE / 2 + 1 + UNIT_SIZE * i);
        sprintf(s, "%d", CK_SIZE - i);
        outtextxy(CK_SIZE * UNIT_SIZE, i * UNIT_SIZE + 5, s);
        outtextxy((CK_SIZE + 1) * UNIT_SIZE, i * UNIT_SIZE, "┃");
    }

    outtextxy((CK_SIZE + 1) * UNIT_SIZE, CK_SIZE * UNIT_SIZE, "┃");

    for (int i = 0; i < CK_SIZE; i++) {
        bar(CELL_SIZE / 2 - 1 + UNIT_SIZE * i, CELL_SIZE / 2 - 1,
            CELL_SIZE / 2 + 1 + UNIT_SIZE * i,
            CELL_SIZE / 2 + 1 + WINDOW_SIZE - CELL_SIZE);
        sprintf(s, "%c", 'A' + i);
        outtextxy(i * UNIT_SIZE + 10, CK_SIZE * UNIT_SIZE, s);
    }

    // 绘制天元及周围四个星位的黑点
    PIMAGE img = newimage();
    getimage(img, "res/。.ico");
    putimage(CK_SIZE / 2 * UNIT_SIZE, CK_SIZE / 2 * UNIT_SIZE, img);
    putimage(3 * UNIT_SIZE, 3 * UNIT_SIZE, img);
    putimage((CK_SIZE - 4) * UNIT_SIZE, 3 * UNIT_SIZE, img);
    putimage(3 * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);
    putimage((CK_SIZE - 4) * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);

    delimage(img);
}

void message_box(const char *s1, 
                 const char *s2, 
                 int font_size) {
    setfont(-font_size, 0, "宋体");
    outtextrect(17 * UNIT_SIZE + 15, 7 * UNIT_SIZE, 
                7 * UNIT_SIZE, 2 * UNIT_SIZE, s1);
    outtextxy(20 * UNIT_SIZE, 9 * UNIT_SIZE, s2);

    for (mouse_msg msg = {0}; is_run(); delay_fps(60)) {
        while (mousemsg()) msg = getmouse();
        if (msg.is_up()) {
            if (msg.x >= 20 * UNIT_SIZE && 
                msg.x <= 22 * UNIT_SIZE &&
                msg.y >= 9 * UNIT_SIZE &&
                msg.y <= 10 * UNIT_SIZE) {
                outtextrect(17 * UNIT_SIZE + 15, 7 * UNIT_SIZE, 
                            7 * UNIT_SIZE, 2 * UNIT_SIZE, "                                        ");
                outtextxy(17 * UNIT_SIZE + 15, 9 * UNIT_SIZE, "                  ");
                break;
            }
        }
    }
}

bool set_chess(int x, int y) {
    if (position[y][x] != EMPTY || y >= CK_SIZE || x >= CK_SIZE)
        return false;

    if (!ai_is_sente) {
        if (step_count == 0 && (x != CK_SIZE / 2 || 
            y != CK_SIZE / 2)) {
            message_box("第一子未落在天元处！", "确定", 23);
            return false;
        } else if (step_count == 1 && 
            (x < 6 || x > 8 || y < 6 || y > 8)) {
            message_box("第二子未落在与天元直接相连的8个点上！", 
                        "确定", 23);
            return false;
        } else if (step_count == 2 && 
            (x < 5 || x > 9 || y < 5 || y > 9)) {
            message_box("第三子未落在指定范围内！", "确定", 23);
            return false;
        }
    }

    PIMAGE img = newimage();
    getimage(img, setblack ? "res/black.ico" : "res/white.ico");
    putimage(x * UNIT_SIZE, y * UNIT_SIZE, img);
    position[y][x] = setblack ? BLACK : WHITE;
    setblack = !setblack;
    return true;
}

bool is_gameover(int x, int y) {
    vector<int> five(5, setblack ? WHITE : BLACK);
    vector<int> ivec;

    /*
     * 将 position 数组的第 y 行拷贝到 ivec 中，然后搜索该行是否出现
     * 了连续相同的五个元素
     */
    copy(position[y], position[y] + CK_SIZE, back_inserter(ivec));
    auto it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
    if (it != ivec.end())
        return true;
    ivec.clear();

    /*
     * 将 position 数组第 x 列拷贝到 ivec 中，然后搜索该列是否出现
     * 了连续相同的五个元素
     */
    for (int i = 0; i < CK_SIZE; i++) {
        ivec.push_back(position[i][x]);
    }
    it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
    if (it != ivec.end())
        return true;
    ivec.clear();

    /*
     * 将 position[y][x] 所在的 "\" 方向斜线拷贝到 ivec 中，搜索
     * 是否出现连续相同的五个元素
     */
    int _x, _y;
    x < y ? (_x = x - x, _y = y - x) : (_x = x - y, _y = y - y);
    for (; _x < CK_SIZE && _y < CK_SIZE; _x++, _y++) {
        ivec.push_back(position[_y][_x]);
    }
    it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
        if (it != ivec.end()) return true;
    ivec.clear();

    /*
     * 将 position[y][x] 所在的 "/" 方向斜线拷贝到 ivec 中，搜索
     * 是否出现连续相同的五个元素
     */
    _x = x;
    _y = y;
    while (_x != 0 && _y != CK_SIZE - 1) {
        _x--;
        _y++;
    }
    for (; _x < CK_SIZE && _y >= 0; _x++, _y--)	{
        ivec.push_back(position[_y][_x]);
    }

    it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
    if (it != ivec.end()) return true;

    return false;
}

