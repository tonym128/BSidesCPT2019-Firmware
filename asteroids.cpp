#include "asteroids.hpp"
#include "asteroids_images.h"
#include "secrets.h"

AsteroidsGameState *asteroidsGameState;

unsigned long frameTime_current = 0;
unsigned long frameTime_last = 0;
unsigned long frameTimeDiff = 0;

void asteroidsSaveSettings(GameBuff *gameBuff, AsteroidsGameState *asteroidsGameState)
{
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_number(root_object, "score", asteroidsGameState->hiScore);
	json_object_set_string(root_object, "name", gameBuff->badgeState->name);
	json_object_set_string(root_object, "mac", gameBuff->badgeState->bt_addr);
	json_object_set_boolean(root_object, "submitted", asteroidsGameState->submitted);
	char *nameString = json_serialize_to_string_pretty(root_value);

	gameSaveFile((char*)"Asteroids.json", nameString);

	json_free_serialized_string(nameString);
	json_value_free(root_value);
}

void allocAsteroidMem(AsteroidsGameState *state) {
	if (state->scrollerText == nullptr) {
		state->scrollerText = (char**) calloc(9, sizeof(char*));
		for (int i = 0; i < 9; i++ )
		{
			state->scrollerText[i] = (char*) calloc(17, sizeof(char));
		}	
	}

	if (state->rotAst == nullptr) state->rotAst = (uint8_t *) calloc(400, sizeof(uint8_t*));
	if (state->rotShip == nullptr) state->rotShip = (uint8_t *) calloc(100, sizeof(uint8_t*));
	if (state->asteroids == nullptr) state->asteroids = (Asteroid *)calloc(ASTEROIDS,sizeof(Asteroid));
}

AsteroidsGameState *asteroidsLoadSettings()
{
	JSON_Value *user_data;
	char *fileData = gameLoadFile((char*)"Asteroids.json");
	user_data = json_parse_string(fileData);
	free(fileData);

	if (user_data == nullptr)
		return nullptr;

	AsteroidsGameState *state = new AsteroidsGameState();
	state->hiScore = (int)json_object_get_number(json_object(user_data), (char*)"score");
	state->submitted = json_object_get_boolean(json_object(user_data), (char*)"submitted");

	return state;
}

void uploadToLeaderBoard(GameBuff *gameBuff)
{
#ifdef ESP32
	if (asteroidsGameState->submitted) {
		Serial.println("Already submitted, no upload");
		return;
	}

	if (asteroidsGameState->hiScore == 0) {
		Serial.println("No hi score, no upload");
		return;
	}

	// Connect to WiFi network
	Serial.println("Start up Wifi");
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP().toString());
	char line[30];
	IPAddress ip = WiFi.localIP();
	sprintf(line, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	HTTPClient http;
	http.begin(AsteroidsURL);							//Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json"); //Specify content-type header
	
	// Read file into memory
	Serial.println("Read File into memory");
	char *fileData = gameLoadFile((char*)"Asteroids.json");
	if (fileData == nullptr) {
		Serial.println("Failed reading file into memory");
	
		http.end(); //Free resources
		WiFi.disconnect();
		WiFi.mode(WIFI_OFF);
		return;
	}

	int httpResponseCode = http.POST(fileData); //Send the actual POST request
	if (httpResponseCode > 0)
	{

		String response = http.getString(); //Get the response to the request
		response.toCharArray(line, 30);
		Serial.println(httpResponseCode); //Print return code
		Serial.println(response);		  //Print request answer
	}
	else
	{
		Serial.print("Error on sending POST: ");
		Serial.println(httpResponseCode);
	}

	free(fileData);
	http.end(); //Free resources
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	
	// Save that this has been submitted already
	asteroidsGameState->submitted = true;
	asteroidsSaveSettings(gameBuff, asteroidsGameState);
#endif
}

