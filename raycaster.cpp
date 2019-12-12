#include "raycaster.hpp"
#include "raycaster_images.h"

// Tutorial and sample code, modified to my needs from here
// https://lodev.org/cgtutor/files/raycaster_flat.cpp
// Check the full tutorial here
// https://lodev.org/cgtutor/raycasting.html

static uint8_t SCREEN_WIDTH = 240;
static uint8_t SCREEN_HEIGHT = 240;

struct GameStateMaze
{
    uint8_t currentState = 2;
    uint8_t previousState = -1;
    int frameCounter = 0;
    int stageTime = 360; // 6 minutes
    uint8_t endTimer = 0;
    uint8_t endSeconds = 3;
    int seconds = 0;
    bool running = false;
    bool restart = false;
    bool win = false;
    bool exit = false;
    bool traversal[mapWidth][mapHeight];
    bool vrFlags[4];
    bool vrFlagProximity[4];
};

GameStateMaze *gameStateMaze = nullptr;

FIXPOINT posX = INT_TO_FIXP(22), posY = INT_TO_FIXP(22);        //x and y start position
FIXPOINT dirX = INT_TO_FIXP(-1), dirY = INT_TO_FIXP(0);         //initial direction vector
FIXPOINT planeX = INT_TO_FIXP(0), planeY = FLOAT_TO_FIXP(0.66); //the 2d raycaster version of camera plane

void setupGameState() {
    gameStateMaze->currentState = 2;
    gameStateMaze->previousState = -1;
    gameStateMaze->frameCounter = 0;
    gameStateMaze->stageTime = 360;
    gameStateMaze->endTimer = 0;
    gameStateMaze->endSeconds = 3;
    gameStateMaze->seconds = 0;
    gameStateMaze->running = false;
    gameStateMaze->restart = false;
    gameStateMaze->win = false;
    gameStateMaze->exit = false;

    gameStateMaze->vrFlags[0] = false;
    gameStateMaze->vrFlags[1] = false;
    gameStateMaze->vrFlags[2] = false;
    gameStateMaze->vrFlags[3] = false;

    gameStateMaze->vrFlagProximity[0] = false;
    gameStateMaze->vrFlagProximity[1] = false;
    gameStateMaze->vrFlagProximity[2] = false;
    gameStateMaze->vrFlagProximity[3] = false;

    for (int i = 0; i < mapWidth; i++)
    {
        for (int j = 0; j < mapHeight; j++)
        {
            gameStateMaze->traversal[i][j] = 0;
        }
    }
}

