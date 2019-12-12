#include "cryptoRunner.h"

#include "background.h"
#include "cryptoRunnerLogo.h"
#include "car.h"
#include "taxi.h"

#include "carGame.h"
#include "secrets.h"

CryptoGameState *cryptoGameState = nullptr;

void cryptoDownloadLeaderboard(GameBuff *gameBuff)
{
#ifdef ESP32
	// Connect to WiFi network
	WiFi.begin(gameBuff->badgeState->ssid, gameBuff->badgeState->password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(gameBuff->badgeState->ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP().toString());
	char line[30];
	IPAddress ip = WiFi.localIP();
	sprintf(line, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	HTTPClient http;
	http.begin(CryptoLeaderURL,ca_cert);	//Specify destination for HTTP request
	int httpResponseCode = http.GET(); //Send the actual POST request
	if (httpResponseCode > 0)
	{
		String response = http.getString(); //Get the response to the request
		response.toCharArray(line, 30);
		Serial.println(httpResponseCode); //Print return code
		Serial.println(response);		  //Print request answer

		gameSaveFile((char*)"CryptoLead.json", (char*)response.c_str());
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
		char *LeaderBoard = (char*)"[  {    \"hi_drive_time\": 59,    \"hi_office_time\": 122,    \"hi_vr1_time\": 18,    \"hi_vr2_time\": 10,    \"hi_vr3_time\": 11,    \"hi_total_time\": 220,    \"id\": 2  }]";
		gameSaveFile((char*)"CryptoLead.json", LeaderBoard);
	} else {
		free(gameData);
	}
#endif
}

void cryptoUploadToLeaderBoard(GameBuff *gameBuff,CryptoGameState *cryptoGameState)
{
#ifdef ESP32
	if (cryptoGameState->hi_total_time == 0 || cryptoGameState->hi_total_time == 999) {
		Serial.println("No hi score, no upload");
		return;
	}

	// Connect to WiFi network
	Serial.println("Start up Wifi");
	WiFi.begin(gameBuff->badgeState->ssid , gameBuff->badgeState->password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(gameBuff->badgeState->ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP().toString());
	char line[30];
	IPAddress ip = WiFi.localIP();
	sprintf(line, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	HTTPClient http;
	http.begin(CryptoURL,ca_cert);							//Specify destination for HTTP request
	http.addHeader("Content-Type", "application/json"); //Specify content-type header
	
	// Read file into memory
	Serial.println("Read File into memory");
	char *fileData = gameLoadFile((char*)"Crypto.json");
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
#endif
}

void cryptoSaveSettings(CryptoGameState *cryptoGameState)
{
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_number(root_object, "hi_drive_time", cryptoGameState->hi_drive_time);
	json_object_set_number(root_object, "hi_office_time", cryptoGameState->hi_office_time);
	json_object_set_number(root_object, "hi_vr1_time", cryptoGameState->hi_vr1_time);
	json_object_set_number(root_object, "hi_vr2_time", cryptoGameState->hi_vr2_time);
	json_object_set_number(root_object, "hi_vr3_time", cryptoGameState->hi_vr3_time);
	json_object_set_number(root_object, "hi_total_time", cryptoGameState->hi_total_time);

	char *nameString = json_serialize_to_string_pretty(root_value);

	gameSaveFile((char*)"Crypto.json", nameString);

	json_free_serialized_string(nameString);
	json_value_free(root_value);
}

bool cryptoLoadRemote(CryptoGameState *cryptoGameState) {
	JSON_Value *user_data;
	char *fileData = gameLoadFile((char*)"CryptoLead.json");
	user_data = json_parse_string(fileData);
	free(fileData);

	if (user_data == nullptr)
		return false;

	// Load the scores and names and show on screen
	JSON_Array *scores;
	JSON_Object *score;
	uint8_t i = 0;

	if (json_value_get_type(user_data) != JSONArray)
	{
		return false;
	}
	
	scores = json_value_get_array(user_data);
	score = json_array_get_object(scores, 0);

	cryptoGameState->remote_hi_drive_time = (int)json_object_get_number(score, "hi_drive_time");
	cryptoGameState->remote_hi_office_time = (int)json_object_get_number(score, "hi_office_time");
	cryptoGameState->remote_hi_vr1_time = (int)json_object_get_number(score, "hi_vr1_time");
	cryptoGameState->remote_hi_vr2_time = (int)json_object_get_number(score, "hi_vr2_time");
	cryptoGameState->remote_hi_vr3_time = (int)json_object_get_number(score, "hi_vr3_time");
	cryptoGameState->remote_hi_total_time = (int)json_object_get_number(score, "hi_total_time");
	
	return true;
}

bool cryptoLoadSettings(CryptoGameState *cryptoGameState)
{
	if (cryptoGameState == nullptr) return false;

	JSON_Value *user_data;
	char *fileData = gameLoadFile((char*)"Crypto.json");
	user_data = json_parse_string(fileData);
	free(fileData);

	if (user_data == nullptr)
		return false;

	cryptoGameState->hi_drive_time = (int)json_object_get_number(json_object(user_data), "hi_drive_time");
	cryptoGameState->hi_office_time = (int)json_object_get_number(json_object(user_data), "hi_office_time");
	cryptoGameState->hi_vr1_time = (int)json_object_get_number(json_object(user_data), "hi_vr1_time");
	cryptoGameState->hi_vr2_time = (int)json_object_get_number(json_object(user_data), "hi_vr2_time");
	cryptoGameState->hi_vr3_time = (int)json_object_get_number(json_object(user_data), "hi_vr3_time");
	cryptoGameState->hi_total_time = (int)json_object_get_number(json_object(user_data), "hi_total_time");

	return true;
}

bool cryptoRunnerGameLoop(GameBuff *gameBuff)
{
	if (cryptoGameState == nullptr)
	{
		if (cryptoGameState == nullptr)
			cryptoGameState = (CryptoGameState *)calloc(1, sizeof(struct CryptoGameState));

		setupCryptoGameState(cryptoGameState);
	}

	int i = 70;
	int counter = 1;
	switch (cryptoGameState->Scene)
	{
	case 0: // Game Front Page - 1 second
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			showLogoRLE(cryptorunner_image, cryptorunner_image_size, gameBuff);
		}
		else
		{
			updateMinTime(1000);
			cryptoGameState->Scene = 1;
		}

		break;
	case 1: // Start Screen -> Press Start to play
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 1;
			cryptoLoadSettings(cryptoGameState);
		}

		Dimensions dim;
		dim.x = 48;
		dim.y = 100;
		dim.height = 100;
		dim.width = 150;
		drawBlock(gameBuff, dim, 0x00);

		drawString2x(gameBuff,(char *)"   New     ", 32, i += 16, cryptoGameState->selection == counter++ ? 0x13 : 0xFF, 0);
		drawString2x(gameBuff,(char *)"   Time    ", 32, i += 16, cryptoGameState->selection == counter++ ? 0x13 : 0xFF, 0);
		drawString2x(gameBuff,(char *)"  Update   ", 32, i += 16, cryptoGameState->selection == counter++ ? 0x13 : 0xFF, 0);
		drawString2x(gameBuff,(char *)"   Exit    ", 32, i += 16, cryptoGameState->selection == counter++ ? 0x13 : 0xFF, 0);
		counter--;
		if (gameBuff->playerKeys.up && gameBuff->playerKeys.debouncedInput)
		{
			cryptoGameState->selection -= 1;
		}

		if (gameBuff->playerKeys.down && gameBuff->playerKeys.debouncedInput)
		{
			cryptoGameState->selection += 1;
		}

		if (cryptoGameState->selection > counter)
			cryptoGameState->selection = 1;
		if (cryptoGameState->selection < 1)
			cryptoGameState->selection = counter;

		if (gameBuff->playerKeys.start || gameBuff->playerKeys.select)
		{
			switch (cryptoGameState->selection)
			{
			case 1:
				setupCryptoGameState(cryptoGameState);
				cryptoGameState->Scene = 15; // Fader
				cryptoGameState->LastScene = 2; // Next scene from Fader
				break;
			case 2:
				cryptoLoadSettings(cryptoGameState);
				cryptoLoadRemote(cryptoGameState);
				cryptoGameState->Scene = 12;
				break;
			case 3:
				cryptoUploadToLeaderBoard(gameBuff, cryptoGameState);
				cryptoDownloadLeaderboard(gameBuff);
				cryptoLoadSettings(cryptoGameState);
				cryptoLoadRemote(cryptoGameState);
				cryptoGameState->Scene = 12;
				break;
			case 4:
				freeCryptoGameState(cryptoGameState);
				cryptoGameState = nullptr;
				return true;
				break;
			}
		}
		// New, Load, Save, Continue, Exit
		break;
	case 2: // Intro Story Scroller
		if (!gameBuff->achievementData->cryptoHunterPlay) {
			gameBuff->achievementData->cryptoHunterPlay = true;
			gameBuff->achievementData->newAchievment = true;
			saveAchievements(gameBuff->achievementData);
		}

		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
			scrollerLoop(gameBuff);
			char scrollerText[20][16] = {
				"The world is   ",
				"dominated by   ",
				"security       ",
				"experts now.   ",
				"               ",
				"Your company is",
				"considered the ",
				"front of the   ",
				"revolution, but",
				"the servers are",
				"under attack.  ",
				"               ",
				"Your boss lost ",
				"the security   ",
				"keys in a      ",
				"boating mishap.",
				"               ",
				"Go onsite to   ",
				"secure the     ",
				"servers!"};
			scrollerSetString(scrollerText);
		}

		if (scrollerLoop(gameBuff) || (gameBuff->playerKeys.a && gameBuff->playerKeys.b))
		{
			deInitScroller();
			cryptoGameState->Scene += 1;
		}
		break;
	case 3: // Drive Game
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			startUpCarGame(cryptoGameState, gameBuff);
		}

		displayClear(gameBuff, 0x00);
		if (!updateDriveGame(gameBuff, cryptoGameState))
		{
			// You win
			cryptoGameState->Scene = 5;
		};
		displayDriveGame(gameBuff, cryptoGameState);
		break;
	case 4: // Die Drive Game
		break;
	case 5: // Office Story Scroller
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
			scrollerLoop(gameBuff);

			char scrollerText[20][16] = {
				"You've made it ",
				"to the office  ",
				"in time to de- ",
				"activate the   ",
				"firewall and   ",
				"take control of",
				"the server back",
				"               ",
				"Everything is  ",
				"broken!        ",
				"               ",
				"But there is   ",
				"still hope.    ",
				"               ",
				"Find the 3 keys",
				"in the VR sim  ",
				"and input them ",
				"into the main  ",
				"terminal!      ",
				"Good luck!     "};
			scrollerSetString(scrollerText);
		}

		if (scrollerLoop(gameBuff) || (gameBuff->playerKeys.a && gameBuff->playerKeys.b))
		{
			deInitScroller();
			cryptoGameState->Scene += 1;
		}
		break;
	case 6: // Run around office
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
			// Start Timer
			initTime();
		}

		if (raycasterLoop(gameBuff))
		{
			cryptoGameState->Scene = 10;

			// Setup the times
			cryptoGameState->office_time = gameBuff->stage_time4 - gameBuff->stage_time1 - gameBuff->stage_time2 - gameBuff->stage_time3;
			cryptoGameState->vr1_time = gameBuff->stage_time1;
			cryptoGameState->vr2_time = gameBuff->stage_time2;
			cryptoGameState->vr3_time = gameBuff->stage_time3;
			cryptoGameState->total_time = gameBuff->stage_time4 + cryptoGameState->drive_time;

			// Setup the hi scores
			if (cryptoGameState->drive_time > 0 && cryptoGameState->drive_time < cryptoGameState->hi_drive_time) {
				cryptoGameState->hi_drive_time = cryptoGameState->drive_time;
			}
			if (cryptoGameState->office_time > 0 && cryptoGameState->office_time < cryptoGameState->hi_office_time) {
				cryptoGameState->hi_office_time = cryptoGameState->office_time;
			}
			if (cryptoGameState->vr1_time > 0 && cryptoGameState->vr1_time < cryptoGameState->hi_vr1_time) {
				cryptoGameState->hi_vr1_time = cryptoGameState->vr1_time;
			}
			if (cryptoGameState->vr2_time > 0 && cryptoGameState->vr2_time < cryptoGameState->hi_vr2_time) {
				cryptoGameState->hi_vr2_time = cryptoGameState->vr2_time;
			}
			if (cryptoGameState->vr3_time > 0 && cryptoGameState->vr3_time < cryptoGameState->hi_vr3_time) {
				cryptoGameState->hi_vr3_time = cryptoGameState->vr3_time;
			}
			if (cryptoGameState->total_time > 0 && cryptoGameState->total_time < cryptoGameState->hi_total_time) {
				cryptoGameState->hi_total_time = cryptoGameState->total_time;
			}

			cryptoSaveSettings(cryptoGameState);
		}
		break;
	case 10: // Win Story Scroller
		if (!gameBuff->achievementData->cryptoHunterFinish) {
			gameBuff->achievementData->cryptoHunterFinish = true;
			gameBuff->achievementData->newAchievment = true;
			saveAchievements(gameBuff->achievementData);
		}

		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
			scrollerLoop(gameBuff);

			char scrollerText[20][16] = {
				"You did it!    ",
				"You saved your ",
				"servers and    ",
				"defended your  ",
				"companies honor",
				"               ",
				"A true hero of ",
				"the modern age.",
				"               ",
				"Time to go rest",
				"until you are  ",
				"needed once    ",
				"more.          ",
				"               ",
				"What better way",
				"to relax than  ",
				"watching a talk",
				"               ",
				"bit.ly/2parl9p ",
				"BSidesCPT 2019 "};
			scrollerSetString(scrollerText);
		}

		if (scrollerLoop(gameBuff) || (gameBuff->playerKeys.a && gameBuff->playerKeys.b))
		{
			deInitScroller();
			cryptoGameState->Scene = 11;
		}
		break;
	case 11: // RotoZoomer
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
		}

		if (rotoLoop(gameBuff))
		{
			cryptoGameState->Scene = 12;
		}
		break;
	case 12: // Timed Leaderboard
		if (cryptoGameState->Scene != cryptoGameState->LastScene)
		{
			cryptoGameState->LastScene = cryptoGameState->Scene;
			cryptoGameState->selection = 0;
			displayClear(gameBuff, 0x00);
		}
		
		i = 0;
		drawString2x(gameBuff,(char *)"  Times (L) (R)",0,1 + 16 * i++,0xFF,0x00);
		drawString2x(gameBuff,(char *)"---------------",0,16 * i++,0x1C,0x00);
		char scores[20];
		sprintf(scores,"Drive :%3lus %3lus",cryptoGameState->hi_drive_time,cryptoGameState->remote_hi_drive_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);
		sprintf(scores,"Office:%3lus %3lus",cryptoGameState->hi_office_time,cryptoGameState->remote_hi_office_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);
		sprintf(scores,"VR1   :%3lus %3lus",cryptoGameState->hi_vr1_time,cryptoGameState->remote_hi_vr1_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);
		sprintf(scores,"VR2   :%3lus %3lus",cryptoGameState->hi_vr2_time,cryptoGameState->remote_hi_vr2_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);
		sprintf(scores,"VR3   :%3lus %3lus",cryptoGameState->hi_vr3_time,cryptoGameState->remote_hi_vr3_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);
		drawString2x(gameBuff,(char*)"---------------",0,16 * i++,0x1C,0x00);
		sprintf(scores,"Total :%3lus %3lus",cryptoGameState->hi_total_time,cryptoGameState->remote_hi_total_time);
		drawString2x(gameBuff,scores,0,16 * i++,0x1C,0x00);

		break;
	case 15:
		if (fadeToBlack(gameBuff))
		{
			cryptoGameState->Scene = cryptoGameState->LastScene;
			cryptoGameState->LastScene = 15;
		}
		break;
	}

	if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select)
	{
		freeCryptoGameState(cryptoGameState);
		cryptoGameState = nullptr;
		return true;
	}
	return false;
}
