#include "pong.h"
const static int BALL_WIDTH = 2;
const static int BALL_HEIGHT = 2;

const static int PADDLE_SPACE = 5;
const static int PADDLE_WIDTH = 10;
const static int PADDLE_HEIGHT = 40;

const static int PADDLE_1_COLOUR = 0xE0;
const static int PADDLE_2_COLOUR = 0x03;
const static int BALL_COLOUR = 0xFF;
const static int BACKGROUND_COLOUR = 0x00;

const static int TEXT_COLOUR = 0xFF;

PongGame *pongGame = nullptr;

void pongDisplay(GameBuff *gameBuff) {
    displayClear(gameBuff,BACKGROUND_COLOUR);
    // Draw start instructions
    if (pongGame->restart && (gameBuff->timeInMillis/1000) % 2 == 0) {
        drawString2x(gameBuff,(char*)"Press A or B",gameBuff->WIDTH/2-(16*6),gameBuff->HEIGHT/2,TEXT_COLOUR,0);
        drawString2x(gameBuff,(char*)"To Play",gameBuff->WIDTH/2-(16*4),gameBuff->HEIGHT/2+20,TEXT_COLOUR,0);
    }

    // Draw Scores
    char score_char[5];
    sprintf(score_char,"%4d",pongGame->score1);
    drawString2x(gameBuff,score_char,40,0,PADDLE_1_COLOUR,0x00);
    sprintf(score_char,"%4d",pongGame->score2);
    drawString2x(gameBuff,score_char,100,0,PADDLE_2_COLOUR,0x00);

    drawString2x(gameBuff,(char*)":",120,0,TEXT_COLOUR,0x00);

    // Draw Ball
    Dimensions dim;
    dim.x = FIXP_TO_INT(pongGame->ball_x);
    dim.y = FIXP_TO_INT(pongGame->ball_y);
    dim.width = BALL_WIDTH;
    dim.height = BALL_HEIGHT;
    drawBlock(gameBuff,dim,BALL_COLOUR);

    // Draw Paddle 1
    dim.x = FIXP_TO_INT(pongGame->paddle_1_x);
    dim.y = FIXP_TO_INT(pongGame->paddle_1_y);
    dim.width = PADDLE_WIDTH;
    dim.height = PADDLE_HEIGHT;
    drawBlock(gameBuff,dim,PADDLE_1_COLOUR);

    // Draw Paddle 2
    dim.x = FIXP_TO_INT(pongGame->paddle_2_x);
    dim.y = FIXP_TO_INT(pongGame->paddle_2_y);
    drawBlock(gameBuff,dim,PADDLE_2_COLOUR);
}

void pongAttract(GameBuff *gameBuff) {
    displayClear(gameBuff,0x00);
    drawString2x(gameBuff,(char*)"PONG!",gameBuff->WIDTH/2-(16*2),0,PADDLE_1_COLOUR,-1);
    if ((gameBuff->timeInMillis/1000) % 2 == 0) {
        drawString2x(gameBuff,(char*)"Press Start",gameBuff->WIDTH/2-(16*5),gameBuff->HEIGHT/2,TEXT_COLOUR,0);
        drawString2x(gameBuff,(char*)"To Play",gameBuff->WIDTH/2-(16*5),gameBuff->HEIGHT/2+20,TEXT_COLOUR,0);
    }
}

void pongComputer(GameBuff *gameBuff) {
    FIXPOINT desired = pongGame->ball_y;
    if (desired < pongGame->paddle_2_y + INT_TO_FIXP(PADDLE_HEIGHT/3)) {
            if (pongGame->paddle_2_y + INT_TO_FIXP(PADDLE_HEIGHT/3) - desired > FIXP_1 * 2)
                pongGame->paddle_2_y -= FIXP_1 * 4;
    } else if (desired > pongGame->paddle_2_y + INT_TO_FIXP(PADDLE_HEIGHT/3)) {
            if (desired - pongGame->paddle_2_y + INT_TO_FIXP(PADDLE_HEIGHT/3) > FIXP_1 * 2)
                pongGame->paddle_2_y += FIXP_1 * 4;
    }

    if (pongGame->paddle_2_y < INT_TO_FIXP(0)) {
        pongGame->paddle_2_y = INT_TO_FIXP(0);
    }

    if (pongGame->paddle_2_y > INT_TO_FIXP(gameBuff->HEIGHT-PADDLE_HEIGHT)) {
        pongGame->paddle_2_y = INT_TO_FIXP(gameBuff->HEIGHT-PADDLE_HEIGHT);
    }
}

void pongStart(GameBuff *gameBuff) {
    pongGame->restart = true;
    pongGame->ball_dx = 0;
    pongGame->ball_dy = 0;

    pongGame->ball_x = INT_TO_FIXP(gameBuff->WIDTH/2);
    pongGame->ball_y = INT_TO_FIXP(gameBuff->HEIGHT/2);
}

void pongInput(GameBuff *gameBuff) {
    if (gameBuff->playerKeys.up) {
        pongGame->paddle_1_y -= FIXP_1 * 5;
    }

    if (gameBuff->playerKeys.down) {
        pongGame->paddle_1_y += FIXP_1 * 5;
    }

    if (pongGame->paddle_1_y < INT_TO_FIXP(0)) {
        pongGame->paddle_1_y = INT_TO_FIXP(0);
    }

    if (pongGame->paddle_1_y > INT_TO_FIXP(gameBuff->HEIGHT-PADDLE_HEIGHT)) {
        pongGame->paddle_1_y = INT_TO_FIXP(gameBuff->HEIGHT-PADDLE_HEIGHT);
    }
}    


