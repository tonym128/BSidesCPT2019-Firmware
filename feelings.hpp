// https://openmoji.org
#ifndef FEELINGS_H
#define FEELINGS_H 1

#include "game.hpp"

#ifndef MYFONT_H_
#include "myfont.hpp"
#endif //MYFONT_H_

static uint8_t screen = 1;
static uint8_t last_screen = -1;

bool feelingsLoop(GameBuff* gameBuff);
void feelingsInit();

#endif //FEELINGS_H