#define SERIAL_HACK 1

#include "game.hpp"

#include "headerimages/ESP32GameOn240x240.jpg.h"
#include "headerimages/ContrastSecLogo.jpg.h"
#include "headerimages/bSidesCapeTown.jpg.h"
#include "headerimages/achievement_unlocked.jpg.h"
#include "headerimages/achievement_unlocked_high.jpg.h"
#include "headerimages/fine.jpg.h"
#include "headerimages/TonyM.jpg.h"

static GameBuff *gameBuff = nullptr;
static byte buttonVals;
static std::array<int, 8> buttonRaw;

#ifdef SDL2_FOUND
#include "platform/game_sdl.h"
#elif ESP32
#include "platform/esp32.h"
#endif

bool displayImage(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
  int screenpixel;
  int i, j, k;
  int r, g, b;
  uint8_t colour;

  for (i = 0; i < w; i++)
  {
    for (j = 0; j < h; j++)
    {
      k = bitmap[i + j * w];

      r = bitExtracted(k, 5, 12);
      g = bitExtracted(k, 6, 6);
      b = bitExtracted(k, 5, 1);

      colour = MakeColor565(r, g, b);
      screenpixel = x + i + (y + j) * gameBuff->WIDTH;
      if (screenpixel >= 0 && screenpixel < gameBuff->MAXPIXEL)
        gameBuff->consoleBuffer[screenpixel] = colour;
    }
  }

  return true;
}

BadgeState *loadBadgeSettings()
{
  JSON_Value *user_data;
  char *fileData = gameLoadFile((char*)"Badge.json");
  user_data = json_parse_string(fileData);
  free(fileData);

  if (user_data == nullptr || user_data == NULL)
  {
    return nullptr;
  }

  BadgeState *state = (BadgeState *)malloc(sizeof(struct BadgeState));
  state->bt_addr = (char *)json_object_get_string(json_object(user_data), (char*)"bt_addr");

  state->ssid = (char *)json_object_get_string(json_object(user_data), (char*)"ssid");
  ssid = state->ssid;

  state->password = (char *)json_object_get_string(json_object(user_data), (char*)"password");
  password = state->password;

  if (json_object_has_value(json_object(user_data), (char*)"customBoot")) {
    state->customBoot = (bool)json_object_get_boolean(json_object(user_data), (char*)"customBoot");
    state->bootMenuItem = (int)json_object_get_number(json_object(user_data), (char*)"bootMenuItem");
  } else {
    state->customBoot = false;
    state->bootMenuItem = 0;
  }

  free(user_data);

  // Try to load Badge Name
  fileData = gameLoadFile((char*)"Name.json");
  user_data = json_parse_string(fileData);
  free(fileData);

  if (user_data != NULL)
  {
    state->name = (char *)json_object_get_string(json_object(user_data), (char*)"name");
    free(user_data);
  }
  else
  {
    state->name = (char *)DEVICE;
  }

  return state;
}

void saveBadgeSettings(BadgeState *state)
{
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  json_object_set_string(root_object, "bt_addr", state->bt_addr);
  json_object_set_string(root_object, "ssid", state->ssid);
  json_object_set_string(root_object, "password", state->password);
  json_object_set_boolean(root_object, "customBoot", state->customBoot);
  json_object_set_number(root_object, "bootMenuItem", (double)state->bootMenuItem);
  char *stateString = json_serialize_to_string_pretty(root_value);

  gameSaveFile((char*)"Badge.json", stateString);
  json_free_serialized_string(stateString);
  json_value_free(root_value);
}


