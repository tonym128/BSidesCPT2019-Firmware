#include "scroller.hpp"
ScrollerData *scrollerData = nullptr;

bool processScrollerInput(GameBuff *gameBuff)
{
    return gameBuff->playerKeys.a && gameBuff->playerKeys.b;
}

bool displayScroller(GameBuff *gameBuff)
{
    scrollerData->line = scrollerData->firstLine;

    for (int textline = 0; textline < textlines; textline++)
    {
        if (scrollerData->line > -16 && scrollerData->line < gameBuff->HEIGHT + 16)
        {
            if (scrollerData->line < 80)
            {
                drawString2x(gameBuff, scrollerData->scrollerText[textline], 0, scrollerData->line, MakeColor((scrollerData->line + 80) / 32, (scrollerData->line + 120) / 32, (scrollerData->line + 80) / 64), 0x00);
            }
            else
            {
                drawString2x(gameBuff, scrollerData->scrollerText[textline], 0, scrollerData->line, 0xff, 0x00);
            }
        }
        scrollerData->line += 16;
    }
    scrollerData->firstLine--;

    if (scrollerData->line < 0)
        return true;
    return false;
}

void scrollerInit(GameBuff *gameBuff)
{
    if (scrollerData == nullptr)
    {
        scrollerData = new ScrollerData();
    }

    scrollerData->exitscroller = false;
    scrollerData->firstLine = gameBuff->HEIGHT;
    scrollerData->firstScrollRun = true;
    scrollerData->startScrollTime = gameBuff->timeInMillis;
    scrollerData->currentScrollTime = gameBuff->timeInMillis;
}

void scrollerSetString(char newlines[textlines][16])
{
    for (int line = 0; line < textlines; line++)
    {
        sprintf(scrollerData->scrollerText[line], "%s", newlines[line]);
    }
}

void deInitScroller()
{
    if (scrollerData != nullptr)
    {
        free(scrollerData);
        scrollerData = nullptr;
    }
}

bool scrollerLoop(GameBuff *gameBuff)
{
    displayClear(gameBuff, 0x00);

    if (scrollerData == nullptr || scrollerData->firstScrollRun)
    {
        scrollerInit(gameBuff);
        scrollerData->firstScrollRun = false;
    }

    if (processScrollerInput(gameBuff))
        scrollerData->exitscroller = true;
    ;

    if (displayScroller(gameBuff))
        scrollerData->exitscroller = true;

    if (scrollerData->exitscroller)
    {
        deInitScroller();
        return true;
    }

    return false; // Not done
}
