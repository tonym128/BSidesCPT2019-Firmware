#include "achievements.h"
#include "pong.h"

bool playPong = false;
bool canPlayPong = false;
bool firstRunAchievments = true;

void displayAchievement(GameBuff *gameBuff,int y, char *name, bool achieved) {
    if (achieved) {
        drawString(gameBuff,(char*)"YES!",0,y,0x1C,0x00);
        drawString(gameBuff,name,50,y,0x1C,0x00);
    } else {
        drawString(gameBuff,(char*)"no..",0,y,0xE0,0x00);
        drawString(gameBuff,name,50,y,0xE0,0x00);
    }
}

bool displayAchievements(GameBuff *gameBuff) {
    int y = 1;
    drawString2x(gameBuff,(char*)"Achievements!",0,0,0xFF,0x00);

    displayAchievement(gameBuff,20*++y,(char*)"Setup Name", gameBuff->achievementData->setupName);
    displayAchievement(gameBuff,20*++y,(char*)"View Schedule", gameBuff->achievementData->scheduleView);
    displayAchievement(gameBuff,20*++y,(char*)"View About", gameBuff->achievementData->aboutView);
    displayAchievement(gameBuff,20*++y,(char*)"Play Asteroids", gameBuff->achievementData->asteroidsPlay);
    displayAchievement(gameBuff,20*++y,(char*)"Play cryptoHunterPlay", gameBuff->achievementData->cryptoHunterPlay);
    displayAchievement(gameBuff,20*++y,(char*)"300 in Asteroids", gameBuff->achievementData->asteroids300);
    displayAchievement(gameBuff,20*++y,(char*)"Finish CryptoHunter", gameBuff->achievementData->cryptoHunterFinish);

    if (canPlayPong) {
        drawString(gameBuff,(char*)"Press Start to Play Pong!",10,240-10,0xFF,0x00);
    }

    return false;
}

bool displayAchievementsLoop(GameBuff *gameBuff) {
    if (firstRunAchievments) {
        canPlayPong = gameBuff->achievementData->aboutView & 
            gameBuff->achievementData->asteroids300 &
            gameBuff->achievementData->asteroidsPlay &
            gameBuff->achievementData->cryptoHunterFinish &
            gameBuff->achievementData->cryptoHunterPlay &
            gameBuff->achievementData->setupName &
            gameBuff->achievementData->scheduleView;
        firstRunAchievments = false;
    }

    if (playPong) {
        if (pongGameLoop(gameBuff)) {
            playPong = false;
        }

        return false;
    } else {
        displayAchievements(gameBuff);

        if (gameBuff->playerKeys.debouncedInput) {
            if (gameBuff->playerKeys.start && gameBuff->playerKeys.select) {
                firstRunAchievments = true;
                return true;
            }

            if (canPlayPong && (gameBuff->playerKeys.start || gameBuff->playerKeys.select)) {
                playPong = true;
            }
        }
    }

    return false;
}
