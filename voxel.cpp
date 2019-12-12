//#define INTERLACE
#define DETAIL_LEVEL FLOAT_TO_FIXP(0.1) //Smaller = higher detail

// https://github.com/s-macke/VoxelSpace
#include "voxel.hpp"

struct VoxelConf
{
    FIXPOINT fx, fy, fangle, fmove, fturn, fdeltaMod, fdistance;
    FIXPOINT fStartX, fStartY;
    int height, horizon, shift;
    FIXPOINT mapScaleFactor;

    int mapwidthperiod, mapheightperiod;
    FIXPOINT fKeyOffsetX;
    FIXPOINT fKeyOffsetY;
    FIXPOINT fKeyOffset;

    bool exitVoxel = false;
    int colour = 0;
    bool firstRun = true;
    Dimensions dimTimer;
    int startTimer;
};

VoxelConf *p = nullptr;

#ifdef INTERLACE
int frame = 0;
#endif

int ccmapOffset = -1;

int8_t cmap;
uint8_t ccolor;
int cmapOffset = -1;
int cheight;
int cheightonscreen;

bool voxelOverKey(GameBuff *gameBuff) {
    return (gameBuff->consoleBuffer[gameBuff->WIDTH * (gameBuff->HEIGHT - 50) + gameBuff->WIDTH/2] == 0xFF);
}

