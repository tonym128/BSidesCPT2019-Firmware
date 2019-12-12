#ifndef ASTEROIDS_H
#define ASTEROIDS_H 1

#ifdef ESP32
#include <HTTPClient.h>
#endif

#include "game.hpp"

static const int FIRECOUNT = 5;
static const int FIREPACING = 150;
static const FIXPOINT FIREPOWER = INT_TO_FIXP(2);
static const int ASTEROIDS =  50;
static const int SCORETIMEMULTTIMEOUT = 2000;

struct Fire {
	Dimensions dim = { 0,0,0,0 };
	FIXPOINT fixX = FIXP_1,fixY = FIXP_1, movX = FIXP_1, movY = FIXP_1;
	int life = 0;
};

struct AsteroidsPlayer {
	Dimensions dim = { 0,0,0,0 };
  FIXPOINT thrust,direction,size;
  FIXPOINT fixX,fixY, movX, movY;
  double rotation;
  bool collision = 0;
  bool inPlay = true;
  Fire fire[FIRECOUNT];
  unsigned long firetimeout = 0;
};

struct Asteroid {
  bool active = 0;
  Dimensions dim;
  FIXPOINT speed,direction,size;
  FIXPOINT fixX,fixY;
  double rotateAmount;
  double rotation;
};

struct AsteroidsGameState {
	char **scrollerText; //[9][17];
  uint8_t *rotAst; //[400];
  uint8_t *rotShip; //[100];

  int hiScore = 0;
  int score = 0;
  int level = 1;
  int scene = 0;
  int lastscene = -99;
  int frameCounter = 0;
  bool paused = false;
  bool submitted = false;
  unsigned long pausedDebounce = 0;

  // Scoring mechanism variables
  int lastScore = 0;
  int multiplier = 1;
  int maxMultiplier = 10;
  unsigned long scoreTimeMultiplier = 0;

  // Game mechanics
  AsteroidsPlayer player1;
  Asteroid *asteroids;

  bool win = false;
  bool running = true;
  bool restart = false;
};

bool asteroidsLoop(GameBuff*);
#endif