void downloadLeaderboard()
{
#ifdef ESP32
	// Connect to WiFi network
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP().toString());
	char line[30];
	IPAddress ip = WiFi.localIP();
	sprintf(line, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	HTTPClient http;
	http.begin(AsteroidsLeaderURL);	//Specify destination for HTTP request
	int httpResponseCode = http.GET(); //Send the actual POST request
	if (httpResponseCode > 0)
	{

		String response = http.getString(); //Get the response to the request
		response.toCharArray(line, 30);
		Serial.println(httpResponseCode); //Print return code
		Serial.println(response);		  //Print request answer

		gameSaveFile((char*)"Leaderboard.json", (char*)response.c_str());
	}
	else
	{
		Serial.print("Error on GET: ");
		Serial.println(httpResponseCode);
	}

	http.end(); //Free resources
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
#else
	char *gameData = nullptr;
	if (gameData == nullptr) {
		char *LeaderBoard = (char*)"[  {    \"score\": 498,    \"name\": \"Gaiden    \",    \"mac\": \"24:0A:C4:12:3B:42\",    \"id\": 10  },  {    \"score\": 498,    \"name\": \"Gaiden    \",    \"mac\": \"24:0A:C4:12:3B:42\",    \"id\": 11  },  {    \"score\": 406,    \"name\": \"ESP32\",    \"mac\": \"00:11:22:33:44:55\",    \"id\": 9  },  {    \"score\": 120,    \"name\": \"rob\",    \"mac\": \"11:11:11:11\",    \"id\": 2  },  {    \"score\": 80,    \"name\": \"mike\",    \"mac\": \"11:11:21:11\",    \"id\": 6  }]";
		gameSaveFile((char*)"Leaderboard.json", LeaderBoard);
	} else {
		free(gameData);
	}
#endif
}

void displayLeaderBoard(GameBuff *gameBuff)
{
	// Put the instructions up
	drawString2x(gameBuff, (char*)"  LeaderBoard", 0, 0, 0xff, 0x00);
	drawString2x(gameBuff, (char*)"  -----------", 0, 16, 0xff, 0x00);

	drawString2x(gameBuff, (char*)"A|B  - Upload", 0, 200, 0xff, 0x00);
	drawString2x(gameBuff, (char*)"Down - Main menu", 0, 220, 0xff, 0x00);

	// Try load the json data
	char *leaderBoard;
	leaderBoard = gameLoadFile((char*)"Leaderboard.json");

	// If we don't have a leaderboard shortcut exit
	if (leaderBoard == nullptr)
		return;

	// Load the scores and names and show on screen
	JSON_Value *root_value;
	JSON_Array *scores;
	JSON_Object *score;
	uint8_t i = 0;

	root_value = json_parse_string(leaderBoard);
	if (json_value_get_type(root_value) != JSONArray)
	{
		drawString2x(gameBuff, (char*)"   (No Data)   ", 0, 16, 0xff, 0x00);
		return;
	}

	char scoreLine[20];
	scores = json_value_get_array(root_value);

	for (i = 0; i < json_array_get_count(scores); i++)
	{
		score = json_array_get_object(scores, i);
		sprintf(scoreLine, "%d %4d %s", i+1, (int)json_object_get_number(score, "score"), json_object_get_string(score, "name"));
		drawString2x(gameBuff, scoreLine, 0, (i + 2) * 16, MakeColor2(255-i*32,255-i*16,255-i*12), 0x00);
	}

	json_value_free(root_value);

	// Free the leaderboard if we ever actually found it, checked earlier, but safety check no 2 doesn't hurt
	if (leaderBoard != nullptr)
		free(leaderBoard);
}

void freeAsteroidGameState()
{
	for (int i = 0; i < 9; i++ )
	{
		free(asteroidsGameState->scrollerText[i]);
	}

	free(asteroidsGameState->scrollerText);	
	asteroidsGameState->scrollerText = nullptr;
	
	free(asteroidsGameState->rotAst);
	asteroidsGameState->rotAst = nullptr;

	free(asteroidsGameState->rotShip);
	asteroidsGameState->rotShip = nullptr;

	free(asteroidsGameState->asteroids);
	asteroidsGameState->asteroids = nullptr;
	
	free(asteroidsGameState);
	asteroidsGameState = nullptr;
}

void resetGameState(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	allocAsteroidMem(gameState);
	srand((unsigned int)time(0));
	gameState->win = false;

	gameState->player1.collision = false;
	gameState->player1.inPlay = true;

	gameState->score = 0;
	gameState->level = 1;

	gameState->player1.dim.width = 10;
	gameState->player1.dim.height = 10;

	// Scoring mechanism variables
	gameState->lastScore = 0;
	gameState->multiplier = 1;
	gameState->maxMultiplier = 10;
	gameState->scoreTimeMultiplier = 0;
}