bool update(GameBuff *gameBuff, GameStateMaze *gameStateMaze)
{
    // Set the time
    gameStateMaze->seconds = getElapsedSeconds();

    // Check for VR Terminal vrFlagProximity
    // Hardcoded to match map
    gameStateMaze->vrFlagProximity[0] = 0;
    gameStateMaze->vrFlagProximity[1] = 0;
    gameStateMaze->vrFlagProximity[2] = 0;
    gameStateMaze->vrFlagProximity[3] = 0;

    // Check 7
    if ((abs(FIXP_TO_INT(posY) - 0) <= 1) && (abs(FIXP_TO_INT(posX) - 1) <= 1)) {
        gameStateMaze->vrFlagProximity[0] = 1;

        if ((gameBuff->playerKeys.a || gameBuff->playerKeys.b) && gameStateMaze->vrFlags[0] == 0) {
            gameStateMaze->previousState = 8;
            gameStateMaze->currentState = 15;
        }
    } else 
    // Check 8
    if ((abs(FIXP_TO_INT(posY) - 23) <= 1) && (abs(FIXP_TO_INT(posX) - 0) <= 1)) {
        gameStateMaze->vrFlagProximity[1] = 1;

        if ((gameBuff->playerKeys.a || gameBuff->playerKeys.b) && gameStateMaze->vrFlags[1] == 0) {
            gameStateMaze->previousState = 8;
            gameStateMaze->currentState = 15;
        }
    } else 
    // Check 9
    if ((abs(FIXP_TO_INT(posY) - 6) <= 1) && (abs(FIXP_TO_INT(posX) - 11) <= 1)) {
        gameStateMaze->vrFlagProximity[2] = 1;

        if ((gameBuff->playerKeys.a || gameBuff->playerKeys.b) && gameStateMaze->vrFlags[2] == 0) {
            gameStateMaze->previousState = 8;
            gameStateMaze->currentState = 15;
        }
    } else
    // Check 10
    if ((abs(FIXP_TO_INT(posY) - 11) <= 1) && (abs(FIXP_TO_INT(posX) - 11) <= 1)) {
        gameStateMaze->vrFlagProximity[3] = 1;

        if (gameBuff->playerKeys.a || gameBuff->playerKeys.b) {
            gameStateMaze->vrFlags[3] = 1;
        }
    }

    // Check for win state // Win is in the top left
    if (!gameStateMaze->win)
    {
        uint8_t count = ((uint8_t)(gameStateMaze->vrFlags[0])) + ((uint8_t)(gameStateMaze->vrFlags[1])) + ((uint8_t)(gameStateMaze->vrFlags[2])) + ((uint8_t)(gameStateMaze->vrFlags[3]));
        gameStateMaze->win = count == 4;
    }
    else if (gameStateMaze->win)
    {
        if (gameStateMaze->endTimer == 0)
        {
            gameStateMaze->endTimer = getElapsedSeconds() + gameStateMaze->endSeconds;
        }
        else if (getElapsedSeconds() > gameStateMaze->endTimer)
        {
            gameStateMaze->previousState = 3;
            gameStateMaze->currentState = 15;
            gameBuff->stage_time4 = gameStateMaze->seconds;
        }
    }

    /* Don't timeout the maze lets keep it fun for the whole family!
    if (checkTime(gameStateMaze->stageTime))
    {
        // If we have run out of time, stop updating the game
        gameStateMaze->win = false;
        if (gameStateMaze->endTimer == 0)
        {
            gameStateMaze->endTimer = getElapsedSeconds() + gameStateMaze->endSeconds;
        }
        else if (getElapsedSeconds() > gameStateMaze->endTimer)
        {
            gameStateMaze->currentState = 5;
        }
        return false;
    }
    */

    gameStateMaze->traversal[FIXP_INT_PART(posX)][FIXP_INT_PART(posY)] = true;

    //speed modifiers
    FIXPOINT moveSpeed = FLOAT_TO_FIXP(0.1);
    FIXPOINT rotSpeed = FLOAT_TO_FIXP(0.05);

    //move forward if no wall in front of you
    if (gameBuff->playerKeys.up)
    {
        if (worldMap[FIXP_INT_PART(posX + FIXP_MULT(dirX, moveSpeed))][FIXP_TO_INT(posY)] == false)
            posX += FIXP_MULT(dirX, moveSpeed);
        if (worldMap[FIXP_INT_PART(posX)][FIXP_INT_PART(posY + FIXP_MULT(dirY, moveSpeed))] == false)
            posY += FIXP_MULT(dirY, moveSpeed);
    }
    //move backwards if no wall behind you
    if (gameBuff->playerKeys.down)
    {
        if (worldMap[FIXP_INT_PART(posX - FIXP_MULT(dirX, moveSpeed))][FIXP_INT_PART(posY)] == false)
            posX -= FIXP_MULT(dirX, moveSpeed);
        if (worldMap[FIXP_INT_PART(posX)][FIXP_INT_PART(posY - FIXP_MULT(dirY, moveSpeed))] == false)
            posY -= FIXP_MULT(dirY, moveSpeed);
    }
    //rotate to the right
    if (gameBuff->playerKeys.right)
    {
        //both camera direction and camera plane must be rotated
        FIXPOINT oldDirX = dirX;
        dirX = FIXP_MULT(dirX, FIXPOINT_COS(-rotSpeed)) - FIXP_MULT(dirY, FIXPOINT_SIN(-rotSpeed));
        dirY = FIXP_MULT(oldDirX, FIXPOINT_SIN(-rotSpeed)) + FIXP_MULT(dirY, FIXPOINT_COS(-rotSpeed));
        FIXPOINT oldPlaneX = planeX;
        planeX = FIXP_MULT(planeX, FIXPOINT_COS(-rotSpeed)) - FIXP_MULT(planeY, FIXPOINT_SIN(-rotSpeed));
        planeY = FIXP_MULT(oldPlaneX, FIXPOINT_SIN(-rotSpeed)) + FIXP_MULT(planeY, FIXPOINT_COS(-rotSpeed));
    }
    //rotate to the left
    if (gameBuff->playerKeys.left)
    {
        //both camera direction and camera plane must be rotated
        FIXPOINT oldDirX = dirX;
        dirX = FIXP_MULT(dirX, FIXPOINT_COS(rotSpeed)) - FIXP_MULT(dirY, FIXPOINT_SIN(rotSpeed));
        dirY = FIXP_MULT(oldDirX, FIXPOINT_SIN(rotSpeed)) + FIXP_MULT(dirY, FIXPOINT_COS(rotSpeed));
        FIXPOINT oldPlaneX = planeX;
        planeX = FIXP_MULT(planeX, FIXPOINT_COS(rotSpeed)) - FIXP_MULT(planeY, FIXPOINT_SIN(rotSpeed));
        planeY = FIXP_MULT(oldPlaneX, FIXPOINT_SIN(rotSpeed)) + FIXP_MULT(planeY, FIXPOINT_COS(rotSpeed));
    }

    return true;
}

