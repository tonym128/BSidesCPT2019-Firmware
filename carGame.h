#ifndef _car_game_h
#define _car_game_h
#include "cryptoRunner.h"

static void initCar(GameBuff* gameBuff, CryptoGameState *cryptoGameState, Car* car) {
	car->y = 50;
	car->lane = (uint8_t) rand() % 3;
	car->speed = (uint8_t) rand() % cryptoGameState->maxRoadSpeed / 8;
	car->distance = cryptoGameState->distance;
}

static void startUpCarGame(CryptoGameState *cryptoGameState, GameBuff *gameBuff)
{
	initTime();
	cryptoGameState->playerCar.x = gameBuff->WIDTH / 2 - 64 / 2;
	cryptoGameState->playerCar.y = gameBuff->HEIGHT - 64 - 4;
	cryptoGameState->playerCar.width = 64;
	cryptoGameState->playerCar.height = 64;

	cryptoGameState->maxCars = 3;
	cryptoGameState->stageDistance = 15000;

	for (int i = 0; i < cryptoGameState->maxCars; i++)
	{
		initCar(gameBuff, cryptoGameState, &cryptoGameState->cars[i]);
	}

	cryptoGameState->frameCounter = 0;
	cryptoGameState->distance = 0;
	cryptoGameState->lastdistance = 0;
	cryptoGameState->turnOffset = 0;

	cryptoGameState->turning = false;
	cryptoGameState->turningLeft = false;
	cryptoGameState->turnDepth = 0;

	cryptoGameState->carSpeed = 0;
	cryptoGameState->roadSpeed = 0;
	cryptoGameState->evenLines = true;
	cryptoGameState->collision = false;
}

static bool updateDriveGame(GameBuff *gameBuff, CryptoGameState *cryptoGameState)
{
	cryptoGameState->frameCounter++;

	if (gameBuff->playerKeys.up || gameBuff->playerKeys.a)
	{
		// Accelerating
		cryptoGameState->carSpeed += 1;
	}
	else
	{
		// Organic slow down
		cryptoGameState->carSpeed -= cryptoGameState->frameCounter % 20 == 0 ? 1 : 0;
	}

	if (gameBuff->playerKeys.down || gameBuff->playerKeys.b)
	{
		cryptoGameState->carSpeed -= 1;
	}

	if (cryptoGameState->carSpeed < 0)
		cryptoGameState->carSpeed = 0;
	if (cryptoGameState->carSpeed > cryptoGameState->maxRoadSpeed)
		cryptoGameState->carSpeed = cryptoGameState->maxRoadSpeed;

	if (gameBuff->playerKeys.left)
	{
		cryptoGameState->playerCar.x -= 1;
	}
	if (gameBuff->playerKeys.right)
	{
		cryptoGameState->playerCar.x += 1;
	}

	// Side lines
	if (cryptoGameState->distance - cryptoGameState->lastdistance >= cryptoGameState->udpateSpeed)
	{
		cryptoGameState->evenLines = !cryptoGameState->evenLines;
		cryptoGameState->lastdistance = cryptoGameState->distance;
	}

	// Collision
	if (cryptoGameState->collision)
	{
		cryptoGameState->carSpeed = 0;
		cryptoGameState->collision = false;
	}

	// Randomly Start Turning
	if (!cryptoGameState->turning)
	{
		cryptoGameState->turning = rand() % 100 == 0;
		if (cryptoGameState->turning)
		{
			cryptoGameState->turningLeft = !cryptoGameState->turningLeft;
			cryptoGameState->turnDepth = rand() % 100;
		}
	}

	// Setup Road speed and distance travelled.
	cryptoGameState->roadSpeed = cryptoGameState->maxRoadSpeed + 1 - cryptoGameState->carSpeed;
	cryptoGameState->distance += cryptoGameState->carSpeed / 8;

	// Do turning logic
	if (cryptoGameState->turning)
	{
		if (cryptoGameState->distance - cryptoGameState->lastdistance >= cryptoGameState->udpateSpeed)
		{
			// Pull Car
			cryptoGameState->playerCar.x += cryptoGameState->turningLeft ? +1 : -1;

			// Going up
			if (cryptoGameState->turnDepth > 0)
			{
				cryptoGameState->turnOffset += cryptoGameState->turningLeft ? -1 : +1;
				cryptoGameState->turnDepth -= 1;
				cryptoGameState->backgroundx -= 1;
			}

			// Going down / finished
			if (cryptoGameState->turnDepth == 0)
			{
				if (cryptoGameState->turnOffset == 0)
				{
					cryptoGameState->turning = false;
				}
				else
				{
					cryptoGameState->turnOffset += cryptoGameState->turningLeft ? +1 : -1;
					cryptoGameState->backgroundx += cryptoGameState->turningLeft ? +1 : -1;
				}
			}

			if (cryptoGameState->turnOffset < -80)
			{
				cryptoGameState->turnOffset = -80;
			}
			if (cryptoGameState->turnOffset > 80)
			{
				cryptoGameState->turnOffset = 80;
			}
		}
	}

	if (cryptoGameState->playerCar.x < 0)
	{
		cryptoGameState->playerCar.x = 0;
	}
	else if (cryptoGameState->playerCar.x + cryptoGameState->playerCar.width > gameBuff->WIDTH)
	{
		cryptoGameState->playerCar.x = gameBuff->WIDTH - cryptoGameState->playerCar.width;
	}

	if (cryptoGameState->distance - cryptoGameState->lastdistance >= cryptoGameState->udpateSpeed)
	{
		cryptoGameState->evenLines = !cryptoGameState->evenLines;
		cryptoGameState->lastdistance = cryptoGameState->distance;
	}

	// initialise / update cars
	for (int i = 0; i < cryptoGameState->maxCars; i++)
	{
		if (cryptoGameState->cars[i].y == 0 || cryptoGameState->cars[i].y == gameBuff->HEIGHT)
		{
			initCar(gameBuff, cryptoGameState, &cryptoGameState->cars[i]);
		}
		else
		{
			cryptoGameState->cars[i].distance += cryptoGameState->cars[i].speed;

			if (cryptoGameState->cars[i].distance - cryptoGameState->distance >= cryptoGameState->udpateSpeed)
			{
				cryptoGameState->cars[i].distance = cryptoGameState->distance;
				cryptoGameState->cars[i].y -= 1;
			}
			if (cryptoGameState->distance - cryptoGameState->cars[i].distance >= cryptoGameState->udpateSpeed)
			{
				cryptoGameState->cars[i].distance = cryptoGameState->distance;
				cryptoGameState->cars[i].y += 1;
			}
		}
	}
	
	// Time Check Driving
	cryptoGameState->drive_time = getElapsedSeconds();

	// If we have hit the required distance
	if (cryptoGameState->stageDistance - cryptoGameState->distance < 0)
	{
		return false;
	}

	return true;
}

