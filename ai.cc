#include "gobang.h"
#include "chesslib.h"
#include <string.h>

/* 
 * 将 buf 与 chess_type 中的所有棋型逐一匹配，返回匹配到的
 * 首地址，并且设置该棋型对应的分值
 */
const char *match(const char *buf, int buf_size, int &score) {
	const char *pres = nullptr;
	for (const auto &m : chess_type) {
		if ((pres = strstr(buf, m.first)) != nullptr) {
			score = m.second;
			return pres;
		}
	}
	return nullptr;
}

#define copy_to_dummy(addr, steps) do { \
	int *pstart = addr; \
	for (int i = 0, j = CK_SIZE - 1; \
		 i < CK_SIZE; i++, j--, pstart += steps) { \
		if (*pstart != EMPTY) { \ 
			dummy_forward[i] = *pstart == enemy ? '2' : '1'; \
			dummy_reverse[j] = dummy_forward[i]; \
		} else { \
			dummy_forward[i] = dummy_reverse[j] = '0'; \
		} \
	} \
} while (0)

void ai(int &x, int &y) {
	if (x == -1) {
		x = y = CK_SIZE / 2;
		return;
	}
	
	char dummy_forward[CK_SIZE + 1];			// 正向储存某一方向上的所有落子点状态
	char dummy_reverse[CK_SIZE + 1];			// 反向储存某一方向上的所有落子点状态
	memset(dummy_forward, 0, sizeof(dummy_forward));
	memset(dummy_reverse, 0, sizeof(dummy_reverse));

	int enemy = setblack ? WHITE : BLACK;		// 标记对方是哪种颜色的棋子
	vector<const int *> candidate;				// 候选点坐标

	// 将敌方落子点的横向复制到两个 dummy 中
	for (int i = 0, j = CK_SIZE - 1; i < CK_SIZE; i++, j--) {
		if (position[y][i] != EMPTY) {
			dummy_forward[i] = position[y][i] == enemy ? '2' : '1';
			dummy_reverse[j] = dummy_forward[i];
		} else {
			dummy_forward[i] = dummy_reverse[j] = '0';
		}
	}

	// 横向匹配棋型
	int max_score = 0;
	for (int i = 0, j = CK_SIZE - 1; i < CK_SIZE; i++, j--) {
		const char *pres = nullptr;
		int tmp;
		if (dummy_forward[i] == '0') {
			dummy_forward[i] = '1';
			
			if ((pres = match(dummy_forward, CK_SIZE, &tmp)) != nullptr) {
				if (tmp > max_score) {
					max_score = tmp;
					candidate.clear();
				}
				candidate.push_back(&position[y][pres-dummy_forward]);
				break;
			}
			dummy_forward[i] = '0';
		}
		if (dummy_reverse[i] == '0') {
			dummy_reverse[i] = '1';
			if ((pres = match(dummy_reverse, CK_SIZE, &tmp)) != nullptr) {
				if (tmp > max_score) {
					max_score = tmp;
					candidate.clear();
				}
				candidate.push_back(&position[y][CK_SIZE-1-(pres-dummy_reverse)]);
				break;
			}
		}
	}

	// 纵向匹配棋型
	for (int i = 0, j = CK_SIZE - 1; i < CK_SIZE; i++, j--) {
		if (position[i][x] != EMPTY) {
			dummy_forward[i] = position[i][x] == enemy ? '2' : '1';
			dummy_reverse[j] = dummy_forward[i];
		} else {
			dummy_forward[i] = dummy_reverse[j] = '0';
		}
	}

	for (int i = 0, j = CK_SIZE - 1; i < CK_SIZE; i++, j--) {
		const char *pres = nullptr;
		int tmp;
		if (dummy_forward[i] == '0') {
			dummy_forward[i] = '1';

			if ((pres = match(dummy_forward, CK_SIZE, &tmp)))
		}
	}
}