void processInput(GameBuff* gameBuff, GameStateMaze *gameStateMaze)
{
    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b) {
        gameStateMaze->exit = true;
    }

    if (gameBuff->playerKeys.select)
    {
        gameStateMaze->running = false;
        gameStateMaze->restart = true;
    }
}

static void drawFloorTexture(GameBuff *gameBuff, const uint8_t *floor_texture, const uint8_t *ceiling_texture, int side, int x, int y, FIXPOINT fperpWallDist, FIXPOINT frayDirX, FIXPOINT frayDirY, int drawStart, int drawEnd, int lineHeight, FIXPOINT mapY, FIXPOINT mapX, FIXPOINT wallX, FIXPOINT posy, FIXPOINT posx)
{
    //FLOOR CASTING
    FIXPOINT floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

    //4 different wall directions possible
    if (side == 0 && frayDirX > 0)
    {
        floorXWall = mapX;
        floorYWall = mapY + wallX;
    }
    else if (side == 0 && frayDirX < 0)
    {
        floorXWall = mapX + FIXP_1;
        floorYWall = mapY + wallX;
    }
    else if (side == 1 && frayDirY > 0)
    {
        floorXWall = mapX + wallX;
        floorYWall = mapY;
    }
    else
    {
        floorXWall = mapX + wallX;
        floorYWall = mapY + FIXP_1;
    }

    FIXPOINT distWall, distPlayer, currentDist;

    distWall = fperpWallDist;
    distPlayer = 0;

    if (drawEnd < 0)
        drawEnd = SCREEN_HEIGHT; //becomes < 0 when the integer overflows

    //draw the floor from drawEnd to the bottom of the screen
    for (int y = drawEnd + 1; y < SCREEN_HEIGHT; y++)
    {
        currentDist = FIXP_DIV(INT_TO_FIXP(SCREEN_HEIGHT) , INT_TO_FIXP((2 * y) - SCREEN_HEIGHT)); //you could make a small lookup table for this instead

        FIXPOINT weight = FIXP_DIV(currentDist - distPlayer , distWall - distPlayer);

        FIXPOINT currentFloorX = FIXP_MULT(weight, floorXWall) + FIXP_MULT(FIXP_1 - weight, posx);
        FIXPOINT currentFloorY = FIXP_MULT(weight, floorYWall)  + FIXP_MULT(FIXP_1 - weight, posy);

        int floorTexX, floorTexY;
        floorTexX = abs(FIXP_TO_INT(currentFloorX * wall_width) % wall_width);
        floorTexY = abs(FIXP_TO_INT(currentFloorY * wall_height) % wall_height);

        // floor
        gameBuff->consoleBuffer[y * gameBuff->WIDTH + x] = floor_texture[wall_width * floorTexY + floorTexX];
        // ceiling
        gameBuff->consoleBuffer[(SCREEN_HEIGHT - y) * gameBuff->WIDTH + x] = ceiling_texture[wall_width * floorTexY + floorTexX];
    }
}

