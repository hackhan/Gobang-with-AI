#include <graphics.h>
#include <stdlib.h>
#include <windows.h>
#include "gobang.h"

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

/*
 * ��һ����ά�����ʾ������ÿ�����ӵ��״̬��
 * 0 �������ӣ�1 ������ӣ�2 �������
 */
int position[CK_SIZE][CK_SIZE];

/*
 * ��¼��ǰ�����������ӣ�ֵΪ true ʱ�ú����£�
 * ����ð�����
 */
bool setblack = true;

int main() {
	initgraph(WINDOW_SIZE, WINDOW_SIZE);
	setcaption("������");
	setbkcolor(EGERGB(218, 165, 105));

	bool flag = select_order();
	draw_board();

	// ����ǰ������������Ϣ
	mouse_msg temp;
	while (mousemsg()) temp = getmouse();

	int x = -1, y = -1;
	do {
		if (flag) {
			ai(x, y);
		} else {
			mouse_msg msg = { 0 };
			for ( ; is_run(); delay_fps(60)) {
				while (mousemsg())
					msg = getmouse();
				if (msg.is_up()) {
					x = msg.x / UNIT_SIZE;
					y = msg.y / UNIT_SIZE;
					break;
				}
			}
		}
		if (set_chess(x, y))
			flag = !flag;
	} while (!is_gameover(x, y));

	Sleep(10);
	MessageBox(NULL, setblack ? " ����ʤ��" : " ����ʤ��", 
			   "Game Over", MB_OK | MB_ICONINFORMATION);

	for (key_msg k = { 0 }; k = getkey(), k.key != key_esc; );
	closegraph();
	return 0;
}

/*
 * ѡ���ս����(�������� / �������)
 * ���ѡ����������򷵻� true�����򷵻� false
 */
bool select_order() {
	setcolor(EGERGB(0, 0, 0));
	setfont(-19, 0, "����");
	outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "�� ��������");
	outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "�� �������");
	setfont(-16, 0, "����");
	outtextxy(9 * UNIT_SIZE, 14 * UNIT_SIZE, "�� �� ѡ��  Enter ȷ��");
	setfont(-19, 0, "����");

	bool flag = true;
	for (key_msg k = { 0 }; k = getkey(), k.key != key_esc; ) {
		switch (k.key) {
			case 38: outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "��");
					 outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "��");
					 flag = true;
					 break;
			case 40: outtextxy(6 * UNIT_SIZE - 4, 6 * UNIT_SIZE, "��");
					 outtextxy(6 * UNIT_SIZE - 4, 7 * UNIT_SIZE, "��");
					 flag = false;
					 break;
			case 13: cleardevice();
					 return flag;
		}
	}
	exit(0);
}


/*
 * ��������
 */
void draw_board() {
	setcolor(EGERGB(0, 0, 0));
	setfillcolor(EGERGB(0, 0, 0));
	
	// ����������
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

	
	// ������Ԫ����Χ�ĸ���λ�ĺڵ�
	PIMAGE img = newimage();
	getimage(img, "res/��.ico");
	putimage(CK_SIZE / 2 * UNIT_SIZE, CK_SIZE / 2 * UNIT_SIZE, img);
	putimage(3 * UNIT_SIZE, 3 * UNIT_SIZE, img);
	putimage((CK_SIZE - 4) * UNIT_SIZE, 3 * UNIT_SIZE, img);
	putimage(3 * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);
	putimage((CK_SIZE - 4) * UNIT_SIZE, (CK_SIZE - 4) * UNIT_SIZE, img);

	delimage(img);
}

bool set_chess(int x, int y) {
	if (position[y][x] != 0) return false;

	PIMAGE img = newimage();
	getimage(img, setblack ? "res/black.ico" : "res/white.ico");
	putimage(x * UNIT_SIZE, y * UNIT_SIZE, img);
	position[y][x] = setblack ? 1 : 2;
	setblack = !setblack;
	return true;
}

bool is_gameover(int x, int y) {
	vector<int> five(5, setblack ? 2 : 1);
	vector<int> ivec;

#ifdef DEBUG
	for (auto it : five) 
		cout << it << " ";
	cout << "\n";
#endif	
	/*
	 * �� position ����ĵ� y �п����� ivec �У�Ȼ�����������Ƿ����
	 * ��������ͬ�����Ԫ��
	 */
	copy(position[y], position[y] + CK_SIZE, back_inserter(ivec));
	auto it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
	if (it != ivec.end()) return true;
	ivec.clear();

	/*
	 * �� position ����� x �п����� ivec �У�Ȼ�����������Ƿ����
	 * ��������ͬ�����Ԫ��
	 */
	for (int i = 0; i < CK_SIZE; i++) {
		ivec.push_back(position[i][x]);
	}
	it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
	if (it != ivec.end()) return true;
	ivec.clear();

	/*
	 * �� position[y][x] ���ڵ� "\" ����б�߿����� ivec �У�����
	 * �Ƿ����������ͬ�����Ԫ��
	 */
	int _x, _y;
	x < y ? (_x = x - x, _y = y - x) : (_x = x - y, _y = y - y);
	for ( ; _x < CK_SIZE && _y < CK_SIZE; _x++, _y++) {
		ivec.push_back(position[_y][_x]);
	}
	it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
	if (it != ivec.end()) return true;
	ivec.clear();

	/*
	 * �� position[y][x] ���ڵ� "/" ����б�߿����� ivec �У�����
	 * �Ƿ����������ͬ�����Ԫ��
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
#ifdef DEBUG
	cout << "ivec: ";
	for (auto it : ivec) 
		cout << it << " ";
	cout << "\n";
#endif
	it = search(ivec.begin(), ivec.end(), five.begin(), five.end());
	if (it != ivec.end()) return true;

	return false;
}