void render(GameBuff *gameBuff, VoxelConf *p)
{
    int heightonscreen = 0;
#ifdef INTERLACE
    frame += 1;
    if (frame == 2)
        frame = 0;
#endif

    FIXPOINT fsinang = FIXPOINT_SIN(p->fangle);
    FIXPOINT fcosang = FIXPOINT_COS(p->fangle);

    uint8_t hiddeny[gameBuff->WIDTH];
    for (uint8_t i = 0; i < gameBuff->WIDTH; i += 1)
    {
        hiddeny[i] = gameBuff->HEIGHT;
#ifdef INTERLACE
        if (i % 2 == frame)
            drawVertLine2(gameBuff, i, 0, gameBuff->HEIGHT, 0);
#endif
    }

    FIXPOINT fdeltaz = FIXP_1;
    // Draw from front to back
    for (FIXPOINT fz = FIXP_1; fz < p->fdistance; fz += fdeltaz)
    {
        // 90 degree field of view
        FIXPOINT fsinfz = FIXP_MULT(fsinang, fz);
        FIXPOINT fcosfz = FIXP_MULT(fcosang, fz);

        FIXPOINT fplx = FIXP_MULT(-fcosang, fz) - fsinfz;
        FIXPOINT fply = fsinfz - fcosfz;
        FIXPOINT fprx = fcosfz - fsinfz;
        FIXPOINT fpry = FIXP_MULT(-fsinang, fz) - fcosfz;
        FIXPOINT fdx = (fprx - fplx) / gameBuff->WIDTH;
        FIXPOINT fdy = (fpry - fply) / gameBuff->WIDTH;
        FIXPOINT finvz = FIXP_DIV(FIXP_1, fz) * 240;

        fplx += p->fx;
        fply += p->fy;

#ifdef INTERLACE
        for (int i = frame; i < gameBuff->WIDTH; i += 2)
#else
        for (int i = 0; i < gameBuff->WIDTH; i += 1)
#endif
        {
            if (hiddeny[i] >= 0)
            {
                int mapoffset = ((FIXP_INT_PART(FIXP_DIV(fply, p->mapScaleFactor)) & p->mapwidthperiod) << p->shift) + (FIXP_INT_PART(FIXP_DIV(fplx, p->mapScaleFactor)) & p->mapheightperiod);
                if (cmapOffset == mapoffset)
                {
                    if (cheight != p->height)
                    {
                        cheightonscreen = heightonscreen = (FIXP_TO_INT((p->height - cmap) * finvz) + p->horizon);
                        cheight = p->height;
                    }
                } else
                {
                    switch (p->colour)
                    {
                    case 0:
                        // Red
                        cmap = ((map_colour[mapoffset] & 0xE0) >> 3) + (map_colour[mapoffset] & 0x03);
                        break;
                    case 1:
                        // Green
                        cmap = ((map_colour[mapoffset] & 0x1C)) + (map_colour[mapoffset] & 0x03);
                        break;
                    case 2:
                        // Blue
                        cmap = ((map_colour[mapoffset] & 0x03)) + (map_colour[mapoffset] & 0x1C);
                        break;
                    case 3:
                        cmap = map_colour[mapoffset] >> 3;
                        break;
                    }

                    cheight = p->height;
                    cheightonscreen = (FIXP_TO_INT((p->height - cmap) * finvz) + p->horizon);
                }

                if (cheightonscreen < hiddeny[i])
                {
                    if (mapoffset == FIXP_TO_INT(p->fKeyOffset)) {
                        ccolor = 0xFF;
                    } else {
                        switch (p->colour)
                        {
                        case 0:
                            // Red
                            ccolor = map_colour[mapoffset] & 0xE0;
                            break;
                        case 1:
                            // Green
                            ccolor = map_colour[mapoffset] & 0x1C;
                            break;
                        case 2:
                            // Blue
                            ccolor = map_colour[mapoffset] & 0x03;
                            break;
                        case 3:
                            ccolor = map_colour[mapoffset];
                            break;
                        }
                    }
                    drawVertLine2(gameBuff, i, cheightonscreen, hiddeny[i], ccolor);
                    hiddeny[i] = cheightonscreen;
                }

#ifdef INTERLACE
                fplx += fdx * 2;
                fply += fdy * 2;
#else
                fplx += fdx;
                fply += fdy;
#endif
            }
        }

        fdeltaz += p->fdeltaMod;
    }

    Dimensions dim;
    dim.x = 0;
    dim.y = gameBuff->HEIGHT - 32;
    dim.width = gameBuff->WIDTH;
    dim.height = 32;
    
    int currentMapOffset = ((FIXP_INT_PART(FIXP_DIV(p->fy, p->mapScaleFactor)) & p->mapwidthperiod) << p->shift) + (FIXP_INT_PART(FIXP_DIV(p->fx, p->mapScaleFactor)) & p->mapheightperiod);
    int startMapOffset = ((FIXP_INT_PART(FIXP_DIV(p->fStartY, p->mapScaleFactor)) & p->mapwidthperiod) << p->shift) + (FIXP_INT_PART(FIXP_DIV(p->fStartX, p->mapScaleFactor)) & p->mapheightperiod);

    if (p->colour < 3) {
        // If on the start block, show the instruction
        if (startMapOffset == currentMapOffset) {
            drawBlock(gameBuff,dim,0x00);
            drawString2x(gameBuff,(char*)"Find the white",0,dim.y,0xFF,0x00);
            drawString2x(gameBuff,(char*)"rabbit!",0,dim.y+16,0xFF,0x00);
        } 
        
        // Draw Aiming Dot
        gameBuff->consoleBuffer[gameBuff->WIDTH * (gameBuff->HEIGHT - 50) + gameBuff->WIDTH/2 + 1] = 0xFF;
        gameBuff->consoleBuffer[gameBuff->WIDTH * (gameBuff->HEIGHT - 50) + gameBuff->WIDTH/2 - 1] = 0xFF;
        gameBuff->consoleBuffer[gameBuff->WIDTH * (gameBuff->HEIGHT - 50) + gameBuff->WIDTH/2 + gameBuff->WIDTH] = 0xFF;
        gameBuff->consoleBuffer[gameBuff->WIDTH * (gameBuff->HEIGHT - 50) + gameBuff->WIDTH/2 - gameBuff->WIDTH] = 0xFF;
        
        // Check Screen for white dot
        if (voxelOverKey(gameBuff)) {
            drawBlock(gameBuff,dim,0x00);
            drawString2x(gameBuff,(char*)"Press A to",0,dim.y,0xFF,0x00);
            drawString2x(gameBuff,(char*)"pick up key",0,dim.y+16,0xFF,0x00);
        }

        // Draw the Timer
        switch (p->colour)
        {
        case 0:
            // Red
            ccolor = 0xE0;
            break;
        case 1:
            // Green
            ccolor = 0x1C;
            break;
        case 2:
            // Blue
            ccolor = 0x03;
            break;
        }

        char timerString[5];
        sprintf(timerString,"%3ds",getElapsedSeconds()-p->startTimer);

        drawBlock(gameBuff,p->dimTimer,0x00);
        drawString(gameBuff,timerString,p->dimTimer.x + 1,p->dimTimer.y + 1,0x1C,0x00);
    }
}