static void drawWallTexture(GameBuff *gameBuff, const uint8_t *texture, int side, int x, FIXPOINT fperpWallDist, FIXPOINT frayDirX, FIXPOINT frayDirY, FIXPOINT fwallX, int drawStart, int drawEnd, int lineHeight, uint8_t mask)
{
    //x coordinate on the texture
    int ftexX = FIXP_TO_INT(fwallX * wall_width);
    if (side == 0 && frayDirX > 0)
        ftexX = wall_width - 1 - ftexX;
    if (side == 1 && frayDirY < 0)
        ftexX = wall_width - 1 - ftexX;

    for (int y = drawStart; y < drawEnd; y++)
    {
        int d = y * 256 - SCREEN_HEIGHT * 128 + lineHeight * 128; //256 and 128 factors to avoid floats
        // TODO: avoid the division to speed this up
        int texY = ((d * wall_width) / lineHeight) / 256;
        uint8_t color = texture[ftexX + wall_height * texY];
        if (mask != 0xFF) color *= mask;
        gameBuff->consoleBuffer[x + y * gameBuff->WIDTH] = color;
    }
}

void display(GameBuff *gameBuff, GameStateMaze *gameStateMaze)
{
	displayClear(gameBuff, 0x00);

    Dimensions floor;
    floor.x = 0;
    floor.y = SCREEN_HEIGHT / 2;
    floor.height = SCREEN_HEIGHT - floor.y;
    floor.width = SCREEN_WIDTH;

    drawBlock(gameBuff, floor, 0x97);

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        //which box of the map we're in
        FIXPOINT fmapX = FIXP_FIXP_INT_PART(posX);
        FIXPOINT fmapY = FIXP_FIXP_INT_PART(posY);

        //calculate ray position and direction
        FIXPOINT fcameraX = FIXP_DIV(INT_TO_FIXP(2 * x), INT_TO_FIXP(SCREEN_WIDTH)) - FIXP_1; //x-coordinate in camera space
        FIXPOINT frayDirX = dirX + FIXP_MULT(planeX, fcameraX);
        FIXPOINT frayDirY = dirY + FIXP_MULT(planeY, fcameraX);

        //length of ray from current position to next x or y-side
        FIXPOINT fsideDistX;
        FIXPOINT fsideDistY;

        //length of ray from one x or y-side to next x or y-side
        FIXPOINT fdeltaDistX = frayDirX == 0 ? INT32_MAX : FIXP_DIV(FIXP_1, frayDirX);
        FIXPOINT fdeltaDistY = frayDirY == 0 ? INT32_MAX : FIXP_DIV(FIXP_1, frayDirY);

        if (fdeltaDistX < 0)
            fdeltaDistX = -fdeltaDistX;
        if (fdeltaDistY < 0)
            fdeltaDistY = -fdeltaDistY;

        FIXPOINT fperpWallDist;

        //what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; //was there a wall hit?
        int side;    //was a NS or a EW wall hit?

        //calculate step and initial sideDist
        if (frayDirX < 0)
        {
            stepX = -1;
            fsideDistX = FIXP_MULT((posX - fmapX), fdeltaDistX);
        }
        else
        {
            stepX = 1;
            fsideDistX = FIXP_MULT((fmapX + FIXP_1 - posX), fdeltaDistX);
        }
        if (frayDirY < 0)
        {
            stepY = -1;
            fsideDistY = FIXP_MULT((posY - fmapY), fdeltaDistY);
        }
        else
        {
            stepY = 1;
            fsideDistY = FIXP_MULT((fmapY + FIXP_1 - posY), fdeltaDistY);
        }

        //perform DDA
        while (hit == 0)
        {
            //jump to next map square, OR in x-direction, OR in y-direction
            if (fsideDistX < fsideDistY)
            {
                fsideDistX += fdeltaDistX;
                fmapX += INT_TO_FIXP(stepX);
                side = 0;
            }
            else
            {
                fsideDistY += fdeltaDistY;
                fmapY += INT_TO_FIXP(stepY);
                side = 1;
            }
            //Check if ray has hit a wall
            if (worldMap[FIXP_TO_INT(fmapX)][FIXP_TO_INT(fmapY)] > 0)
                hit = 1;
        }

        //Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
        if (side == 0)
            fperpWallDist = frayDirX == 0 ? INT32_MAX : FIXP_DIV(fmapX - posX + FIXP_DIV(INT_TO_FIXP(1 - stepX), INT_TO_FIXP(2)), frayDirX);
        else
            fperpWallDist = frayDirY == 0 ? INT32_MAX : FIXP_DIV(fmapY - posY + FIXP_DIV(INT_TO_FIXP(1 - stepY), INT_TO_FIXP(2)), frayDirY);

        //Calculate height of line to draw on screen
        int lineHeight = fperpWallDist == 0 ? INT32_MAX : FIXP_TO_INT(FIXP_DIV(INT_TO_FIXP(SCREEN_HEIGHT), fperpWallDist));

        //calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0)
            drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT)
            drawEnd = SCREEN_HEIGHT - 1;

        //give x and y sides different brightness
        // if (side == 1) {color = color / 2;}
        int pattern = worldMap[FIXP_TO_INT(fmapX)][FIXP_TO_INT(fmapY)] - 1;
