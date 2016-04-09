#include "gobang.h"
#include <iostream>

typedef int (*pRow) 		 [CK_SIZE];
typedef int (*pLeftOblique)  [CK_SIZE + 1];
typedef int (*pRightOblique) [CK_SIZE - 1];

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

/* 
 * 扫描敌方落子点的横向和纵向
 */
template <typename pTemplate>
void scan_line_and_row(pTemplate pFirst,
					   int &count,
					   int &max,
					   int &enemy,
					   vector<int *> &candidateVec) {
	pTemplate pLast = nullptr;
	pTemplate startAddress = pFirst;

	while (pFirst != startAddress + CK_SIZE) {
		if (*(int *)pFirst == enemy) {
			for (pLast = pFirst + 1; pLast != startAddress + CK_SIZE; ) {
				if (*(int *)pLast == enemy) pLast++;
				else break;
			}

			if ((count = pLast - pFirst) >= max && count < 5) {
		#ifdef DEBUG
				cout << "pLast: " << pLast << endl;
				cout << "pFirst: " << pFirst << endl;
				cout << "pLast - pFirst: " << (pLast - pFirst) << endl;
		#endif
				if (count > max) {
					candidateVec.clear();
					max = count;
				}
				if (pFirst > startAddress && *((int *)(pFirst - 1)) == EMPTY)
					candidateVec.push_back((int *)(pFirst - 1));
				if (pLast <= startAddress + CK_SIZE - 1 &&
				    *(int *)pLast == EMPTY)
				    candidateVec.push_back((int *)pLast);
			}

			pFirst = pLast;
			continue;
		}
		pFirst++;
	}
}

void ai(int &x, int &y) {
	int max = 0, count = 0;
	int enemy = setblack ? WHITE : BLACK;	// 标记对方是哪种颜色的棋子
	vector<int *> candidateVec;		// 将 position 数组中候选的点保存于此

	if (x == -1)  {
		x = y = CK_SIZE / 2;
		return;
	}
	
	// 水平方向扫描，寻找可以堵截敌方威胁的最优点，并将其存入 candidateVec 中
	scan_line_and_row(&position[y][0], count, max, enemy, candidateVec);

	//  竖直方向扫描
	scan_line_and_row((pRow)&position[0][x], count, max, enemy, candidateVec);

	/* 
	 * "\" 方向扫描
	 */
	int _x, _y;
	int x_ = -1, y_ = -1;
	x < y ? (_x = x - x, _y = y - x) : (_x = x - y, _y = y - y);
	while (_x < CK_SIZE && _y < CK_SIZE) {
		if (position[_y][_x] == enemy) {
			for (y_ = _y + 1, x_ = _x + 1; 
				 y_ < CK_SIZE && x_ < CK_SIZE; ) {
				if (position[y_][x_] == enemy) {
					x_++;
					y_++;
				} else break;
			}
		
			if ((count = x_ - _x) >= max && count < 5) {
				if (count > max) {
					candidateVec.clear();
					max = count;
				}
				if (_x > 0 && _y > 0 && position[_y - 1][_x - 1] == EMPTY)
					candidateVec.push_back(&position[_y - 1][_x - 1]);
				if (x_ < CK_SIZE && y_ < CK_SIZE && position[y_][x_] == EMPTY)
					candidateVec.push_back(&position[y_][x_]);
				
			}
			_y = y_;
			_x = x_;
			continue;
		}
		_y++;
		_x++;
	}

	/* 
	 * "/" 方向扫描
	 */
	_x = x;
	_y = y;
	while (_x != 0 && _y != CK_SIZE - 1) {
		_x--;
		_y++;
	}
	while (_x < CK_SIZE && _y >= 0)	{
		if (position[_y][_x] == enemy) {
			for (y_ = _y - 1, x_ = _x + 1; 
				 y_ >= 0 && x_ < CK_SIZE; ) {
				if (position[y_][x_] == enemy) {
					x_++;
					y_--;
				} else break;
			}

			if ((count = x_ - _x) >= max && count < 5) {
				if (count > max) {
					candidateVec.clear();
					max = count;
				}
				if (_x > 0 && _y < CK_SIZE && position[_y + 1][_x - 1] == EMPTY)
					candidateVec.push_back(&position[_y + 1][_x - 1]);
				if (x_ < CK_SIZE && y_ >= 0 && position[y_][x_] == EMPTY)
					candidateVec.push_back(&position[y_][x_]);
			}
			_y = y_;
			_x = x_;
			continue;
		}
		_y--;
		_x++;
	}

#ifdef DEBUG
	cout << "candidateVec: " << endl;
	for (auto &pi : candidateVec) {
		cout << (pi - &position[0][0]) / CK_SIZE << ", ";
		cout << (pi - &position[0][0]) % CK_SIZE << endl;
	}
	cout << "max: " << max << endl;
	cout << "count: " << count << endl;
#endif

	/*
	 * 四个方向上都遍历过后，从 candidateVec 中选择最优点
	 */
	int maxScore = -1;
	for (auto &p : candidateVec) {
		int _x, _y;
		int offset = p - &position[0][0];
		_y = offset / CK_SIZE;
		_x = offset % CK_SIZE;

		if (position_score[_y][_x] > maxScore) {
			maxScore = position_score[_y][_x];
			x = _x;
			y = _y;
		}
	}

	/*
	 * 如果没有找到合适的候选点，则根据 position_score 数组寻找
	 * 分值最大的空点
	 */
	if (candidateVec.empty()) {
		int max = -1;
		for (int i = 0; i < CK_SIZE; i++) {
			for (int j = 0; j < CK_SIZE; j++) {
				if (position[i][j] == 0 && 
					position_score[i][j] > max) {
					max = position_score[i][j];
					x = j;
					y = i;
				}
			}
		}
#ifdef DEBUG
		cout << "x: " << x << endl;
		cout << "y: " << y << endl;
#endif
	}
	
}