void startLevel(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	gameState->win = false;
	gameState->player1.collision = false;

	// Initialise player
	gameState->player1.direction = 0;
	gameState->player1.rotation = 0;
	gameState->player1.thrust = FLOAT_TO_FIXP(0.05 + gameState->level / (double)100);
	gameState->player1.dim.x = gameBuff->WIDTH / 2;
	gameState->player1.dim.y = gameBuff->HEIGHT / 2;

	gameState->player1.fixX = INT_TO_FIXP(gameState->player1.dim.x);
	gameState->player1.fixY = INT_TO_FIXP(gameState->player1.dim.y);

	gameState->player1.movX = INT_TO_FIXP(0);
	gameState->player1.movY = INT_TO_FIXP(0);

	// No weapons fire
	for (int i = 0; i < FIRECOUNT; i++)
	{
		gameState->player1.fire[i].life = 0;
	}

	// 1 Asteroid per stage
	for (int i = 0; i < ASTEROIDS; i++)
	{
		gameState->asteroids[i].dim.width = 0;
		gameState->asteroids[i].dim.height = 0;
	}

	for (int i = 0; i < gameState->level; i++)
	{
		int size = 20;
		gameState->asteroids[i].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
		gameState->asteroids[i].size = FLOAT_TO_FIXP(size);
		gameState->asteroids[i].direction = INT_TO_FIXP(rand() % 360);
		gameState->asteroids[i].rotateAmount = rand() % 4000;
		gameState->asteroids[i].dim.width = size;
		gameState->asteroids[i].dim.height = size;

		gameState->asteroids[i].dim.x = rand() % 2 == 0 ? 0 : gameBuff->WIDTH;
		gameState->asteroids[i].dim.y = rand() % 2 == 0 ? 0 : gameBuff->HEIGHT;
		gameState->asteroids[i].fixX = INT_TO_FIXP(gameState->asteroids[i].dim.x);
		gameState->asteroids[i].fixY = INT_TO_FIXP(gameState->asteroids[i].dim.y);
	}
}

bool displayScroller(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	gameState->frameCounter += 1;
	return drawScroller(gameBuff, gameState->scrollerText, gameState->frameCounter);
}

bool displayOutroScroller(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	gameState->frameCounter += 1;
	return drawScroller(gameBuff, gameState->scrollerText, gameState->frameCounter);
}

