#ifndef SCROLLER_H
#define SCROLLER_H 1

#include "game.hpp"

#ifndef MYFONT_H_
#include "myfont.hpp"
#endif //MYFONT_H_
const static int textlines = 20;

struct ScrollerData {
    long startScrollTime = 0;
    long currentScrollTime = 0;
    int firstLine;

    char scrollerText[textlines][16];

    int line;
    bool firstScrollRun = true;
    bool exitscroller = false;
};

bool scrollerLoop(GameBuff*);
void scrollerSetString(char[textlines][16]);
void deInitScroller();

#endif