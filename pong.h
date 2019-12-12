#ifndef _PONG_H
#define _PONG_H
#include "game.hpp"

struct PongGame {
    bool attract;
    bool restart;
    int score1;
    int score2;

    FIXPOINT ball_x;
    FIXPOINT ball_y;
    FIXPOINT ball_dx;
    FIXPOINT ball_dy;

    FIXPOINT paddle_1_x;
    FIXPOINT paddle_1_y;
    FIXPOINT paddle_2_x;
    FIXPOINT paddle_2_y;
};

bool pongGameLoop(GameBuff *gameBuff);

#endif // _PONG_H