void updateGame(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	// Pause
	if (gameBuff->playerKeys.start || gameBuff->playerKeys.select)
	{
		if (gameBuff->timeInMillis > gameState->pausedDebounce)
		{
			gameState->paused = !gameState->paused;
			gameState->pausedDebounce = gameBuff->timeInMillis + 1000;

			if (!gameState->paused)
			{
				// Unfreeze game time now. (Will be 1ms in displayGame)
				frameTime_last = gameBuff->timeInMillis;
				frameTime_current = gameBuff->timeInMillis;
				frameTimeDiff = frameTime_current - frameTime_last;
				if (frameTimeDiff == 0)
					frameTimeDiff = 1;
			}
		}
	}

	if (gameState->paused)
		return;

	// If there's no Asteroids left ... WIN!
	gameState->win = true;
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.height != 0)
		{
			gameState->win = false;
		}
	}

	if (gameState->win)
	{
		gameState->level++;
	}

	// Proces player input
	if (gameBuff->playerKeys.left)
	{
		gameState->player1.rotation -= 0.1;
		if (gameState->player1.rotation < 0)
			gameState->player1.rotation = 6;
		gameState->player1.direction = FLOAT_TO_FIXP(gameState->player1.rotation * 60);
	}

	if (gameBuff->playerKeys.right)
	{
		gameState->player1.rotation += 0.1;
		if (gameState->player1.rotation > 6)
			gameState->player1.rotation = 0;
		gameState->player1.direction = FLOAT_TO_FIXP(gameState->player1.rotation * 60);
	}

	if (gameBuff->playerKeys.up || gameBuff->playerKeys.b)
	{
		gameState->player1.movX += xVec(gameState->player1.thrust, gameState->player1.direction);
		gameState->player1.movY += yVec(gameState->player1.thrust, gameState->player1.direction);
	}

	if (gameBuff->playerKeys.down)
	{
		gameState->player1.movX -= xVec(gameState->player1.thrust, gameState->player1.direction) / 3;
		gameState->player1.movY -= yVec(gameState->player1.thrust, gameState->player1.direction) / 3;
	}

	if (gameBuff->playerKeys.a)
	{
		if (gameState->player1.firetimeout < gameBuff->timeInMillis)
		{
			for (int i = 0; i < FIRECOUNT; i++)
			{
				if (gameState->player1.fire[i].life > 0)
					continue;
				gameState->player1.firetimeout = gameBuff->timeInMillis + FIREPACING;
				gameState->player1.fire[i].dim.height = 1;
				gameState->player1.fire[i].dim.width = 1;
				gameState->player1.fire[i].dim.x = gameState->player1.dim.x + gameState->player1.dim.width / 2;
				gameState->player1.fire[i].dim.y = gameState->player1.dim.y + gameState->player1.dim.height / 2;
				gameState->player1.fire[i].fixX = gameState->player1.fixX + INT_TO_FIXP(gameState->player1.dim.width / 2);
				gameState->player1.fire[i].fixY = gameState->player1.fixY + INT_TO_FIXP(gameState->player1.dim.height / 2);
				gameState->player1.fire[i].movX = gameState->player1.movX;
				gameState->player1.fire[i].movY = gameState->player1.movY;

				gameState->player1.fire[i].movX += xVec(FIREPOWER, gameState->player1.direction);
				gameState->player1.fire[i].movY += yVec(FIREPOWER, gameState->player1.direction);

				gameState->player1.fire[i].life = 2000;
				break;
			}
		}
	}

	// Update ship position
	gameState->player1.fixX += gameState->player1.movX;
	gameState->player1.fixY += gameState->player1.movY;

	if (gameState->player1.fixX < INT_TO_FIXP(0))
		gameState->player1.fixX += INT_TO_FIXP(gameBuff->WIDTH);
	if (gameState->player1.fixX > INT_TO_FIXP(gameBuff->WIDTH))
		gameState->player1.fixX -= INT_TO_FIXP(gameBuff->WIDTH);

	if (gameState->player1.fixY < INT_TO_FIXP(0))
		gameState->player1.fixY += INT_TO_FIXP(gameBuff->HEIGHT);
	if (gameState->player1.fixY > INT_TO_FIXP(gameBuff->HEIGHT))
		gameState->player1.fixY -= INT_TO_FIXP(gameBuff->HEIGHT);

	gameState->player1.dim.x = FIXP_TO_INT(gameState->player1.fixX) % gameBuff->WIDTH;
	gameState->player1.dim.y = FIXP_TO_INT(gameState->player1.fixY) % gameBuff->HEIGHT;

	// Update the Asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.width)
		{
			gameState->asteroids[i].fixX += xVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);
			gameState->asteroids[i].fixY += yVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);

			if (gameState->asteroids[i].fixX < INT_TO_FIXP(0))
				gameState->asteroids[i].fixX += INT_TO_FIXP(gameBuff->WIDTH);
			if (gameState->asteroids[i].fixX > INT_TO_FIXP(gameBuff->WIDTH))
				gameState->asteroids[i].fixX -= INT_TO_FIXP(gameBuff->WIDTH);

			if (gameState->asteroids[i].fixY < INT_TO_FIXP(0))
				gameState->asteroids[i].fixY += INT_TO_FIXP(gameBuff->HEIGHT);
			if (gameState->asteroids[i].fixY > INT_TO_FIXP(gameBuff->HEIGHT))
				gameState->asteroids[i].fixY -= INT_TO_FIXP(gameBuff->HEIGHT);

			gameState->asteroids[i].dim.x = FIXP_TO_INT(gameState->asteroids[i].fixX) % gameBuff->WIDTH;
			gameState->asteroids[i].dim.y = FIXP_TO_INT(gameState->asteroids[i].fixY) % gameBuff->HEIGHT;
			gameState->asteroids[i].rotation = ((double)gameBuff->timeInMillis) / gameState->asteroids[i].rotateAmount;
		}
	}

	// Update the weapons
	for (int i = 0; i < FIRECOUNT; i++)
	{
		if (gameState->player1.fire[i].life <= 0)
			continue;

		gameState->player1.fire[i].fixX += gameState->player1.fire[i].movX;
		gameState->player1.fire[i].fixY += gameState->player1.fire[i].movY;

		if (gameState->player1.fire[i].fixX < INT_TO_FIXP(0))
			gameState->player1.fire[i].fixX += INT_TO_FIXP(gameBuff->WIDTH);
		if (gameState->player1.fire[i].fixX > INT_TO_FIXP(gameBuff->WIDTH))
			gameState->player1.fire[i].fixX -= INT_TO_FIXP(gameBuff->WIDTH);

		if (gameState->player1.fire[i].fixY < INT_TO_FIXP(0))
			gameState->player1.fire[i].fixY += INT_TO_FIXP(gameBuff->HEIGHT);
		if (gameState->player1.fire[i].fixY > INT_TO_FIXP(gameBuff->HEIGHT))
			gameState->player1.fire[i].fixY -= INT_TO_FIXP(gameBuff->HEIGHT);

		gameState->player1.fire[i].dim.x = FIXP_TO_INT(gameState->player1.fire[i].fixX) % gameBuff->WIDTH;
		gameState->player1.fire[i].dim.y = FIXP_TO_INT(gameState->player1.fire[i].fixY) % gameBuff->HEIGHT;

		gameState->player1.fire[i].life -= frameTimeDiff;
	}

	bool hasRotShip = false;

	// Collision detect ship vs Asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->player1.collision)
			break;

		// If it's a valid asteroid
		if (gameState->asteroids[i].dim.width)
		{
			// If we have a collision on the bounding box
			if (rectCollisionCheck(gameState->player1.dim, gameState->asteroids[i].dim))
			{
				if (!hasRotShip)
				{
					for (int i = 0; i < 100; i++) {
						gameState->rotShip[i] = 0;
					}

					hasRotShip = true;
					rotateObject(gameState->player1.dim, gameState->player1.rotation, 1, Ship10x10, gameState->rotShip, 0x00);
				}

				for (int i = 0; i < 400; i++) {
					gameState->rotAst[i] = 0;
				}

				//Do a mask collision check
				switch (gameState->asteroids[i].dim.height)
				{
				case 20:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, gameState->rotAst, 0x00);
					break;
				case 10:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, gameState->rotAst, 0x00);
					break;
				case 5:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, gameState->rotAst, 0x00);
					break;
				}

				if (maskCollisionCheck(gameState->player1.dim, gameState->asteroids[i].dim, gameState->rotShip, gameState->rotAst))
				{
					gameState->player1.collision = true;
				}
			}
		}
	}

	// Collider Check
	if (gameState->player1.collision)
	{
		gameState->player1.inPlay = false;
		gameState->scene = 5;
		if (gameState->score > gameState->hiScore)
		{
			gameState->hiScore = gameState->score;
			asteroidsGameState->submitted = false;
			asteroidsSaveSettings(gameBuff, gameState);
		}

		if (gameState->score > 300)
			if (!gameBuff->achievementData->asteroids300) {
				gameBuff->achievementData->asteroids300 = true;
				gameBuff->achievementData->newAchievment = true;
				saveAchievements(gameBuff->achievementData);
			}
	}

	// Collision check bullets vs Asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		// If it's a valid asteroid
		if (gameState->asteroids[i].dim.width)
		{
			// If we have a collision on the bounding box
			for (int j = 0; j < FIRECOUNT; j++)
			{
				if (gameState->player1.fire[j].life <= 0)
					continue;
				if (rectCollisionCheck(gameState->player1.fire[j].dim, gameState->asteroids[i].dim))
				{
					for (int i = 0; i < 400; i++) {
						gameState->rotAst[i] = 0;
					}

					//Do a mask collision check
					switch (gameState->asteroids[i].dim.height)
					{
					case 20:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, gameState->rotAst, 0x00);
						break;
					case 10:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, gameState->rotAst, 0x00);
						break;
					case 5:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, gameState->rotAst, 0x00);
						break;
					}

					if (maskCollisionCheck(gameState->player1.fire[j].dim, gameState->asteroids[i].dim, Bullet, gameState->rotAst))
					{
						//Explode this one!
						// If the score time multiplier wasn't over (1 second default) then give bonus points.

						if (gameState->scoreTimeMultiplier > gameBuff->timeInMillis)
						{
							gameState->score += gameState->asteroids[i].dim.width / 2;
						}

						// Reset score multiplier
						gameState->scoreTimeMultiplier = gameBuff->timeInMillis + SCORETIMEMULTTIMEOUT;

						// If the size is the same as the last asteroid, multiplier goes up.
						if (gameState->lastScore == gameState->asteroids[i].dim.width)
						{
							// Multiplier code
							if (gameState->multiplier > gameState->maxMultiplier)
							{
								gameState->multiplier = gameState->maxMultiplier;
							}
							else
							{
								gameState->multiplier += 1;
							}
						}
						else
						{
							// Reset the multiplier
							gameState->multiplier = 1;
						}

						gameState->score += gameState->asteroids[i].dim.width * gameState->multiplier;
						gameState->lastScore = gameState->asteroids[i].dim.width;
						int counter = 0;
						int size = 0;

						switch (gameState->asteroids[i].dim.width)
						{
						case 10:
							//Spawn 3 x 5
							counter = 2;
							size = 5;
							break;
						case 20:
							//Spawn 3 x 10
							counter = 2;
							size = 10;
							break;
						}

						// Spawn Asteroids
						while (counter > 0)
						{
							for (int emptyAsteroid = 0; emptyAsteroid < ASTEROIDS; emptyAsteroid++)
							{
								if (gameState->asteroids[emptyAsteroid].dim.width == 0)
								{
									counter--;
									gameState->asteroids[emptyAsteroid].fixX = gameState->asteroids[i].fixX;
									gameState->asteroids[emptyAsteroid].fixY = gameState->asteroids[i].fixY;
									gameState->asteroids[emptyAsteroid].dim.x = gameState->asteroids[i].dim.x;
									gameState->asteroids[emptyAsteroid].dim.y = gameState->asteroids[i].dim.y;

									gameState->asteroids[emptyAsteroid].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
									gameState->asteroids[emptyAsteroid].size = FLOAT_TO_FIXP(size);
									gameState->asteroids[emptyAsteroid].direction = INT_TO_FIXP(rand() % 360);
									gameState->asteroids[emptyAsteroid].rotateAmount = rand() % 4000;
									gameState->asteroids[emptyAsteroid].dim.width = size;
									gameState->asteroids[emptyAsteroid].dim.height = size;
									break;
								}
							}
						}

						gameState->asteroids[i].dim.width = 0;
						gameState->asteroids[i].dim.height = 0;

						// Bullet is done
						gameState->player1.fire[j].life = 0;
					}
				}
			}
		}
	}
}