#ifdef DRAWBLOCKS
        drawVertLine2(gameBuff,x,drawStart, drawEnd,0x0F * (pattern+3));
#else
        FIXPOINT fwallX; //where exactly the wall was hit

        if (side == 0)
        {
            fwallX = (FIXP_MULT(fperpWallDist, frayDirY)) + posY;
        }
        else
        {
            fwallX = (FIXP_MULT(fperpWallDist, frayDirX)) + posX;
        }

        fwallX = FIXP_DEC_PART((fwallX));

        //draw the pixels of the stripe as a vertical line
        int mask = 0xFF;
        const uint8_t *image;
       
        switch (pattern)
        {
        case 0:
            image = wall_image;
            mask = 0xFF;
            break;
        case 1:
            image = wall_image;
            mask = 0xE0;
            break;
        case 2:
            image = wall_image;
            mask = 0x3F;
            break;
        case 3:
            image = wall_image;
            mask = 0x92;
            break;
        case 4:
            image = wall_image;
            mask = 0xFF;
            break;
        case 6: // VR 1 No Red
            image = computer_image;
            mask = 0x7F;
            break;
        case 7: // VR 2 No Green
            image = computer_image;
            mask = 0xEF;
            break;
        case 8: // VR 3 No Blue
            image = computer_image;
            mask = 0xFD;
            break;
        case 9: // Main Terminal Full Colour
            image = computer_image;
            mask = 0xFF;
            break;
        }

        drawWallTexture(gameBuff, image, side, x, fperpWallDist, frayDirX, frayDirY, fwallX, drawStart, drawEnd, lineHeight, mask);
        drawFloorTexture(gameBuff, floor_image, ceiling_image, side, x, FIXP_TO_INT(posY), fperpWallDist, frayDirX, frayDirY, drawStart, drawEnd, lineHeight, fmapY, fmapX, fwallX, posY, posX);