AchievmentsData *loadAchievements() {
	JSON_Value *user_data;
	char *fileData = gameLoadFile((char*)"achievements.json");
	user_data = json_parse_string(fileData);
	free(fileData);

	if (user_data == nullptr)
		return nullptr;

	AchievmentsData *data = (AchievmentsData *)malloc(sizeof(AchievmentsData));

	data->asteroids300 = (bool)json_object_dotget_boolean(json_object(user_data), "asteroids300");
	data->asteroidsPlay = (bool)json_object_dotget_boolean(json_object(user_data), "asteroidsPlay");
	data->cryptoHunterFinish = (bool)json_object_dotget_boolean(json_object(user_data), "cryptoHunterFinish");
	data->cryptoHunterPlay = (bool)json_object_dotget_boolean(json_object(user_data), "cryptoHunterPlay");
	data->scheduleView = (bool)json_object_dotget_boolean(json_object(user_data), "scheduleView");
	data->setupName = (bool)json_object_dotget_boolean(json_object(user_data), "setupName");
	data->aboutView = (bool)json_object_dotget_boolean(json_object(user_data), "aboutView");
  data->newAchievment = false;
  
	return data;
}

bool saveAchievements(AchievmentsData *data) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_boolean(root_object, "asteroids300", data->asteroids300);
	json_object_set_boolean(root_object, "asteroidsPlay", data->asteroidsPlay);
	json_object_set_boolean(root_object, "cryptoHunterFinish", data->cryptoHunterFinish);
	json_object_set_boolean(root_object, "cryptoHunterPlay", data->cryptoHunterPlay);
	json_object_set_boolean(root_object, "scheduleView", data->scheduleView);
	json_object_set_boolean(root_object, "setupName", data->setupName);
	json_object_set_boolean(root_object, "aboutView", data->aboutView);

	char *nameString = json_serialize_to_string_pretty(root_value);

	gameSaveFile((char*)"achievements.json", nameString);

	json_free_serialized_string(nameString);
	json_value_free(root_value);

    return true;
}

void gameSetup()
{
  if (gameBuff == nullptr)
    gameBuff = new GameBuff();
	  gameBuff->gameMode = 0;
	  gameBuff->maxGameMode = 8;

	  gameBuff->enter = false;
  	gameBuff->fireRunning = true;
  	gameBuff->firstRun = true;

  if (gameBuff->badgeState == nullptr || gameBuff->badgeState == NULL)
  {
    gameBuff->badgeState = loadBadgeSettings();

    if (gameBuff->badgeState == nullptr || gameBuff->badgeState == NULL)
    {
      gameBuff->badgeState = (BadgeState *)malloc(sizeof(struct BadgeState));

      gameBuff->badgeState->bt_addr = (char *)malloc(20 * sizeof(char));
      initBLEDevice();
      printBLEDeviceAddress(gameBuff->badgeState->bt_addr);
      deinitBLEDevice();
      gameBuff->badgeState->ssid = ssid;
      gameBuff->badgeState->password = password;
      gameBuff->badgeState->customBoot = false;
      gameBuff->badgeState->bootMenuItem = 0;

      saveBadgeSettings(gameBuff->badgeState);
      gameBuff->badgeState = loadBadgeSettings();
      // It reboots after this if there isn't enough free memory to alloc the consoleBuffer, but that's fine it'll have the file on 2nd boot.
    }
  }

  if (gameBuff->achievementData == nullptr) {
      gameBuff->achievementData = loadAchievements();
      if (gameBuff->achievementData == nullptr)
      {
          gameBuff->achievementData = new AchievmentsData();
          saveAchievements(gameBuff->achievementData);
      }
  }

  gameBuff->playerKeys.debouncedInput = true;
  gameBuff->consoleBuffer = (uint8_t *)calloc(gameBuff->MAXPIXEL, sizeof(uint8_t));

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(displayImage);

  if (!gameBuff->badgeState->customBoot) {
    TJpgDec.drawJpg(0, 0, ESP32GameOn240x240_jpg, sizeof(ESP32GameOn240x240_jpg));
    sendToScreen();
    updateMinTime(1000);

    TJpgDec.drawJpg(0, 0, ContrastSecLogo_jpg, sizeof(ContrastSecLogo_jpg));
    sendToScreen();
    updateMinTime(1000);

    TJpgDec.drawJpg(0, 0, bSidesCapeTown_jpg, sizeof(bSidesCapeTown_jpg));
    sendToScreen();
    updateMinTime(1000);
  }

  displayClear(gameBuff, 0x00);

  if (gameBuff->badgeState->customBoot) {
    gameBuff->enter = true;
    gameBuff->gameMode = gameBuff->badgeState->bootMenuItem;
  }
}

