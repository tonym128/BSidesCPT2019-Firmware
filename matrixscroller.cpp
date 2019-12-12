#include "matrixscroller.hpp"

struct Matrix
{
    char *scrollerText = (char*)"Send codes over serial port to unlock features";

    bool firstRun = true;
    bool decode = false;
    int linesY[60];
    uint8_t lineSpeed[60];
    uint8_t lineColour[60];
    bool linesEnabled[60];

    double jitter = 0.01f;
    double maxjitter = 0.02f;
    double minjitter = 0.00f;

    double zoom = 0.9f;
    double maxzoom = 1.0f;
    double minzoom = 0.8f;
    uint8_t frameCounter = 0;
    int startName = -360;
    int endName = 360;
    int nameX = startName;

} matrixData;

bool matrixLoop(GameBuff *gameBuff)
{
    if (matrixData.frameCounter==255) {
        matrixData.frameCounter=0;
    } else {
        matrixData.frameCounter++;
    }
    // Initial Setup
    if (matrixData.firstRun)
    {
        for (int i = 0; i < 60; i++)
        {
            matrixData.linesEnabled[i] = false;
            matrixData.linesY[i] = -29 * 8;
            matrixData.lineSpeed[i] = rand() % 4;
            matrixData.firstRun = false;
        }
    }

    // Input
    if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.a && gameBuff->playerKeys.down && gameBuff->playerKeys.start)
        matrixData.decode = !matrixData.decode;

    if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.a && gameBuff->playerKeys.b)
        return true;

    if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select)
        return true;

    // Display
    displayClear(gameBuff, 0x00);
    ++matrixData.nameX;
    if (matrixData.nameX == matrixData.endName) matrixData.nameX = matrixData.startName;
    drawString2x(gameBuff,gameBuff->badgeState->name,matrixData.nameX,gameBuff->HEIGHT/16,0x6E,0);
    drawScreenDouble(gameBuff);
    drawScreenDouble(gameBuff);

    for (int i = 0; i < 60; i++)
    {
        if (matrixData.linesEnabled[i])
        {
            char *LastChar = &matrixData.scrollerText[28];
            if (matrixData.decode) {
                drawString(gameBuff, matrixData.scrollerText, matrixData.linesY[i] * 8, (i % 30) * 8, matrixData.lineColour[i], 0x00);
                drawString(gameBuff, LastChar, matrixData.linesY[i] * 8 + 28 * 8, (i % 30) * 8, 0xFF, 0x00);
            } else {
                drawPigString(gameBuff, matrixData.scrollerText, matrixData.linesY[i] * 8, (i % 30) * 8, matrixData.lineColour[i], 0x00);
                drawPigString(gameBuff, LastChar, matrixData.linesY[i] * 8 + 28 * 8, (i % 30) * 8, 0xFF, 0x00);
            }

//          if (frameCounter % matrixData.lineSpeed[i])
            matrixData.linesY[i] += matrixData.lineSpeed[i];
        }

        if (matrixData.linesY[i] > gameBuff->WIDTH)
        {
            matrixData.linesEnabled[i] = false;
        }
    }
    
    // Seed Data
    int random = rand() % 120;
    if (random < 60)
    {
        if (!matrixData.linesEnabled[random])
        {
            matrixData.linesEnabled[random] = true;
            matrixData.linesY[random] = -46 * 8;
            matrixData.lineSpeed[random] = rand() % 4;
            matrixData.lineColour[random] = rand() % 256;
        }
    }

    // Mess with screen code... didn't really pan out so cool.
    // Dimensions imageDim;
	// imageDim.width = 240;
	// imageDim.height = 240;
	// imageDim.y = 0;
	// imageDim.x = 0;

    // matrixData.jitter += (rand()%100-50)/1000.0f;
    // if (matrixData.jitter < matrixData.minjitter) matrixData.jitter = matrixData.minjitter;
    // if (matrixData.jitter > matrixData.maxjitter) matrixData.jitter = matrixData.maxjitter;

    // matrixData.zoom += (rand()%100-50)/1000.0f;
    // if (matrixData.zoom < matrixData.minzoom) matrixData.zoom = matrixData.minzoom;
    // if (matrixData.zoom > matrixData.maxzoom) matrixData.zoom = matrixData.maxzoom;

  	// rotateObject(gameBuff, imageDim, matrixData.jitter, matrixData.zoom, gameBuff->consoleBuffer, 0x00);

    return false;
}