static void drawAsteroids(AsteroidsGameState *gameState, GameBuff *gameBuff) {
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.height > 0)
		{
			switch (gameState->asteroids[i].dim.height)
			{
			case 20:
				rotateObject(gameBuff, gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20,0x00);
				break;
			case 10:
				rotateObject(gameBuff, gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10,0x00);
				break;
			case 5:
				rotateObject(gameBuff, gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5,0x00);
				break;
			}
		}
	}
}

void displayGame(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	if (gameState->paused)
	{
		drawString2x(gameBuff, (char*)"Paused", 60, 100, 0xFF, 0x00);
		return;
	}
	else
	{
		frameTime_last = frameTime_current;
		frameTime_current = gameBuff->timeInMillis;
		frameTimeDiff = frameTime_current - frameTime_last;
		if (frameTimeDiff == 0)
			frameTimeDiff = 1;
	}

	displayClear(gameBuff, 0x00);

	// Draw ship
	rotateObject(gameBuff, gameState->player1.dim, gameState->player1.rotation, 1, Ship10x10,0x00);

	//Draw asteroids
	drawAsteroids(gameState, gameBuff);

	char topline[30];
	sprintf(topline, "Scr %5d M %d%c Lvl %2d", gameState->score, gameState->multiplier, gameState->scoreTimeMultiplier > gameBuff->timeInMillis ? 'x' : 'o', gameState->level);
	//sprintf(topline, "%3.1f %3.1f %3.1f",FIXP_TO_FLOAT(gameState->player1.direction),	FIXP_TO_FLOAT(xVec(FIXP_1, gameState->player1.direction)),FIXP_TO_FLOAT(yVec(FIXP_1, gameState->player1.direction)));
	drawString(gameBuff, topline, 1, 1, 0xFF, 0x00);

	// Draw bullets
	for (int i = 0; i < FIRECOUNT; i++)
	{
		if (gameState->player1.fire[i].life <= 0)
			continue;
		
		drawObject(gameBuff, gameState->player1.fire[i].dim, Bullet, 0x00);
	}
}

