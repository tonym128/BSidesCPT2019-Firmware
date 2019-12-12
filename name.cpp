#include "name.hpp"

struct NameState
{
    char *name;
    char *device;
    bool isSet;
    char *inputName;
    int inputChar;
    PlayerKeys playerKeys[5];
    int playerKeysIndex = 0;
};

int nameScreen = 0;

unsigned long debounceInputTime = 100;
unsigned long debounceInput = 0;

NameState *currentState = nullptr;

void SaveSettings(NameState *nameState)
{
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "name", nameState->name);
    json_object_set_string(root_object, "device", nameState->device);
    json_object_set_number(root_object, "isSet", nameState->isSet);
    char *nameString = json_serialize_to_string_pretty(root_value);

    gameSaveFile((char *)"Name.json", nameString);

    json_free_serialized_string(nameString);
    json_value_free(root_value);
}

NameState *LoadSettings()
{
    JSON_Value *user_data;
    char *fileData = gameLoadFile((char *)"Name.json");
    user_data = json_parse_string(fileData);
    free(fileData);

    if (user_data == nullptr)
        return nullptr;

    NameState *state = (NameState *)malloc(sizeof(struct NameState));
    state->name = (char *)json_object_get_string(json_object(user_data), "name");

    state->inputName = (char *)malloc(11 * sizeof(char));
    sprintf(state->inputName, "%s", state->name);

    state->device = (char *)json_object_get_string(json_object(user_data), "device");
    state->isSet = json_object_get_string(json_object(user_data), "isSet");
    state->playerKeysIndex = 0;
    return state;
}

void freeState()
{
    free(currentState->name);
    free(currentState->device);
    if (currentState->inputName != nullptr)
        free(currentState->inputName);

    free(currentState);
    currentState = nullptr;
}

void setState()
{
    if (currentState == nullptr)
    {
        currentState = LoadSettings();
        if (currentState != nullptr)
        {
            currentState->playerKeysIndex = 0;
        }
        else
        {
            currentState = (NameState *)malloc(sizeof(struct NameState));

            currentState->name = (char *)malloc(11 * sizeof(char));
            for (int i = 0; i < 10; i++)
            {
                currentState->name[i] = ' ';
            }
            currentState->name[10] = '\0';

            currentState->inputName = (char *)malloc(11 * sizeof(char));
            for (int i = 0; i < 10; i++)
            {
                currentState->inputName[i] = ' ';
            }
            currentState->inputName[10] = '\0';

            currentState->device = (char *)malloc(11 * sizeof(char));

            sprintf(currentState->device, "%s", getDevicePlatform());
            currentState->isSet = true;
            currentState->playerKeysIndex = 0;
            SaveSettings(currentState);
        }
    }
}

PlayerKeys inputBuffer(GameBuff *gameBuff)
{
    currentState->playerKeys[currentState->playerKeysIndex] = gameBuff->playerKeys;
    currentState->playerKeysIndex++;
    if (currentState->playerKeysIndex == 5)
        currentState->playerKeysIndex = 0;

    PlayerKeys playerKeys;
    playerKeys.a = currentState->playerKeys[0].a && currentState->playerKeys[1].a && currentState->playerKeys[2].a && currentState->playerKeys[3].a && currentState->playerKeys[4].a;
    playerKeys.b = currentState->playerKeys[0].b && currentState->playerKeys[1].b && currentState->playerKeys[2].b && currentState->playerKeys[3].b && currentState->playerKeys[4].b;
    playerKeys.start = currentState->playerKeys[0].start && currentState->playerKeys[1].start && currentState->playerKeys[2].start && currentState->playerKeys[3].start && currentState->playerKeys[4].start;
    playerKeys.select = currentState->playerKeys[0].select && currentState->playerKeys[1].select && currentState->playerKeys[2].select && currentState->playerKeys[3].select && currentState->playerKeys[4].select;
    playerKeys.up = currentState->playerKeys[0].up && currentState->playerKeys[1].up && currentState->playerKeys[2].up && currentState->playerKeys[3].up && currentState->playerKeys[4].up;
    playerKeys.down = currentState->playerKeys[0].down && currentState->playerKeys[1].down && currentState->playerKeys[2].down && currentState->playerKeys[3].down && currentState->playerKeys[4].down;
    playerKeys.left = currentState->playerKeys[0].left && currentState->playerKeys[1].left && currentState->playerKeys[2].left && currentState->playerKeys[3].left && currentState->playerKeys[4].left;
    playerKeys.right = currentState->playerKeys[0].right && currentState->playerKeys[1].right && currentState->playerKeys[2].right && currentState->playerKeys[3].right && currentState->playerKeys[4].right;

    return playerKeys;
}