static bool displayDriveGame(GameBuff *gameBuff, CryptoGameState *cryptoGameState)
{
	displayClear(gameBuff, 0x00);

	int width = gameBuff->WIDTH / 30;
	int startY = gameBuff->HEIGHT / 3;

	int height = 1;
	int j = 0;
	int y = startY;
	Dimensions dim;
	dim.x = 0;
	dim.y = 0;
	dim.height = 80;
	dim.width = 240;

	// Draw Background
	Dimensions image;
	image.x = cryptoGameState->backgroundx;
	image.y = 0;
	image.height = 80;
	image.width = 542;

	drawObjectScrollLoop(gameBuff, dim, image, background_image, -1);

	dim.width = gameBuff->WIDTH;
	while (y < gameBuff->HEIGHT)
	{
		// Draw grass side of road
		dim.x = 0;
		dim.y = y;
		dim.height = height;
		if (cryptoGameState->evenLines == 0)
			drawBlock(gameBuff, dim, j % 2 ? 0x1C : 0x10);
		else
			drawBlock(gameBuff, dim, j % 2 ? 0x10 : 0x1C);

		y += height + 1;
		j += 1;

		if (j % 5 == 0)
			height += 1;
	}

	// Draw the road
	y = gameBuff->HEIGHT / 3;

	int turnOffset = cryptoGameState->turnOffset;
	Dimensions taxis[5];

	while (y < gameBuff->HEIGHT)
	{
		// Draw Off Road
		dim.x = (gameBuff->WIDTH - width - y * 5 / 6) / 2 - 2 + turnOffset;
		dim.y = y;
		dim.height = 0;
		dim.width = width + y * 5 / 6 + 4;
		drawBlock(gameBuff, dim, y % 2 ? 0xFF : 0x00);

		// Draw Road
		dim.x += 2;
		dim.width -= 4;
		drawBlock(gameBuff, dim, true);

		for (int i = 0; i < cryptoGameState->maxCars; i++)
		{
			if (cryptoGameState->cars[i].y == y)
			{
				taxis[i].height = 64;
				taxis[i].width = 64;
				taxis[i].y = y;
				taxis[i].x = (dim.width  / 3) * cryptoGameState->cars[i].lane + dim.x;
			}
		}

		// Progress Loop
		y += 1;

		// Change the offset every second line (give a nice flow to the bottom)
		if (y % 2 == 0 && turnOffset != 0)
		{
			turnOffset = (int)(turnOffset * 0.9);
		}
	}
	// Draw Car
	if (gameBuff->playerKeys.left)
	{
		drawObject(gameBuff, cryptoGameState->playerCar, car_left_image, 0x00, cryptoGameState->carMask);
	}
	else if (gameBuff->playerKeys.right)
	{
		drawObject(gameBuff, cryptoGameState->playerCar, car_right_image, 0x00, cryptoGameState->carMask);
	}
	else
	{
		drawObject(gameBuff, cryptoGameState->playerCar, car_middle_image, 0x00, cryptoGameState->carMask);
	}

	Dimensions dimCar;
	dimCar.height = 64;
	dimCar.width = 64;
	double zoom = 0;

	for (int i = 0; i < cryptoGameState->maxCars; i++)
	{
		if (taxis[i].y > 0) {
			zoom = 4 - (taxis[i].y - (gameBuff->HEIGHT / 3)) / 10.0;
			if (zoom < 1)
				zoom = 1;
			else if (zoom > 4)
				zoom = 4;
			dimCar.x = taxis[i].x;
			dimCar.y = taxis[i].y;

			// And the car is on screen
			if (dimCar.y > 40 && dimCar.y < gameBuff->HEIGHT)
			{
				dimCar.x -= (int)((zoom - 1) * 10.0);
				dimCar.y -= (int)((zoom - 1) * 10.0);
				if (!cryptoGameState->turning) {
					rotateObjectAlpha(gameBuff, dimCar, 0, zoom, taxi_middle_image, 0x03, cryptoGameState->taxiMask);
				} else if (cryptoGameState->turningLeft) {
					rotateObjectAlpha(gameBuff, dimCar, 0, zoom, taxi_left_image, 0x03, cryptoGameState->taxiMask);
				} else {
					rotateObjectAlpha(gameBuff, dimCar, 0, zoom, taxi_right_image, 0x03, cryptoGameState->taxiMask);
				}
			}

			// // Taxi 1 debug
			// char text[30];
			// sprintf(text,"X: %3d, Y: %3d",taxis[0].x,taxis[0].y);
			// drawString(gameBuff,text,0,0,0xFF,0x00);
			// sprintf(text,"Lane : %1d Zoom : %1.4f",cryptoGameState->cars[0].lane, zoom);
			// drawString(gameBuff,text,0,8,0xFF,0x00);
			// if (cryptoGameState->playerCar.x > 0) drawBlock(gameBuff,cryptoGameState->playerCar,0xFF,1);
			// if (taxis[0].x > 0) drawBlock(gameBuff,taxis[0],0xFF,1);
			// if (taxis[1].x > 0) drawBlock(gameBuff,taxis[1],0xFF,1);
			// if (taxis[2].x > 0) drawBlock(gameBuff,taxis[2],0xFF,1);

			// Collision Detection
			if (rectCollisionCheck(dimCar, cryptoGameState->playerCar)) {
				if (maskCollisionBitCheck(taxis[i], cryptoGameState->playerCar, cryptoGameState->taxiMask, cryptoGameState->carMask))
				{
					cryptoGameState->collision = true;
				}
			}
		}
	}

	// Draw Status Bar
	dim.x = 0;
	dim.y = 220;
	dim.width = 240;
	dim.height = 20;
	drawBlock(gameBuff, dim, 0x00);
	char status[20];
	sprintf(status, "%3dkm/h %2lukm", cryptoGameState->carSpeed, (cryptoGameState->stageDistance - cryptoGameState->distance) / 1000);
	drawString2x(gameBuff, status, 0, 222, 0x1F, 0x00);

	sprintf(status, "%3lus", cryptoGameState->drive_time);
	drawString(gameBuff, status, 200, 222, 0x1F, 0x00);

    // Scrolling 3 , 2 , 1, !, rough rough rough.
    // if (cryptoGameState->startup == nullptr) {
    //     cryptoGameState->startup = (uint8_t *)calloc(1024,sizeof(uint8_t));
    // }
    // drawFont(cryptoGameState->startup,32,32,32*32,'3',0xFF,0x00,0,0);
    // dim.x = gameBuff->WIDTH / 2 - 16;
    // dim.y = gameBuff->HEIGHT / 2 - 16;
    // dim.width = 32;
    // dim.height = 32;
    // rotateObjectAlpha(gameBuff, dim, 0, cryptoGameState->startupMon, cryptoGameState->startup, 0x00);
    // if (cryptoGameState->startupUp) {
    //     cryptoGameState->startupMon += 0.1f;
    // } else {
    //     cryptoGameState->startupMon -= 0.1f;
    // }

    // if (cryptoGameState->startupMon > 9 || cryptoGameState->startupMon < 1) cryptoGameState->startupUp = !cryptoGameState->startupUp;

	return false;
}

#endif