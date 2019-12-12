#include "about.hpp"
#include "background.h"
#include "headerimages/TonyM.jpg.h"
#include "headerimages/Mike.jpg.h"
#include "headerimages/DaVinciLabs.jpg.h"
#include "headerimages/christog.jpg.h"
#include "headerimages/justinp.jpg.h"
#include "headerimages/philipg.jpg.h"

static const char *scroller = "Credits : Mike Davis - Hardware, Tony Mamacos - Code, Justin Priday - Case, Christo Goosen & Philip Goosen - Assembly, DaVinciLabs - 3D Printing";

struct Abouts
{
    int scrollerFrameCounter = 0;
    int endCounter = strlen(scroller) * 8 + 240 + 60;
    int x, y, yOffset;
    Dimensions dim1, dim2;
};

Abouts *abouts = nullptr;

void sinScreen(GameBuff *gameBuff, int counter, Abouts *a)
{
    int pixelFrom;
    int pixelTo;
    
    for (a->x = 0; a->x < gameBuff->WIDTH; a->x++)
    {
        a->yOffset = FIXP_TO_INT(FIXPOINT_SIN(FLOAT_TO_FIXP(a->x / 15.0f)) * 5);
        a->y = 0;
        if (a->yOffset < 0)
        {
            for (a->y = gameBuff->HEIGHT; a->y > -1 * a->yOffset; a->y--)
            {
                pixelFrom = (a->y + a->yOffset) * gameBuff->WIDTH + a->x;
                pixelTo = a->y * gameBuff->WIDTH + a->x;

                if (pixelTo >= 0 && pixelFrom >= 0 && pixelTo < gameBuff->MAXPIXEL && pixelFrom < gameBuff->MAXPIXEL)
                    gameBuff->consoleBuffer[pixelTo] = gameBuff->consoleBuffer[pixelFrom];
            }
        }
        else
        {
            for (a->y = 0; a->y < gameBuff->HEIGHT - a->yOffset; a->y++)
            {
                pixelFrom = (a->y + a->yOffset) * gameBuff->WIDTH + a->x;
                pixelTo = a->y * gameBuff->WIDTH + a->x;

                if (pixelTo >= 0 && pixelFrom >= 0 && pixelTo < gameBuff->MAXPIXEL && pixelFrom < gameBuff->MAXPIXEL)
                    gameBuff->consoleBuffer[pixelTo] = gameBuff->consoleBuffer[pixelFrom];
            }
        }
    }
}

bool displayAbout(GameBuff *gameBuff)
{
    if (abouts == nullptr)
    {
        abouts = new Abouts();

        abouts->dim1.x = 0;
        abouts->dim1.y = 0;
        abouts->dim1.height = 80;
        abouts->dim1.width = 240;

        abouts->dim2.x = 0;
        abouts->dim2.y = 0;
        abouts->dim2.height = 80;
        abouts->dim2.width = 542;

        TJpgDec.setJpgScale(1);
        TJpgDec.setCallback(displayImage);

        return false;
    }

    // Draw Sine Scroller
    abouts->scrollerFrameCounter += 3;

    displayClear(gameBuff, 0x24);

    // Draw Background
    abouts->dim2.x = abouts->scrollerFrameCounter / 3;
    drawObjectScrollLoop(gameBuff, abouts->dim1, abouts->dim2, background_image, -1);
    drawScreenDouble(gameBuff);

    drawString(gameBuff, (char *)scroller, gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter, gameBuff->HEIGHT / 8 + 4, 0xFE, 0x00);

    //uint16_t w = 0, h = 0;
    //TJpgDec.getJpgSize(&w, &h, TonyM_jpg, sizeof(TonyM_jpg));

    int curPixel = gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter + 80;
    if (curPixel > -64 && curPixel < 120)
        TJpgDec.drawJpg(curPixel, 50, Mike_jpg, sizeof(Mike_jpg));

    curPixel = gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter + 280;
    if (curPixel > -64 && curPixel < 120)
        TJpgDec.drawJpg(curPixel, 50, TonyM_jpg, sizeof(TonyM_jpg));

    curPixel = gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter + 450;
    if (curPixel > -64 && curPixel < 120)
        TJpgDec.drawJpg(curPixel, 50, justinp_jpg, sizeof(justinp_jpg));

    curPixel = gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter + 630;
    if (curPixel > -64 && curPixel < 120)
        TJpgDec.drawJpg(curPixel, 50, christog_jpg, sizeof(christog_jpg));

    curPixel += 130;
    if (curPixel > -100 && curPixel < 140)
        TJpgDec.drawJpg(curPixel, 50, philip_bsides_jpg, sizeof(philip_bsides_jpg));

    curPixel = gameBuff->WIDTH + 10 - abouts->scrollerFrameCounter + 970;
    if (curPixel > -64 && curPixel < 120)
        TJpgDec.drawJpg(curPixel, 50, DaVinciLabs_jpg, sizeof(DaVinciLabs_jpg));

    sinScreen(gameBuff, abouts->scrollerFrameCounter, abouts);

    drawScreenDouble(gameBuff);

    if (abouts->scrollerFrameCounter > abouts->endCounter || (gameBuff->playerKeys.start && gameBuff->playerKeys.debouncedInput))
    {
        free(abouts);
        abouts = nullptr;

        if (!gameBuff->achievementData->aboutView) {
            gameBuff->achievementData->aboutView = true;
            gameBuff->achievementData->newAchievment = true;
            saveAchievements(gameBuff->achievementData);
        }

        return true;
    }

    return false;
}