void inputNameScreen(GameBuff *gameBuff)
{
    PlayerKeys playerKeys = inputBuffer(gameBuff);

    if (playerKeys.left && debounceInput < gameBuff->timeInMillis)
    {
        debounceInput = gameBuff->timeInMillis + debounceInputTime;
        currentState->inputChar -= 1;
        if (currentState->inputChar < 0)
            currentState->inputChar = 9;
    }
    if (playerKeys.right && debounceInput < gameBuff->timeInMillis)
    {
        debounceInput = gameBuff->timeInMillis + debounceInputTime;
        currentState->inputChar += 1;
        if (currentState->inputChar > 9)
            currentState->inputChar = 0;
    }
    if (playerKeys.up && debounceInput < gameBuff->timeInMillis)
    {
        debounceInput = gameBuff->timeInMillis + debounceInputTime;
        char value = currentState->inputName[currentState->inputChar] + 1;
        if (value > 126)
            value = 30;
        currentState->inputName[currentState->inputChar] = (char)value;
    }
    if (playerKeys.down && debounceInput < gameBuff->timeInMillis)
    {
        debounceInput = gameBuff->timeInMillis + debounceInputTime;
        char value = currentState->inputName[currentState->inputChar] - 1;
        if (value < 30)
            value = 126;
        currentState->inputName[currentState->inputChar] = (char)value;
    }
    if (playerKeys.a && debounceInput < gameBuff->timeInMillis)
    {
        debounceInput = gameBuff->timeInMillis + debounceInputTime;
        char value = currentState->inputName[currentState->inputChar];
        if (value >= 'a' && value <= 'z')
        {
            value -= 32;
        }
        else if (value >= 'A' && value <= 'Z')
        {
            value += 32;
        }
        currentState->inputName[currentState->inputChar] = (char)value;
    }

    drawString2x(gameBuff, currentState->inputName, 0, 0, 0xFF, 0x00);
    unsigned int currentCharValue = (int)currentState->inputName[currentState->inputChar];
    char currentChar[4];
    sprintf(currentChar, "%3d", currentCharValue);

    drawString2x(gameBuff, currentChar, 0, 32, 0xFF, 0x00);

    Dimensions dim;
    dim.height = 2;
    dim.width = 16;
    dim.x = 16 * currentState->inputChar;
    dim.y = 17;
    drawBlock(gameBuff, dim, 0xFF);

    if ((gameBuff->playerKeys.start || gameBuff->playerKeys.select) && gameBuff->playerKeys.debouncedInput)
    {
        nameScreen = 0;
        if (strcmp(currentState->inputName, currentState->name) != 0)
        {
            sprintf(currentState->name, "%s", currentState->inputName);
            currentState->isSet = 1;
            SaveSettings(currentState);

            if (!gameBuff->achievementData->setupName)
            {
                gameBuff->achievementData->setupName = true;
                gameBuff->achievementData->newAchievment = true;
                saveAchievements(gameBuff->achievementData);
            }
        }
    }
}

void showNameScreen(GameBuff *gameBuff)
{
    drawString2x(gameBuff, currentState->name, 0, 0, 0xFF, 0x00);
    drawString2x(gameBuff, currentState->device, 0, 16, 0xFF, 0x00);

    drawString(gameBuff, (char *)"BD Addr : ", 0, 56, 0xFF, 0x00);
    drawString(gameBuff, gameBuff->badgeState->bt_addr, 0, 64, 0xFF, 0x00);

    if (gameBuff->timeInMillis / 1000 % 2 == 0)
        drawString(gameBuff, (char *)"Press A or B to set name", 0, gameBuff->HEIGHT - 16, 0xFF, 0x00);

    if ((gameBuff->playerKeys.a || gameBuff->playerKeys.b) && gameBuff->playerKeys.debouncedInput)
    {
        nameScreen = 1;
        currentState->inputChar = 0;
    }
}

bool nameLoop(GameBuff *gameBuff)
{
    displayClear(gameBuff, 0x00);
    setState();
    switch (nameScreen)
    {
    case 0: // Show Name
        showNameScreen(gameBuff);
        break;
    case 1: // Input Name
        inputNameScreen(gameBuff);
        break;
    }

    if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select)
    {
        freeState();
        return true;
    }

    return false;
}
