#include "tunnel.hpp"
#include "raycaster_images.h"

#define texWidth 64
#define texHeight 64
#define screenWidth 60
#define screenHeight 60

//Make the tables twice as big as the screen. The center of the buffers is now the position (w,h).
byte **distanceTable;
byte **angleTable;
bool initDone = false;
float animation;

void tunnelInit(GameBuff *gameBuff)
{
    int r = screenHeight * 2, c = screenWidth * 2, i;

    distanceTable = (byte **)malloc(r * sizeof(byte *));
    angleTable = (byte **)malloc(r * sizeof(byte *));
    for (i = 0; i < r; i++)
    {
        distanceTable[i] = (byte *)malloc(c * sizeof(byte));
        angleTable[i] = (byte *)malloc(c * sizeof(byte));
    }

    //generate non-linear transformation table, now for the bigger buffers (twice as big)
    for (int y = 0; y < screenHeight * 2; y++)
        for (int x = 0; x < screenWidth * 2; x++)
        {
            byte angle, distance;
            float ratio = 32.0;
            //these formulas are changed to work with the new center of the tables
            distance = int(ratio * texHeight / sqrt(float((x - screenWidth) * (x - screenWidth) + (y - screenHeight) * (y - screenHeight)))) % texHeight;
            angle = (unsigned int)(0.5 * texWidth * atan2(float(y - screenHeight), float(x - screenWidth)) / 3.1416);
            distanceTable[y][x] = distance;
            angleTable[y][x] = angle;
        }
}

bool tunnelLoop(GameBuff *gameBuff)
{
    if (!initDone)
    {
        tunnelInit(gameBuff);
        initDone = true;
    }

    //begin the loop
    animation = (float)gameBuff->timeInMillis / (float)2000.0;

    //calculate the shift values out of the animation value
    int shiftX = int(texWidth * 1.0 * animation);
    int shiftY = int(texHeight * 0.25 * animation);
    //calculate the look values out of the animation value
    //by using sine functions, it'll alternate between looking left/right and up/down
    //make sure that x + shiftLookX never goes outside the dimensions of the table, same for y
    int shiftLookX = screenWidth / 2 + int(screenWidth / 4 * sin(animation));
    int shiftLookY = screenHeight / 2 + int(screenHeight / 4 * sin(animation * 2.0));

    for (int y = 0; y < screenHeight; y++)
        for (int x = 0; x < screenWidth; x++)
        {
            //get the texel from the texture by using the tables, shifted with the animation variable
            //now, x and y are shifted as well with the "look" animation values
            int color = ceiling_image[((unsigned int)(distanceTable[x + shiftLookX][y + shiftLookY] + shiftX) % texWidth) * texWidth +
                                   ((unsigned int)(angleTable[x + shiftLookX][y + shiftLookY] + shiftY) % texHeight)];
            gameBuff->consoleBuffer[y * gameBuff->WIDTH + x] = color;
        }

    drawScreenDouble(gameBuff);
    drawScreenDouble(gameBuff);

    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b)
    {
        int r = screenHeight * 2, c = screenWidth * 2, i;
        for (i = 0; i < r; i++)
        {
            free(distanceTable[i]);
            free(angleTable[i]);
        }
        free(distanceTable);
        free(angleTable);
        initDone = false;
        return true;
    }

    return false;
}