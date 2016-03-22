#include <graphics.h>
#include "gobang.h"

int main() {
	initgraph(WINDOW_SIZE, WINDOW_SIZE);
	setcaption("������");
	setbkcolor(EGERGB(218, 165, 105));
	draw_board();
	getch();
	closegraph();
	return 0;
}

void draw_board() {
	setcolor(EGERGB(0, 0, 0));
	setfillcolor(EGERGB(0, 0, 0));
	
	/*
	 * �������̵�������
	 */
	for (int i = 0; i < CK_SIZE; i++) {
		bar(CELL_SIZE / 2 - 1, 
			CELL_SIZE / 2 - 1 + UNIT_SIZE * i,
			CELL_SIZE / 2 + 1 + WINDOW_SIZE - CELL_SIZE,
			CELL_SIZE / 2 + 1 + UNIT_SIZE * i);
	}
	for (int i = 0; i < CK_SIZE; i++) {
		bar(CELL_SIZE / 2 - 1 + UNIT_SIZE * i,
			CELL_SIZE / 2 - 1,
			CELL_SIZE / 2 + 1 + UNIT_SIZE * i,
			CELL_SIZE / 2 + 1 + WINDOW_SIZE - CELL_SIZE);
	}

	PIMAGE img;
	img = newimage();

	/*
	 * ������Ԫ����Χ�ĸ���λ�ĺڵ�
	 */
	getimage(img, "res/��.ico");
	putimage(CK_SIZE / 2 * UNIT_SIZE, CK_SIZE / 2 * UNIT_SIZE, img);
	putimage(3 * UNIT_SIZE, 3 * UNIT_SIZE, img);
	putimage((CK_SIZE - 4) * UNIT_SIZE, 3 * UNIT_SIZE, img);
	putimage(3 * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);
	putimage((CK_SIZE - 4) * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);

	delimage(img);
}