void processAttractMode(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	// Press Start or Select to start
	if (gameBuff->playerKeys.debouncedInput) {
		if (gameBuff->playerKeys.start || gameBuff->playerKeys.select)
		{
			gameState->scene = 1;
		}
		// Press Up to upload and get latest scores
		if (gameBuff->playerKeys.up)
		{
			gameState->scene = 9;
		}
	}
}

void initAttractMode(AsteroidsGameState *gameState)
{
	for (int i = 0; i < ASTEROIDS; i++)
	{
		gameState->asteroids[i].dim.width = 0;
		gameState->asteroids[i].dim.height = 0;
	}

	for (int i = 0; i < rand() % 8 + 2; i++)
	{
		int size = rand() % 3;
		switch (size)
		{
		case 0:
			size = 5;
			break;
		case 1:
			size = 10;
			break;
		case 2:
			size = 20;
			break;
		}

		gameState->asteroids[i].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
		gameState->asteroids[i].size = FLOAT_TO_FIXP(size);
		gameState->asteroids[i].direction = INT_TO_FIXP(rand() % 360);
		gameState->asteroids[i].rotateAmount = rand() % 4000;
		gameState->asteroids[i].dim.width = size;
		gameState->asteroids[i].dim.height = size;
	}
}

void updateAttractMode(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	// Draw some asteroids floating in the background
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.width)
		{
			gameState->asteroids[i].fixX += xVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);
			gameState->asteroids[i].fixY += yVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);

			if (gameState->asteroids[i].fixX < INT_TO_FIXP(0))
				gameState->asteroids[i].fixX += INT_TO_FIXP(gameBuff->WIDTH);
			if (gameState->asteroids[i].fixX > INT_TO_FIXP(gameBuff->WIDTH))
				gameState->asteroids[i].fixX -= INT_TO_FIXP(gameBuff->WIDTH);

			if (gameState->asteroids[i].fixY < INT_TO_FIXP(0))
				gameState->asteroids[i].fixY += INT_TO_FIXP(gameBuff->HEIGHT);
			if (gameState->asteroids[i].fixY > INT_TO_FIXP(gameBuff->HEIGHT))
				gameState->asteroids[i].fixY -= INT_TO_FIXP(gameBuff->HEIGHT);

			gameState->asteroids[i].dim.x = FIXP_TO_INT(gameState->asteroids[i].fixX) % gameBuff->WIDTH;
			gameState->asteroids[i].dim.y = FIXP_TO_INT(gameState->asteroids[i].fixY) % gameBuff->HEIGHT;
			gameState->asteroids[i].rotation = ((double)gameBuff->timeInMillis) / gameState->asteroids[i].rotateAmount;
		}
	}
}

