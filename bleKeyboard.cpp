#include "bleKeyboard.h"

static int status = 0;
bool displayOnce(GameBuff *gameBuff)
{
	displayClear(gameBuff, 0x00);
    drawString2x(gameBuff,(char *)"   Bluetooth   ", 0, 16, 0xFF, 0);
    drawString2x(gameBuff,(char *)"    GamePad    ", 0, 32, 0xFF, 0);
    drawString2x(gameBuff,(char *)" Press A and B ", 0, 100, 0xFF, 0);
    drawString2x(gameBuff,(char *)"    To Exit    ", 0, 120, 0xFF, 0);
    return true;
}

bool bleKeyboardLoop(GameBuff *gameBuff)
{
    if (status == 0)
    {
        displayOnce(gameBuff);
        status = 1;
    }
    else if (status == 1)
    {
        free(gameBuff->consoleBuffer);
        gameBuff->consoleBuffer = nullptr;
        if (startBLEKeyboard())
        {
            status = 2; // Succesfully started
        }
        else
        {
            status = 8; // Failed to start
        }
    }

    processBLEKeyboard(gameBuff->playerKeys);

    if (gameBuff->playerKeys.debouncedInput)
    if ((gameBuff->playerKeys.a && gameBuff->playerKeys.b) || (gameBuff->playerKeys.start && gameBuff->playerKeys.select))
    {
        stopBLEKeyboard();
        status = 0;
        gameBuff->consoleBuffer = (uint8_t *)calloc(gameBuff->WIDTH * gameBuff->HEIGHT,sizeof(uint8_t));

        return true;
    };

    return false;
}
