#include "rotozoomer.hpp"

int FrameCounter = 0;
Dimensions imageDim;
bool direction = false;
const int zoomFactor = 120;
double PI_180 = PI / 180;
bool firstFrame = true;

void rotoInit()
{
	imageDim.width = defcon_width;
	imageDim.height = defcon_height;

	imageDim.y = 0;
	imageDim.x = 0;
}

bool rotoLoop(GameBuff *gameBuff)
{
	if (firstFrame) {
		firstFrame = false;
		rotoInit();
	}

	double zoom = (FrameCounter % zoomFactor) / (double)zoomFactor;
	if (FrameCounter % zoomFactor == 0)
		direction = !direction;

	if (direction)
		zoom = (zoomFactor / double(zoomFactor)) - zoom;

	rotateObject(gameBuff, imageDim, FrameCounter * PI_180, zoom, rotoZoomer_image, 0xFF);
	drawScreenDouble(gameBuff);
	FrameCounter++;
	return ((gameBuff->playerKeys.a && gameBuff->playerKeys.b) || (FrameCounter > 300));
}