void displayAttractMode(AsteroidsGameState *gameState, GameBuff *gameBuff)
{
	// Clear the screen
	displayClear(gameBuff, 0x00);

	//Draw asteroids
	drawAsteroids(gameState, gameBuff);

	// Alternate press button text on and off every second
	if (gameBuff->timeInMillis / 1000 % 2 == 0)
	{
		drawString2x(gameBuff, (char*)"Press Start", 40, 90, 0xFF, 0x00);
		drawString2x(gameBuff, (char*)" to begin  ", 40, 110, 0xFF, 0x00);
	}
	else
	{
		drawString2x(gameBuff, (char*)"Press Up", 60, 190, 0xFF, 0x00);
		drawString2x(gameBuff, (char*)"for leaderboard", 0, 210, 0xFF, 0x00);
	}

	char hiScore[30];
	sprintf(hiScore, "Hi Score : %7d", gameState->hiScore);
	drawString(gameBuff, hiScore, 0, 0, 0xFF, 0x00);
}

bool asteroidsLoop(GameBuff *gameBuff)
{
	if (asteroidsGameState == nullptr)
	{
		asteroidsGameState = asteroidsLoadSettings();
		if (asteroidsGameState == nullptr)
		{
			// Initial run, save settings
			asteroidsGameState = (AsteroidsGameState *)calloc(1, sizeof(struct AsteroidsGameState));

			resetGameState(asteroidsGameState, gameBuff);
			asteroidsSaveSettings(gameBuff, asteroidsGameState);
		}
		else
		{
			resetGameState(asteroidsGameState, gameBuff);
		}
	}

	switch (asteroidsGameState->scene)
	{
	case -1: // Load Screen
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			// showLogo(asteroids, gameBuff);
		}
		else
		{
			updateMinTime(1000);
			asteroidsGameState->scene++;
		}
		break;
	case 0: // Attract screen
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			initAttractMode(asteroidsGameState);
		}

		processAttractMode(asteroidsGameState, gameBuff);
		updateAttractMode(asteroidsGameState, gameBuff);
		displayAttractMode(asteroidsGameState, gameBuff);
		break;
	case 1: // Intro
		if (!gameBuff->achievementData->asteroidsPlay) {
            gameBuff->achievementData->asteroidsPlay = true;
            gameBuff->achievementData->newAchievment = true;
            saveAchievements(gameBuff->achievementData);
        }

		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			asteroidsGameState->frameCounter = 0;
			resetGameState(asteroidsGameState, gameBuff);

			strcpy(asteroidsGameState->scrollerText[0], (char*)"The longest");
			strcpy(asteroidsGameState->scrollerText[1], (char*)"journey starts");
			strcpy(asteroidsGameState->scrollerText[2], (char*)"with a single");
			strcpy(asteroidsGameState->scrollerText[3], (char*)"step.");
			strcpy(asteroidsGameState->scrollerText[4], (char*)"");
			strcpy(asteroidsGameState->scrollerText[5], (char*)"  Or Asteroid  ");
			strcpy(asteroidsGameState->scrollerText[6], (char*)"   Good Luck!  ");
			strcpy(asteroidsGameState->scrollerText[7], (char*)"");
			strcpy(asteroidsGameState->scrollerText[8], (char*)" -= Asteroid =- ");
		}

		displayClear(gameBuff, 0x00);

		if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.a && gameBuff->playerKeys.b) {
			asteroidsGameState->scene = 4;
		}

		if (!displayScroller(asteroidsGameState, gameBuff))
		{
			asteroidsGameState->scene = 4;
		}

		break;
	case 3: // Asteroid!
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			if (asteroidsGameState->level == 1)
			{
				resetGameState(asteroidsGameState, gameBuff);
				startLevel(asteroidsGameState, gameBuff);
			}
		}

		if (asteroidsGameState->win)
		{
			if (asteroidsGameState->level >= 5)
			{
				asteroidsGameState->scene = 2;
				asteroidsGameState->win = false;
			}
			else
			{
				startLevel(asteroidsGameState, gameBuff);
				asteroidsGameState->scene = 4;
			}
			return false;
		}
		updateGame(asteroidsGameState, gameBuff);
		displayGame(asteroidsGameState, gameBuff);
		break;
	case 2: // Outro
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			asteroidsGameState->frameCounter = 0;

			strcpy(asteroidsGameState->scrollerText[0], "Well done on    ");
			strcpy(asteroidsGameState->scrollerText[1], "saving earth    ");
			strcpy(asteroidsGameState->scrollerText[2], "Your mission    ");
			strcpy(asteroidsGameState->scrollerText[3], "is done.        ");
			strcpy(asteroidsGameState->scrollerText[4], "Get back home & ");
			strcpy(asteroidsGameState->scrollerText[5], "get some R&R    ");
			strcpy(asteroidsGameState->scrollerText[6], "                ");
			strcpy(asteroidsGameState->scrollerText[7], "                ");
			strcpy(asteroidsGameState->scrollerText[8], " -= Congrats =- ");
		}

		displayClear(gameBuff, 0x00);
		if (!displayOutroScroller(asteroidsGameState, gameBuff))
		{
			asteroidsGameState->scene = 0;
			asteroidsGameState->win = false;
			return false;
		}

		break;
	case 5: // Game Over
	{
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
		}

		if (asteroidsGameState->frameCounter == 50)
		{
			asteroidsGameState->scene = 0;
		}

		drawString2x(gameBuff,(char*)"Game Over", 40, 120, 0xFF, 0x00);
		asteroidsGameState->frameCounter++;
	}
	break;
	case 4: // Level Slider
	{
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			displayClear(gameBuff, 0x00);
		}

		if (asteroidsGameState->frameCounter == 30)
		{
			asteroidsGameState->scene = 3;
		}

		displayClear(gameBuff, 0x00);

		char fps[16];
		sprintf(fps, "Level %i", asteroidsGameState->level);
		drawString2x(gameBuff, fps, 60, 100, 0xFF, 0x00);

		asteroidsGameState->frameCounter++;
	}
	break;
	case 9: // Leaderboard
	{
		// On Load show current leaderboard
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			displayClear(gameBuff, 0x00);
			displayLeaderBoard(gameBuff);
		}
	
		// A or B to upload and download
		if (gameBuff->playerKeys.a || gameBuff->playerKeys.b) {
			asteroidsGameState->scene = 10; // Upload
		}

		// Back to game
		if (gameBuff->playerKeys.down) {
			asteroidsGameState->scene = -1;
		}
	}
	break;
	case 10: { //Upload 
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			displayClear(gameBuff, 0x00);
			drawString2x(gameBuff,(char*)"Uploading...",0,0,0xff,0x00);
			updateMinTime(1000);
		} else {
			uploadToLeaderBoard(gameBuff);
			drawString2x(gameBuff,(char*)"Uploaded!",0,16,0xff,0x00);
			asteroidsGameState->scene = 11;
			updateMinTime(1000);
		}
	}
	break;
	case 11: { // Download
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			drawString2x(gameBuff,(char*)"Downloading.",0,32,0xff,0x00);
			updateMinTime(1000);
		} else {
			downloadLeaderboard();
			drawString2x(gameBuff,(char*)"Downloaded!",0,48,0xff,0x00);
			asteroidsGameState->scene = 9;
			updateMinTime(1000);
		}
	}
	break;
	case 12: { // Offer to upload
		if (asteroidsGameState->lastscene != asteroidsGameState->scene)
		{
			asteroidsGameState->frameCounter = 0;
			asteroidsGameState->lastscene = asteroidsGameState->scene;
			displayClear(gameBuff,0x00);
			int i = 2;
			drawString2x(gameBuff,(char*)"New Hi Score!",0,16*i++,0xff,0x00);
			drawString2x(gameBuff,(char*)"Would you like",0,16*i++,0xff,0x00);
			drawString2x(gameBuff,(char*)"Upload ?",0,16*i++,0xff,0x00);
			drawString2x(gameBuff,(char*)"Start : Yes",0,16*i++,0xff,0x00);
			drawString2x(gameBuff,(char*)"A or B : No",0,16*i++,0xff,0x00);
		} else {
			if (gameBuff->playerKeys.debouncedInput) {
				if (gameBuff->playerKeys.start || gameBuff->playerKeys.select) {
					asteroidsGameState->scene = 10;
				} else if (gameBuff->playerKeys.a || gameBuff->playerKeys.b) {
					asteroidsGameState->scene = 3;
				}
			}		
		}
	}
	break;
	}

	if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select)
	{
		freeAsteroidGameState();
		return true;
	}
	return false;
}
