#include "gobang.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void ai(int &x, int &y) {
	srand((unsigned int)time(NULL));
	x = rand() % CK_SIZE;
	y = rand() % CK_SIZE;
}