#endif
    }

    Dimensions dim;
    char fps[30];
    dim.x = 0;
    dim.y = 10;
    dim.height=32;
    dim.width=gameBuff->WIDTH;

    // Draw VR Flag text
    if (gameStateMaze->vrFlagProximity[0] && gameStateMaze->vrFlags[0] == 0) {
        drawBlock(gameBuff,dim,0x00);
        drawString2x(gameBuff,(char*)"Press A to",0,10,0xFF,0x00);
        drawString2x(gameBuff,(char*)"enter VR1",0,26,0xFF,0x00);
    } else if (gameStateMaze->vrFlagProximity[1] && gameStateMaze->vrFlags[1] == 0) {
        drawBlock(gameBuff,dim,0x00);
        drawString2x(gameBuff,(char*)"Press A to",0,10,0xFF,0x00);
        drawString2x(gameBuff,(char*)"enter VR2",0,26,0xFF,0x00);
    } else if (gameStateMaze->vrFlagProximity[2] && gameStateMaze->vrFlags[2] == 0) {
        drawBlock(gameBuff,dim,0x00);
        drawString2x(gameBuff,(char*)"Press A to",0,10,0xFF,0x00);
        drawString2x(gameBuff,(char*)"enter VR3",0,26,0xFF,0x00);
    } else if (gameStateMaze->vrFlagProximity[3]) {
        drawBlock(gameBuff,dim,0x00);
        uint8_t count = ((uint8_t)(gameStateMaze->vrFlags[0])) + ((uint8_t)(gameStateMaze->vrFlags[1])) + ((uint8_t)(gameStateMaze->vrFlags[2]));
        if (count == 3) {
            drawString2x(gameBuff,(char*)"Press A to",0,10,0xFF,0x00);
            drawString2x(gameBuff,(char*)"fix crypto!",0,26,0xFF,0x00);
        } else {
            sprintf(fps,"Find %d more",3-count);
            drawString2x(gameBuff,fps,0,10,0xFF,0x00);
            drawString2x(gameBuff,(char*)"VR keys",0,26,0xFF,0x00);
        }
    }

    // Draw keys
    dim.x = 240-32*4;
    dim.y = 240 -32;
    dim.height = 32;
    dim.width = 32;

    drawObject(gameBuff,dim,key,0xFF,gameStateMaze->vrFlags[0] ? 0xE0 : 0x01,0xFF);
    dim.x+=32;
    drawObject(gameBuff,dim,key,0xFF,gameStateMaze->vrFlags[1] ? 0x1C : 0x01,0xFF);
    dim.x+=32;
    drawObject(gameBuff,dim,key,0xFF,gameStateMaze->vrFlags[2] ? 0x03 : 0x01,0xFF);

    // Draw Progress bar
    // int percTimeLeft = SCREEN_WIDTH - gameStateMaze->seconds * SCREEN_WIDTH / gameStateMaze->stageTime;
    // dim.x = 0;
    // dim.y = 0;
    // dim.height = 5;
    // dim.width = percTimeLeft;
    // drawBlock(gameBuff, dim, 0xFF);

    // Draw Win!
    if (gameStateMaze->win)
    {
        drawString2x(gameBuff,(char*)"YOU WIN!",gameBuff->WIDTH/2 - 16*4, gameBuff->HEIGHT/2 - 8,0xFF,0x00);
        gameStateMaze->currentState = 16;
        gameStateMaze->previousState = 4;
    }
    else if (getElapsedSeconds() > gameStateMaze->stageTime)
    {
        // drawString2x(gameBuff,(char*)"GAME OVER!",gameBuff->WIDTH/2 - 16*4, gameBuff->HEIGHT/2 - 8,0xFF,0x00);
    }

    //Draw map
    dim.height = mapHeight;
    dim.width = mapWidth;
    dim.x = SCREEN_WIDTH - mapWidth - 1;
    dim.y = SCREEN_HEIGHT - mapHeight - 1;
    uint8_t traverseMap[mapWidth * mapHeight];
    for (int i = 0; i < mapWidth; i++)
    {
        for (int j = 0; j < mapHeight; j++)
        {
            traverseMap[i + j * mapWidth] = gameStateMaze->traversal[i][j] == 1 ? 0xE0 : 0x00;
        }
    }

    //which box of the map we're in
    int mapX = int(posX);
    int mapY = int(posY);

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (((mapX + i) > 0) && ((mapX + i) < mapWidth) && ((mapY + j) > 0) && ((mapY + j) < mapHeight))
                traverseMap[(mapX + i) + (mapY + j) * mapWidth] = 1;
        }
    }

    drawObject(gameBuff, dim, traverseMap,-1);

    // Draw Timer
    dim.y -= 12;
    dim.x -= 10;
    dim.height = 10;
    dim.width = 32;

    drawBlock(gameBuff, dim, 0x00);
    sprintf(fps,"%3ds",gameStateMaze->seconds);
    drawString(gameBuff,fps,dim.x + 1,dim.y + 1,0x1C,0x00);

}

