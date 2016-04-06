#include "gobang.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <vector>
using namespace std;

typedef int (*pRow) [CK_SIZE];

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

void ai(int &x, int &y) {
	int max = 0, count = 0;
	int enemy = setblack ? 2 : 1;	// 标记对方是哪种颜色的棋子
	vector<int *> candidateVec;		// position 数组中候选元素的地址容器

	int *pFirst = NULL, *pLast = NULL;

	/*
	 * 水平方向扫描，寻找可以堵截敌方威胁的最优点，并将其存入 candidateVec 中
	 */
	for (pFirst = &position[y][0]; pFirst != &position[y][0] + CK_SIZE; ) {
		if (*pFirst == enemy) {
			for (pLast = pFirst + 1; pLast != &position[y][0] + CK_SIZE; )
				if (*pLast == enemy) pLast++;
				else break;

			if ((count = pLast - pFirst) >= max && count < 5) {
				if (count > max)
					candidateVec.clear();
				if (pFirst > &position[y][0] && *(pFirst - 1) == 0) 
					candidateVec.push_back(pFirst - 1);
				if (pLast <= &position[y][CK_SIZE - 1] && *pLast == 0)
					candidateVec.push_back(pLast);
				max = count;
			}

			pFirst = pLast;
			continue;
		}
		pFirst++;
	}

#ifdef DEBUG
	printf("position[%d]: \n", y);
	for (int i = 0; i < CK_SIZE; i++) {
		printf("%d ", position[y][i]);
	}
	printf("\n");

	cout << "candidateVec: ";
	for (auto &p : candidateVec)
		cout << p << endl;
#endif

	/*
	 * 竖直方向扫描
	 */
	pRow pRowFirst, pRowLast;
	for (pRowFirst = (pRow)&position[0][x]; 
		 pRowFirst != (pRow)&position[0][x] + CK_SIZE; ) {
		if (*pRowFirst == 0) {
			
		}
	}

	/*
	 * 四个方向上都遍历过后，从 candidateVec 中选择最优点
	 */
	for (auto &p : candidateVec) {
		int _x, _y;
		int max = -1;
		int offset = (int *)p - &position[0][0];
		_y = offset / CK_SIZE;
		_x = offset % CK_SIZE;

		if (position_score[_y][_x] > max) {
			max = position_score[_y][_x];
			x = _x;
			y = _y;
		}
	}

#ifdef DEBUG
	printf("x: %d\n", x);
	printf("y: %d\n", y);
#endif
	
	/*
	 * 如果没有找到合适的候选点，则根据 position_score 数组寻找
	 * 分值最大的空点
	 */
	if (candidateVec.empty()) {
		int max = -1, _x, _y;
		for (int i = 0; i < CK_SIZE; i++) {
			for (int j = 0; j < CK_SIZE; j++) {
				if (position[i][j] == 0 && position_score[i][j] > max) {
					max = position_score[i][j];
					_x = j;
					_y = i;
				}
			}
		}
		x = _x;
		y = _y;
	}
	
}

