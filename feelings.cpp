#include "feelings.hpp"
#include "headerimages/emo_angry.jpg.h"
#include "headerimages/emo_cool.jpg.h"
#include "headerimages/emo_kissy.jpg.h"
#include "headerimages/emo_left.jpg.h"
#include "headerimages/emo_right.jpg.h"
#include "headerimages/emo_sad.jpg.h"
#include "headerimages/emo_scared.jpg.h"
#include "headerimages/emo_tongue.jpg.h"

static bool processFeelingsInput(GameBuff *gameBuff)
{
    if (gameBuff->playerKeys.debouncedInput) {
        if (gameBuff->playerKeys.up) screen = 0;
        else if (gameBuff->playerKeys.down) screen = 1;
        else if (gameBuff->playerKeys.left) screen = 2;
        else if (gameBuff->playerKeys.right) screen = 3;
        else if (gameBuff->playerKeys.select) screen = 4;
        else if (gameBuff->playerKeys.start) screen = 5;
        else if (gameBuff->playerKeys.b) screen = 6;
        else if (gameBuff->playerKeys.a) screen = 7;

        if ((gameBuff->playerKeys.a && gameBuff->playerKeys.b) || (gameBuff->playerKeys.start && gameBuff->playerKeys.select)) {
            return true;
        }
    }
    return false;
}

static void displayFeelings(GameBuff *gameBuff) {
    if (last_screen != screen) {
        last_screen = screen;
        return;
    }
    
    const unsigned char* feelingImage;
    int size;
    switch (screen) {
        case 0:
            feelingImage = emo_sad_jpg;
            size = sizeof(emo_sad_jpg);
            break;
        case 1:
            feelingImage = emo_kissy_jpg;
            size = sizeof(emo_kissy_jpg);
            break;
        case 2:
            feelingImage = emo_cool_jpg;
            size = sizeof(emo_cool_jpg);
            break;
        case 3:
            feelingImage = emo_angry_jpg;
            size = sizeof(emo_angry_jpg);
            break;
        case 4:
            feelingImage = emo_left_jpg;
            size = sizeof(emo_left_jpg);
            break;
        case 5:
            feelingImage = emo_right_jpg;
            size = sizeof(emo_right_jpg);
            break;
        case 6:
            feelingImage = emo_scared_jpg;
            size = sizeof(emo_scared_jpg);
            break;
        case 7:
            feelingImage = emo_tongue_jpg;
            size = sizeof(emo_tongue_jpg);
            break;
    }

    TJpgDec.drawJpg(0,0,feelingImage,size);
}

bool feelingsLoop(GameBuff* gameBuff) {
    displayFeelings(gameBuff);
    updateMinTime(100);
    return processFeelingsInput(gameBuff);
};

void feelingsInit() {
    
};