bool raycasterLoop(GameBuff *gameBuff)
{
    if (gameStateMaze == nullptr) {
        gameStateMaze = (GameStateMaze *)malloc(sizeof(struct GameStateMaze));
        setupGameState();
    }

    switch (gameStateMaze->currentState)
    {
    case 2:
        // Play
        if (gameStateMaze->previousState != gameStateMaze->currentState)
        {
            gameStateMaze->previousState = gameStateMaze->currentState;
            gameStateMaze->frameCounter = 0;
            initTime();
        }

        processInput(gameBuff, gameStateMaze);
        update(gameBuff, gameStateMaze);
        display(gameBuff, gameStateMaze);

        break;
    case 4:
        // Done and Exit
        gameStateMaze->exit = true;
        gameBuff->stage_time4 = gameStateMaze->seconds;
        break;
    case 8: // VR
        if (gameStateMaze->previousState != gameStateMaze->currentState)
        {
            gameStateMaze->previousState = gameStateMaze->currentState;

            if (gameStateMaze->vrFlagProximity[0]) { 
                gameBuff->stage_time1 = gameStateMaze->seconds;
                voxelSetup(gameBuff,0);
            }
            else if (gameStateMaze->vrFlagProximity[1]) {
                gameBuff->stage_time2 = gameStateMaze->seconds;
                voxelSetup(gameBuff,1);
            }
            else if (gameStateMaze->vrFlagProximity[2]) {
                gameBuff->stage_time3 = gameStateMaze->seconds;
                voxelSetup(gameBuff,2);
            }
        }

        if (voxelLoop(gameBuff)) {
            gameStateMaze->currentState = 16;
            gameStateMaze->previousState = 2;
            gameStateMaze->seconds = getElapsedSeconds();
            for (int i = 0; i < 3; i++) { // Set the key that we're at to fetched.
                if (gameStateMaze->vrFlagProximity[i]) gameStateMaze->vrFlags[i] = 1;
            }

            if (gameStateMaze->vrFlagProximity[0]) { 
                gameBuff->stage_time1 = gameStateMaze->seconds - gameBuff->stage_time1;
            }
            else if (gameStateMaze->vrFlagProximity[1]) {
                gameBuff->stage_time2 = gameStateMaze->seconds - gameBuff->stage_time2;
            }
            else if (gameStateMaze->vrFlagProximity[2]) {
                gameBuff->stage_time3 = gameStateMaze->seconds - gameBuff->stage_time3;
            }
        }

        break;
	case 15:
		if (fadeToBlack(gameBuff))
		{
            gameStateMaze->currentState = gameStateMaze->previousState;
            gameStateMaze->previousState = 15;
		}
		break;
	case 16:
		if (fadeToWhite(gameBuff))
		{
            gameStateMaze->currentState = gameStateMaze->previousState;

            if (gameStateMaze->currentState == 2) {
                gameStateMaze->previousState = 2; // Don't flush state
            } else {
                gameStateMaze->previousState = 15;
            }
		}
		break;
    }

    if (gameStateMaze->exit) {
        gameStateMaze->exit = false;
        free(gameStateMaze);
        gameStateMaze = nullptr;
        return true;
    }
    
    return false;
}