void voxelInput(GameBuff *gameBuff, VoxelConf *p)
{
    // Move by default code
    //p->fx -= FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove);
    //p->fy -= FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove);

    // Input code
    if (gameBuff->playerKeys.up)
    {
        p->fx -= FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove * 3);
        p->fy -= FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove * 3);
    }
    if (gameBuff->playerKeys.down)
    {
        p->fx += FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove);
        p->fy += FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove);
    }
    if (gameBuff->playerKeys.left)
    {
        p->fangle += p->fturn;
    }
    if (gameBuff->playerKeys.right)
    {
        p->fangle -= p->fturn;
    }

    // if (gameBuff->playerKeys.select)
    // {
    //     p->height += 1;
    // }
    // if (gameBuff->playerKeys.start)
    // {
    //     p->height -= 1;
    // }

    if (gameBuff->playerKeys.a || gameBuff->playerKeys.b)
    {
        if (voxelOverKey(gameBuff)) {
            p->exitVoxel = true;
        }
    }
    
    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b)
    {
        p->exitVoxel = true;
    }

    if (p->colour < 0) p->colour = 3;
    else if (p->colour > 3) p->colour = 0;

    // // Collision detection. Don't fly below the surface.
    // int mapoffset = (((int)(FIXP_INT_PART(p->fy / p->mapScaleFactor)) & (map_width - 1)) << p->shift) + (((int)FIXP_INT_PART(p->fx / p->mapScaleFactor)) & (map_height - 1));
    // switch (colour)
    // {
    // case 0: cmap = ((map_colour[mapoffset] & 0xE0) >> 3) + (map_colour[mapoffset] & 0x03); break; // Red
    // case 1: cmap = ((map_colour[mapoffset] & 0x1C)) + (map_colour[mapoffset] & 0x03); break; // Green
    // case 2: cmap = ((map_colour[mapoffset] & 0x03)) + (map_colour[mapoffset] & 0x1C); break; // Blue
    // case 3: cmap = map_colour[mapoffset] >> 3; break; // All colours
    // };

    // if ((cmap) > p->height)
    //     p->height = cmap;
}

void voxelSetup(GameBuff *gameBuff, uint8_t stage_colour) {
    if (p == nullptr) { 
        p = new VoxelConf(); 
        voxelInit(gameBuff);
    }

    p->colour = stage_colour;
}

bool voxelLoop(GameBuff *gameBuff)
{
    if (p == nullptr) {
        p = new VoxelConf();
        p->firstRun = true;
    }

    if (p->firstRun)
    {
        p->firstRun = false;
        voxelInit(gameBuff);
    }

    voxelInput(gameBuff, p);
#ifndef INTERLACE
    displayClear(gameBuff, 0x00);
#endif
    render(gameBuff, p);

    if (p->exitVoxel)
    {
        free(p);
        p = nullptr;
        return true;
    }

    return false; // Not done
}

void voxelInit(GameBuff *gameBuff)
{
    p->mapwidthperiod = map_width - 1;
    p->mapheightperiod = map_height - 1;

    p->fdistance = INT_TO_FIXP(800);
    p->fStartX = INT_TO_FIXP(90);
    p->fStartY = INT_TO_FIXP(90);
    p->fx = p->fStartX;
    p->fy = p->fStartY;
    p->fangle = FLOAT_TO_FIXP(-0.6);

    p->fdeltaMod = DETAIL_LEVEL;
    p->fmove = FLOAT_TO_FIXP(2.);
    p->fturn = FLOAT_TO_FIXP(0.1);

    p->height = 60;
    p->horizon = 15;
    p->shift = 7;
    p->mapScaleFactor = INT_TO_FIXP(8);
    
    p->fKeyOffsetY = INT_TO_FIXP(rand()%128);
    p->fKeyOffsetX = INT_TO_FIXP(rand()%128);
    p->fKeyOffset =  p->fKeyOffsetY * map_width + p->fKeyOffsetX; 

    p->exitVoxel = false;

    p->startTimer = getElapsedSeconds();
    p->dimTimer.x = gameBuff->WIDTH-34;
    p->dimTimer.y = gameBuff->HEIGHT-12;
    p->dimTimer.height = 10;
    p->dimTimer.width = 32;
}
