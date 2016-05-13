#ifndef CHESS_TABLE_H
#define CHESS_TABLE_H

#include <vector>
#include <iostream>
#include <map>
using namespace std;

/*
 * 定义一个二维数组记录棋盘上每个位置的分值，
 * 根据棋盘分布问题，越中心的点分值越高。
 */
const int position_score[CK_SIZE][CK_SIZE] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0 },
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 },
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0 },
	{ 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0 },
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

extern int position[CK_SIZE][CK_SIZE];
extern bool setblack;

/*vector<const char *> continuous_long = { "11111" };
vector<const char *> active_four     = { "011110" };
vector<const char *> dash_four       = { "011112", "0101110", "0110110" };
vector<const char *> active_three    = { "01110", "010110" };

vector<vector<const char *>> chesstype_table = { 
	continuous_long, 
	active_three, 
	dash_four, 
	active_three 
};*/

map<const char *, size_t> chess_type = {
	{ "11111", 10000 }, { "011110", 1000 },
	{ "011112", 100 }, { "0101110", 100 }, { "0110110", 100 },
	{ "01110", 10 }, { "010110", 10 }
};

#endif
