#include "myGame.hpp"

bool displayMenu(GameBuff *gameBuff)
{
	int i = 0;
	int x = 32;

	gameBuff->maxGameMode = 0;
	drawString2x(gameBuff, (char *)"BSides 2019 ", x, i += 16, 0xFF, 0);
	drawString2x(gameBuff, (char *)" Cape Town  ", x, i += 16, 0xFF, 0);
	drawString2x(gameBuff, (char *)"----------- ", x, i += 16, 0xFF, 0);
	drawString2x(gameBuff, (char *)"0) Schedule ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"1) Asteroids", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"2) GooseHunt", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"3) Crypto!  ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"4) GamePad  ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"5) IFeelZ   ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"6) Matrix   ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"7) Cheevos  ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"8) About    ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	drawString2x(gameBuff, (char *)"9) Setup    ", x, i += 16, gameBuff->gameMode == gameBuff->maxGameMode++ ? 0x1C : 0xFF, 0);
	gameBuff->maxGameMode--;
	return true;
}

bool updateMenus(GameBuff *gameBuff)
{
	if (gameBuff->playerKeys.debouncedInput)
	{
		if (gameBuff->playerKeys.up)
		{
			gameBuff->gameMode -= 1;
		}
		if (gameBuff->playerKeys.down)
		{
			gameBuff->gameMode += 1;
		}

		if (gameBuff->playerKeys.a || gameBuff->playerKeys.b || gameBuff->playerKeys.start || gameBuff->playerKeys.select)
		{
			gameBuff->enter = true;
		}
	}

	// Loop around
	if (gameBuff->gameMode == -1)
	{
		gameBuff->gameMode = gameBuff->maxGameMode;
	}
	else if (gameBuff->gameMode == gameBuff->maxGameMode + 1)
	{
		gameBuff->gameMode = 0;
	}

	return true;
}

bool myGameLoop(GameBuff *gameBuff)
{
	if (gameBuff->enter)
	{
		if (gameBuff->fireRunning)
		{
			gameBuff->fireRunning = false;
			stopFire();
		}

		switch (gameBuff->gameMode)
		{
		case 0:
			if (displaySchedule(gameBuff))
			{
				gameBuff->enter = false;
			}
			break;
		case 1:
			if (gameBuff->firstRun)
			{
				gameBuff->firstRun = false;
			}
			else if (asteroidsLoop(gameBuff))
			{
				gameBuff->enter = false;
				gameBuff->firstRun = true;
#ifdef ESP32
				ESP.restart();
#endif
			}
			break;
		case 2:
			if (gameBuff->firstRun)
			{
				gameBuff->firstRun = false;

				// Boot switch logic
				if (!gameBuff->badgeState->customBoot)
				{
					gameBuff->badgeState->customBoot = true;
					gameBuff->badgeState->bootMenuItem = 2;
					saveBadgeSettings(gameBuff->badgeState);
#ifdef ESP32
					ESP.restart();
#else
					exit(0);
#endif
				}
			}
			else if (btLoop(gameBuff))
			{
				gameBuff->enter = false;
				gameBuff->firstRun = true;
				gameBuff->badgeState->customBoot = false;
				gameBuff->badgeState->bootMenuItem = 0;
				saveBadgeSettings(gameBuff->badgeState);
#ifdef ESP32
				ESP.restart();
#else
				exit(0);
#endif
			}
			break;
		case 3:
			if (gameBuff->firstRun)
			{
				gameBuff->firstRun = false;
			}
			else if (cryptoRunnerGameLoop(gameBuff))
			{
				gameBuff->enter = false;
				gameBuff->firstRun = true;
#ifdef ESP32
				ESP.restart();
#endif
			}
			break;
		case 4:
			if (gameBuff->firstRun)
			{
				gameBuff->firstRun = false;

				// Boot switch logic
				if (!gameBuff->badgeState->customBoot)
				{
					gameBuff->badgeState->customBoot = true;
					gameBuff->badgeState->bootMenuItem = 4;
					saveBadgeSettings(gameBuff->badgeState);
#ifdef ESP32
					ESP.restart();
#else
					exit(0);
#endif
				}
			}
			else if (bleKeyboardLoop(gameBuff))
			{
				gameBuff->enter = false;
				gameBuff->firstRun = true;
				gameBuff->badgeState->customBoot = false;
				gameBuff->badgeState->bootMenuItem = 0;
				saveBadgeSettings(gameBuff->badgeState);
#ifdef ESP32
				ESP.restart();
#else
				exit(0);
#endif
			}
			break;
		case 5:
			if (feelingsLoop(gameBuff))
			{
				gameBuff->enter = false;
			}
			break;
		case 6:
			if (matrixLoop(gameBuff))
			{
				gameBuff->enter = false;
			}
			break;
		case 7:
			if (gameBuff->firstRun)
			{
				gameBuff->firstRun = false;
				displayClear(gameBuff, 0x00);
			}
			else if (displayAchievementsLoop(gameBuff))
			{
				gameBuff->enter = false;
				gameBuff->firstRun = true;
			}

			break;
		case 8:
			if (displayAbout(gameBuff))
			{
				gameBuff->enter = false;
			}
			break;
		case 9:
			if (setupScreenLoop(gameBuff))
			{
				gameBuff->enter = false;
			}
			break;
		}
		return false;
	}
	else
	{
		if (!gameBuff->fireRunning)
			gameBuff->fireRunning = true;
		updateMenus(gameBuff);
		drawFire(gameBuff);
		displayMenu(gameBuff);
		return false;
	}
}
