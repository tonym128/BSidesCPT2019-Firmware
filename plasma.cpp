#include "plasma.hpp"

// Plasma
FIXPOINT dist(FIXPOINT a, FIXPOINT b, FIXPOINT c, FIXPOINT d)
{
	FIXPOINT alessc = (a - c);
	FIXPOINT blessd = (b - d);
	alessc = FIXP_MULT(alessc,alessc);
	blessd = FIXP_MULT(blessd,blessd);

	return sqrtF2F(alessc+blessd);
}

FIXPOINT fix_mytime = FIXP_1;
FIXPOINT value;

bool plasma(GameBuff *gameBuff)
{
    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b) {
		return true;
	}
	
	FIXPOINT fix_100 = INT_TO_FIXP(100);
	FIXPOINT fix_128 = INT_TO_FIXP(128);
	FIXPOINT fix_192 = INT_TO_FIXP(192);
	FIXPOINT fix_64 = INT_TO_FIXP(64);
	FIXPOINT fix_8 = INT_TO_FIXP(8);
	FIXPOINT fix_7 = INT_TO_FIXP(7);
	FIXPOINT fix_32 = INT_TO_FIXP(32);
	FIXPOINT fix_4 = INT_TO_FIXP(4);
	FIXPOINT fix_y = FIXP_1;
	FIXPOINT fix_x = FIXP_1;

 	fix_mytime+=FIXP_1/2;

	for (int y = 0; y < gameBuff->HEIGHT/2; y += 1)
	{
		fix_y = INT_TO_FIXP(y);
		for (int x = 0; x < gameBuff->WIDTH/2; x += 1)
		{
			fix_x = INT_TO_FIXP(x);

			value = FIXPOINT_SIN(FIXP_DIV(dist(fix_x + fix_mytime, fix_y, fix_128, fix_128), fix_8)) + 
				FIXPOINT_SIN(FIXP_DIV(dist(fix_x, fix_y, fix_64, fix_64),fix_8)) + 
				FIXPOINT_SIN(FIXP_DIV(dist(fix_x, fix_y + FIXP_DIV(fix_mytime, fix_7), fix_192, fix_64), fix_7)) + 
				FIXPOINT_SIN(FIXP_DIV(dist(fix_x, fix_y, fix_192, fix_100),fix_8));

			gameBuff->consoleBuffer[x + y*gameBuff->WIDTH] = FIXP_TO_INT(FIXP_MULT(fix_4 + value,fix_32));
		}
	}
	drawScreenDouble(gameBuff);
	return false;
}

// End Plasma