void processInput(byte buttonVals)
{
  gameBuff->timeInMillis = getTimeInMillis();

  gameBuff->playerKeys.up = processKey(buttonVals, P1_Top);
  gameBuff->playerKeys.down = processKey(buttonVals, P1_Bottom);
  gameBuff->playerKeys.left = processKey(buttonVals, P1_Left);
  gameBuff->playerKeys.right = processKey(buttonVals, P1_Right);
  gameBuff->playerKeys.a = processKey(buttonVals, P2_Right);
  gameBuff->playerKeys.b = processKey(buttonVals, P2_Left);
  gameBuff->playerKeys.start = processKey(buttonVals, P2_Bottom);
  gameBuff->playerKeys.select = processKey(buttonVals, P2_Top);

  if (gameBuff->playerKeys.debouncedInput && buttonVals > 0) {
    gameBuff->playerKeys.debouncedInput = false;
    gameBuff->playerKeys.debounceTimeout = gameBuff->playerKeys.debounceDelay + gameBuff->timeInMillis;
  } else if (!gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.debounceTimeout < gameBuff->timeInMillis) {
    gameBuff->playerKeys.debouncedInput = true;
  }

  if (gameBuff->playerKeys.select || gameBuff->playerKeys.start)
  {
    if (gameBuff->playerKeys.up && gameBuff->playerKeys.down)
    {
      if (gameBuff->timeInMillis > esp32gameon_debounce)
      {
        esp32gameon_debug_fps = !esp32gameon_debug_fps;
        esp32gameon_debounce = gameBuff->timeInMillis + 1000;
      }
    }

    if (gameBuff->playerKeys.left && gameBuff->playerKeys.right)
    {
      if (gameBuff->timeInMillis > esp32gameon_debounce)
      {
        esp32gameon_debug_output = !esp32gameon_debug_output;
        esp32gameon_debounce = gameBuff->timeInMillis + 1000;
      }
    }

    if (gameBuff->playerKeys.a && gameBuff->playerKeys.b)
    {
      format();
    }
  }
}

