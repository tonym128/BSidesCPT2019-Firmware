#include "fire.h"

static const int FIRE_WIDTH = 120;
static const int FIRE_HEIGHT = 120;
static const int MAX_FIRE_PIXEL = FIRE_WIDTH * FIRE_HEIGHT;
static uint8_t *firePixels = nullptr;
static uint8_t palette[32];

void SetPalette(int i, uint8_t c) {
   if (i < 0 || i > 32) return;
   palette[i] = c;
}

void CreatePalette() {
    for (int i = 0; i < 16; i+=2) {
      // Black -> Red
      SetPalette(i, MakeColor(i, 0, 0));
      SetPalette(i+1, MakeColor(i, 0, 0));
      // Red -> Yellow
      SetPalette(i + 8, MakeColor(7, i, 0));
      SetPalette(i + 9, MakeColor(7, i, 0));
      // Yellow -> White
      SetPalette(i + 16, MakeColor(7, 7, i));
      SetPalette(i + 17, MakeColor(7, 7, i));
    }
  }

void spreadFire(int src)
{
    int random = rand() % 3;
    int dst = src - random + 1 - FIRE_WIDTH;
    int fireValue = firePixels[src] - (random & 1);

    if (fireValue > 32) fireValue = 32;
    if (fireValue < 0) fireValue = 0;
	if (dst > 0 && dst < MAX_FIRE_PIXEL)
		firePixels[dst] = fireValue;
}

void startFire()
{
	if (firePixels == nullptr) {
	    firePixels = (uint8_t *)calloc(FIRE_WIDTH * FIRE_HEIGHT, sizeof(uint8_t));
	}

    int y = (FIRE_HEIGHT - 1) * FIRE_WIDTH;
    for (int x = 0; x < FIRE_WIDTH; x++)
    {
		if (y + x  > 0 && y + x < MAX_FIRE_PIXEL)
			firePixels[y + x] = 22;
    }
}

void doFire()
{
    for (int x = 0; x < FIRE_WIDTH; x++)
    {
        for (int y = 0; y < FIRE_HEIGHT * FIRE_WIDTH; y+= FIRE_HEIGHT)
        {
            spreadFire(y + x);
        }
    }
}

void stopFire() {
	if (firePixels != nullptr) {
		free(firePixels);
		firePixels = nullptr;
	}
}

bool drawFire(GameBuff *gameBuff)
{
    if (firePixels == nullptr)
    {
        startFire();
        CreatePalette();
    }

    doFire();

    int fire = 0;
	for (int x = 0; x < FIRE_WIDTH; x++)
		for (int y = 0; y < FIRE_HEIGHT; y++) {
            gameBuff->consoleBuffer[x + y* gameBuff->HEIGHT] = palette[firePixels[x + y*FIRE_HEIGHT]];
        }

    drawScreenDouble(gameBuff);

    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b)
    {
        stopFire();
        return true;
    }

    return false;
}