bool pongUpdate(GameBuff *gameBuff) {
    if (pongGame->restart) {
        // Waiting for a keypress to Start
        if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.b || gameBuff->playerKeys.a)) {
            pongGame->restart = false;
            pongStart(gameBuff);
            pongGame->ball_dx = ((rand()%10)-5) * FIXP_1;
            pongGame->ball_dy = ((rand()%10)-5) * FIXP_1;
            if (pongGame->ball_dx < FIXP_1 && pongGame->ball_dx > INT_TO_FIXP(0)) {
                pongGame->ball_dx = FIXP_1;
            }
            else if (pongGame->ball_dx > FIXP_1 * -1 && pongGame->ball_dx < INT_TO_FIXP(0)) {
                pongGame->ball_dx = FIXP_1 * -1;
            }
            pongGame->restart = false;

        }

        return false;
    }

    // Move the ball
    pongGame->ball_x += pongGame->ball_dx;
    pongGame->ball_y += pongGame->ball_dy;

    // Check non zero speeds
    if (pongGame->ball_dx == 0) pongGame->ball_dx = FIXP_1;
    if (pongGame->ball_dy == 0) pongGame->ball_dy = FIXP_1;

    // Check top wall bounce
    if (pongGame->ball_y < INT_TO_FIXP(0)) {
        pongGame->ball_dy *= -1;
    }

    // Check bottom wall bounce
    if (pongGame->ball_y > INT_TO_FIXP(gameBuff->HEIGHT)) {
        pongGame->ball_dy *= -1;
    }

    bool paddle_hit = false;
    // Check paddle 1 bounce
    if (pongGame->ball_x <= INT_TO_FIXP(PADDLE_WIDTH+PADDLE_SPACE) && pongGame->ball_x >= INT_TO_FIXP(PADDLE_SPACE)) {
        if (pongGame->ball_y >= pongGame->paddle_1_y && pongGame->ball_y <= pongGame->paddle_1_y + INT_TO_FIXP(PADDLE_HEIGHT)) {
            // Hit
            if (pongGame->ball_dx < INT_TO_FIXP(0)) {
                pongGame->ball_dx *= -1;
                paddle_hit = true;
            }
        }
    }
    // Check paddle 2 bounce
    if (pongGame->ball_x >= INT_TO_FIXP(gameBuff->WIDTH-(PADDLE_WIDTH+PADDLE_SPACE)) && pongGame->ball_x <= INT_TO_FIXP(gameBuff->WIDTH - PADDLE_SPACE)) {
        if (pongGame->ball_y >= pongGame->paddle_2_y && pongGame->ball_y <= pongGame->paddle_2_y + INT_TO_FIXP(PADDLE_HEIGHT)) {
            // Hit
            if (pongGame->ball_dx > INT_TO_FIXP(0)) {
                pongGame->ball_dx *= -1;
                paddle_hit = true;
            }
        }
    }

    if (paddle_hit) {
        int div = rand()%10;
        if (div > 0) {
            if (pongGame->ball_dx < INT_TO_FIXP(0)) pongGame->ball_dx -= FIXP_DIV(pongGame->ball_dx,INT_TO_FIXP(div));
            else pongGame->ball_dx += FIXP_DIV(pongGame->ball_dx,INT_TO_FIXP(div));
        }

        div = rand()%10;
        if (div > 0) {
            if (pongGame->ball_dy < INT_TO_FIXP(0)) pongGame->ball_dy -= FIXP_DIV(pongGame->ball_dy,INT_TO_FIXP(div));
            else pongGame->ball_dy += FIXP_DIV(pongGame->ball_dy,INT_TO_FIXP(div));
        }
    }
    // Check paddle 2 win
    if (pongGame->ball_x < INT_TO_FIXP(0)) {
        pongGame->score2 += 1;
        pongGame->restart = true;
        pongGame->ball_x = INT_TO_FIXP(gameBuff->WIDTH/2);
        pongGame->ball_y = INT_TO_FIXP(gameBuff->HEIGHT/2);
        updateMinTime(1000);
    }

    // Check paddle 1 win
    if (pongGame->ball_x > INT_TO_FIXP(gameBuff->WIDTH)) {
        pongGame->score1 += 1;
        pongGame->restart = true;
        pongGame->ball_x = INT_TO_FIXP(gameBuff->WIDTH/2);
        pongGame->ball_y = INT_TO_FIXP(gameBuff->HEIGHT/2);
        updateMinTime(1000);
    }
    
    return false;
}

bool pongGameLoop(GameBuff *gameBuff) {
    if (pongGame == nullptr) {
        pongGame = new PongGame();
        pongGame->attract = true;
    }

    if (pongGame->attract) {
        pongAttract(gameBuff);
        if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.select || gameBuff->playerKeys.select)) {
            pongGame->attract = false;
            pongGame->score1 = 0;            
            pongGame->score2 = 0;            
            pongGame->paddle_1_x = INT_TO_FIXP(PADDLE_SPACE);
            pongGame->paddle_2_x = INT_TO_FIXP(gameBuff->WIDTH-PADDLE_SPACE-PADDLE_WIDTH);
            pongStart(gameBuff);
        }
    } else {
        pongInput(gameBuff);    
        pongComputer(gameBuff);    
        pongUpdate(gameBuff);
        pongDisplay(gameBuff);
    }

    if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.start && gameBuff->playerKeys.select)) {
        free(pongGame);
        pongGame = nullptr;
        return true;
    }

    return false;
}
