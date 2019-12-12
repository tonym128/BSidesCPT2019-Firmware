#ifndef _CRYPTO_RUNNER_H
#define _CRYPTO_RUNNER_H 1
#include "game.hpp"
#include "scroller.hpp"

// Game State Management
struct Car {
	uint8_t speed = 0;
	uint8_t y = 0;
	uint8_t lane = 0;
	bool visible = 0;
	int distance = 0;
};

struct CryptoGameState {
	int Scene = 0;
	int LastScene = -1;
	int selection = 0;
	unsigned long frameCounter = 0;

	// All
	bool paused = false;

	// Car
	Dimensions playerCar;
	bool carMask[64*64];
	bool taxiMask[64*64];

	uint8_t MaxOffRoadSpeed = 25;
	uint8_t maxRoadSpeed = 120;
	uint8_t crashSpeed = 0;
	uint8_t udpateSpeed = 30;

	long distance;
	long stageDistance;

	bool turning;
	bool turningLeft;
	int roadSpeed = 0;
	int turnDepth = 0;
	int turnOffset = 0;

	long lastdistance;
	int carSpeed = 0;
	bool collision = false;
	bool evenLines;
	long backgroundx = 0;
	int maxCars = 5;
	Car *cars = nullptr;

	unsigned int drive_stage = 0;
	unsigned int drive_distance = 0;
	unsigned int drive_distanceCompleted = 0;

	unsigned long drive_time = 0;
	unsigned long office_time = 0;
	unsigned long vr1_time = 0;
	unsigned long vr2_time = 0;
	unsigned long vr3_time = 0;
	unsigned long total_time = 0;

	unsigned long hi_drive_time = 0;
	unsigned long hi_office_time = 0;
	unsigned long hi_vr1_time = 0;
	unsigned long hi_vr2_time = 0;
	unsigned long hi_vr3_time = 0;
	unsigned long hi_total_time = 0;

	unsigned long remote_hi_drive_time = 0;
	unsigned long remote_hi_office_time = 0;
	unsigned long remote_hi_vr1_time = 0;
	unsigned long remote_hi_vr2_time = 0;
	unsigned long remote_hi_vr3_time = 0;
	unsigned long remote_hi_total_time = 0;

	uint8_t *startup = nullptr;
	double startupMon = 0;
	bool startupUp = false;
};

static void setupCryptoGameState(CryptoGameState *gameState) {

	gameState->Scene = 0;

	// TODO: DEBUG CODE
	// gameState->Scene = 11;
	// END DEBUG CODE

	gameState->LastScene = -1;
	gameState->paused = false;
	gameState->drive_stage = 0;
	gameState->drive_distance = 0;
	gameState->drive_distanceCompleted = 0;

	gameState->udpateSpeed = 200;

	gameState->cars = (Car *)calloc(5, sizeof(Car));
	gameState->maxRoadSpeed = 120;
	gameState->crashSpeed = 0;
	gameState->udpateSpeed = 30;

	gameState->drive_time = 0;
	gameState->office_time = 0;
	gameState->vr1_time = 0;
	gameState->vr2_time = 0;
	gameState->vr3_time = 0;
	gameState->total_time = 0;

	gameState->hi_drive_time = 999;
	gameState->hi_office_time = 999;
	gameState->hi_vr1_time = 999;
	gameState->hi_vr2_time = 999;
	gameState->hi_vr3_time = 999;
	gameState->hi_total_time = 999;

	// gameState->startup = nullptr;
	// gameState->startupMon = 5.0f;
	// gameState->startupUp = false;
}

static void freeCryptoGameState(CryptoGameState *cryptoGameState) {
	if (cryptoGameState->startup != nullptr) {
		free(cryptoGameState->startup);
	}
	if (cryptoGameState->cars != nullptr) 
	{
		free(cryptoGameState->cars);
	}

	free(cryptoGameState);
}
// End - Game State Management

bool cryptoRunnerGameLoop(GameBuff *gameBuff);

#endif