void serialInput() {
#ifdef SERIAL_HACK
int counter = 0;
char text[11] = "";

#ifdef ESP32
  if (Serial.available()) {
    while (Serial.available() && counter < 10)
    {
      // Handle bytes reception
      text[counter] = Serial.read();
      ++counter;
      // Give time for data to come in
      delay(5); 
    }
#else
  if (false) {
#endif
    if (strcmp(text,"esp") == 0) {
      displayClear(gameBuff,0x00);
      TJpgDec.drawJpg(0, 0, ESP32GameOn240x240_jpg, sizeof(ESP32GameOn240x240_jpg));
      sendToScreen();
      updateMinTime(5000);
    } else 
    if (strcmp(text,"esp32") == 0) {
      displayClear(gameBuff,0x00);
      TJpgDec.drawJpg(0, 0, ESP32GameOn240x240_jpg, sizeof(ESP32GameOn240x240_jpg));
      sendToScreen();
      updateMinTime(5000);
    } else 
    if (strcmp(text,"bsides") == 0) {
      displayClear(gameBuff,0x00);
      TJpgDec.drawJpg(0, 0, bSidesCapeTown_jpg, sizeof(bSidesCapeTown_jpg));
      sendToScreen();
      updateMinTime(5000);
    } else
    if (strcmp(text,"contrast") == 0) {
      displayClear(gameBuff,0x00);
      TJpgDec.drawJpg(0, 0, ContrastSecLogo_jpg, sizeof(ContrastSecLogo_jpg));
      sendToScreen();
      updateMinTime(5000);
    } else
    if (strcmp(text,"tony") == 0) {
      displayClear(gameBuff,0x00);
      TJpgDec.drawJpg(0, 0, TonyM_jpg, sizeof(TonyM_jpg));
      drawScreenDouble(gameBuff);
      drawScreenDouble(gameBuff);
      sendToScreen();
      updateMinTime(5000);
    } else
    if (strcmp(text,"unlock") == 0) {
      gameBuff->achievementData->aboutView = true;
      gameBuff->achievementData->asteroids300 = true;
      gameBuff->achievementData->asteroidsPlay = true;
      gameBuff->achievementData->cryptoHunterFinish = true;
      gameBuff->achievementData->cryptoHunterPlay = true;
      gameBuff->achievementData->scheduleView = true;
      gameBuff->achievementData->setupName = true;

      gameBuff->achievementData->newAchievment = true;
      saveAchievements(gameBuff->achievementData);
    } else
    if (strcmp(text,"lock") == 0) {
      gameBuff->achievementData->aboutView = false;
      gameBuff->achievementData->asteroids300 = false;
      gameBuff->achievementData->asteroidsPlay = false;
      gameBuff->achievementData->cryptoHunterFinish = false;
      gameBuff->achievementData->cryptoHunterPlay = false;
      gameBuff->achievementData->scheduleView = false;
      gameBuff->achievementData->setupName = false;

      gameBuff->achievementData->newAchievment = true;
      saveAchievements(gameBuff->achievementData);
    } else
    if (strcmp(text,"rotate") == 0) {
      gameBuff->rotate = !gameBuff->rotate;
    } else
    if (strcmp(text,"debug") == 0) {
      esp32gameon_debug_fps = !esp32gameon_debug_fps;
      esp32gameon_debug_output = !esp32gameon_debug_output;
    } else
    if (strcmp(text,"fps") == 0) {
      esp32gameon_debug_fps_serial = !esp32gameon_debug_fps_serial;
    } else
    if (strcmp(text,"up") == 0) {
      gameBuff->playerKeys.up = true;
    } else
    if (strcmp(text,"down") == 0) {
      gameBuff->playerKeys.down = true;
    } else
    if (strcmp(text,"left") == 0) {
      gameBuff->playerKeys.left = true;
    } else
    if (strcmp(text,"right") == 0) {
      gameBuff->playerKeys.right = true;
    } else
    if (strcmp(text,"a") == 0) {
      gameBuff->playerKeys.a = true;
    } else
    if (strcmp(text,"b") == 0) {
      gameBuff->playerKeys.b = true;
    } else
    if (strcmp(text,"start") == 0) {
      gameBuff->playerKeys.start = true;
    } else
    if (strcmp(text,"select") == 0) {
      gameBuff->playerKeys.select = true;
    }
  }
#endif
}

void gameLoop()
{
  // put your main code here, to run repeatedly:
  processInput(getReadShift());
  serialInput();

  if (myGameLoop(gameBuff))
  {
    gameSetup();
  }

  calcFPS();
  if (gameBuff->consoleBuffer != nullptr)
  {
    if (esp32gameon_debug_fps)
    {
#ifdef ESP32
      if (!heap_caps_check_integrity_all(true))
      {
        Dimensions dimRed;
        dimRed.x = 0;
        dimRed.y = 0;
        dimRed.width = 30;
        dimRed.height = 30;

        drawBlock(gameBuff, dimRed, 0xE0);
        drawString(gameBuff,(char*)"heap!",0,0,0xFF,0);
      }
#endif

      drawFPS(gameBuff);
    }

#ifdef ESP32
    if (esp32gameon_debug_fps_serial) {
      Serial.println(currentFPS());
    }
#endif

    if (esp32gameon_debug_output)
    {
      std::array<int, 8> rawValues = getRawInput();
      byte readShift = getReadShift();
      for (int i = 0; i < 8; i += 1)
      {
        char fpsString[17];
        sprintf(fpsString, "%d:%03d", i, rawValues[i]);
        bool pressed = (readShift >> i) & 1;
        drawString(gameBuff, fpsString, 0, i * 8, pressed ? 0x80 : 0xFF, -1);
      }
    }

    if (gameBuff->achievementData->newAchievment) {
      TJpgDec.drawJpg(0,0,achievement_unlocked_high_jpg,sizeof(achievement_unlocked_high_jpg));
      gameBuff->achievementData->newAchievment = false;
      sendToScreen();
      updateMinTime(2000);
    }

    sendToScreen();
    updateMinTime(33);
  }
}
