#include <TFT_eSPI.h>

#define BLE_GAMEPAD 1
#ifdef BLE_GAMEPAD
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEHIDDevice.h>
#include <BLE2902.h>
#endif

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "esp_bt_main.h"
#include "esp_bt_device.h"

#ifdef BLE_GAMEPAD
static BLEHIDDevice *pHID;
BLEServer *pServer;
BLECharacteristic *input;
bool deviceConnected = false;
bool oldDeviceConnected = false;
#endif

uint8_t *GPIO_axis;
uint8_t *GPIO_buttons;

#include <WiFi.h>
#include <FS.h> // Include the SPIFFS library
#include "SPIFFS.h"

static const byte TOUCH_SENSE_MAX = 50;
static const byte TOUCH_SENSE_MIN = 20;
int inputVal = 0;
static std::array<int, 8> TOUCH_SENSE;
static const char *DEVICE = (char *)"ESP32";

TFT_eSPI tft = TFT_eSPI(); // Invoke library

/* BT Addr */
bool initBLEDevice()
{
  Serial.println("Starting to initialize controller");
  if (!btStart())
  {
    Serial.println("Failed to initialize controller");
    return false;
  }

  if (esp_bluedroid_init() != ESP_OK)
  {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }

  if (esp_bluedroid_enable() != ESP_OK)
  {
    Serial.println("Failed to enable bluedroid");
    return false;
  }

  Serial.println("Completed initializing controller");
  return true;
}

bool deinitBLEDevice()
{
  Serial.println("Start to deinitialize controller");
  if (!btStop())
  {
    Serial.println("Failed to deinitialize controller");
    return false;
  }

  if (esp_bluedroid_disable() != ESP_OK)
  {
    Serial.println("Failed to disable bluedroid");
    return false;
  }

  if (esp_bluedroid_deinit() != ESP_OK)
  {
    Serial.println("Failed to deinit bluedroid");
    return false;
  }

  Serial.println("Completed deinitializing controller");
  return true;
}

void printBLEDeviceAddress(char *bd_addr)
{
  Serial.println("Get BT Addr");
  const uint8_t *point = esp_bt_dev_get_address();
  sprintf(bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X", (int)point[0], (int)point[1], (int)point[2], (int)point[3], (int)point[4], (int)point[5]);
  Serial.println("Got BT Addr");
}

/* End of BT Addr */
#ifdef BLE_GAMEPAD
struct gamepad_report_t
{
  uint16_t buttons;
  int16_t left_x;
  int16_t left_y;
};

bool operator!=(const gamepad_report_t &lhs, const gamepad_report_t &rhs)
{
  return (lhs.left_x != rhs.left_x) || (lhs.left_y != rhs.left_y) || (lhs.buttons != rhs.buttons);
}

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Connected");
    deviceConnected = true;
    // workaround after reconnect (see comment below)
    BLE2902 *desc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Disconnected");
    BLE2902 *desc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
    deviceConnected = false;
  }
};

gamepad_report_t oldValue, newValue;

bool setupBLE()
{
  Serial.println("Starting BLE");
  char *fileData = gameLoadFile((char *)"Name.json");
  char *nameString;

  if (fileData != nullptr)
  {
    JSON_Value *user_data;
    user_data = json_parse_string(fileData);
    free(fileData);

    if (user_data == nullptr)
    {
      nameString = (char *)malloc(11 * sizeof(char));
      sprintf(nameString, "%s-BsidesGst", getDevicePlatform());
    }
    else
    {
      nameString = (char *)json_object_get_string(json_object(user_data), "name");
    }
  }
  else
  {
    nameString = (char *)malloc(11 * sizeof(char));
    sprintf(nameString, "%s-BsidesGst", getDevicePlatform());
  }

  Serial.println("Loaded Name");
  Serial.println(nameString);

  BLEDevice::init(nameString);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Instantiate HID Device
  pHID = new BLEHIDDevice(pServer);
  input = pHID->inputReport(1);

  pHID->manufacturer()->setValue("BSIDESCPT2019");

  pHID->pnp(0x02, 0x0810, 0xe501, 0x0106);
  pHID->hidInfo(0x00, 0x01);

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // Set Report Map
  const uint8_t reportMap[] = {
      0x05, 0x01,       /* USAGE_PAGE (Generic Desktop)       */
      0x09, 0x05,       /* USAGE (Game Pad)                   */
      0xa1, 0x01,       /* COLLECTION (Application)           */
      0xa1, 0x03,       /*   COLLECTION (Report)              */
      0x85, 0x01,       /*     REPORT_ID (1)                  */
      0xa1, 0x00,       /*     COLLECTION (Physical)          */
      0x05, 0x01,       /*       USAGE_PAGE (Generic Desktop) */
      0x09, 0x30,       /*       USAGE (X)                    */
      0x09, 0x31,       /*       USAGE (Y)                    */
      0x16, 0x00, 0x80, //  LOGICAL_MINIMUM (-32768)
      0x26, 0xFF, 0x7F, //  LOGICAL_MAXIMUM(32767)
      0x36, 0x00, 0x80, //  PHYSICAL_MINIMUM(-32768)
      0x46, 0xFF, 0x7F, //  PHYSICAL_MAXIMUM(32767)

      0x75, 0x10, //  REPORT_SIZE(16)
      0x95, 0x02, /*       REPORT_COUNT (2)             */
      0x81, 0x02, /*       INPUT (Data,Var,Abs)         */
      0xc0,       /*     END_COLLECTION                 */
      0x05, 0x09, /*     USAGE_PAGE (Button)            */
      0x19, 0x01, /*     USAGE_MINIMUM (Button 1)       */
      0x29, 0x0e, /*     USAGE_MAXIMUM (Button 14)      */
      0x15, 0x00, /*     LOGICAL_MINIMUM (0)            */
      0x25, 0x01, /*     LOGICAL_MAXIMUM (1)            */
      0x95, 0x0e, /*     REPORT_COUNT (14)              */
      0x75, 0x01, /*     REPORT_SIZE (1)                */
      0x81, 0x02, /*     INPUT (Data,Var,Abs)           */
      0x95, 0x01, /*     REPORT_COUNT (1)               */
      0x09, 0x3d, /*     USAGE (Start)                    */
      0x15, 0x00, /*     LOGICAL_MINIMUM (0)            */
      0x25, 0x01, /*     LOGICAL_MAXIMUM (1)            */
      0x75, 0x02, /*     REPORT_SIZE (1)                */
      0x81, 0x02, /*     INPUT (Data,Var,Abs)           */
      0x09, 0x3e, /*     USAGE (Select)                    */
      0x15, 0x00, /*     LOGICAL_MINIMUM (0)            */
      0x25, 0x01, /*     LOGICAL_MAXIMUM (1)            */
      0x75, 0x02, /*     REPORT_SIZE (1)                */
      0x81, 0x02, /*     INPUT (Data,Var,Abs)           */
      0xc0,       /*   END_COLLECTION                   */
      0xc0        /* END_COLLECTION                     */
  };

  pHID->reportMap((uint8_t *)reportMap, sizeof(reportMap));

  // Start the service
  pHID->startServices();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(pHID->hidService()->getUUID());
  pAdvertising->start();

  Serial.println("waiting 1 sec");
  delay(1000);

  Serial.println("Waiting a client connection to notify...");
  return true;
}

bool startBLEKeyboard()
{
  setupBLE();
  return true;
}

bool processBLEKeyboard(PlayerKeys pkeys)
{
  if (deviceConnected)
  {
    // AXIS
    newValue.left_y = 0;
    newValue.left_x = 0;

    if (pkeys.up)
      newValue.left_y = -32768;
    if (pkeys.right)
      newValue.left_x = 32767;
    if (pkeys.down)
      newValue.left_y = 32767;
    if (pkeys.left)
      newValue.left_x = -32768;

    // BUTTONS
    newValue.buttons = 0;

    if (pkeys.a)
      newValue.buttons |= (1 << 0);
    if (pkeys.b)
      newValue.buttons |= (1 << 1);
    if (pkeys.select)
      newValue.buttons |= (1 << 13);
    if (pkeys.start)
      newValue.buttons |= (1 << 14);

    if (newValue != oldValue)
    {
      // Debug
      // Serial.println("---");
      // if (newValue.left_x != oldValue.left_x)
      // {
      //   Serial.print("X Axis: ");
      //   Serial.println(newValue.left_x, HEX);
      // }
      // if (newValue.left_y != oldValue.left_y)
      // {
      //   Serial.print("Y Axis: ");
      //   Serial.println(newValue.left_y, HEX);
      // }
      // if (newValue.buttons != oldValue.buttons)
      // {
      //   Serial.print("Button Input: ");
      //   Serial.println(newValue.buttons, BIN);
      // }

      uint8_t a[] = {newValue.left_x, newValue.left_x >> 8, newValue.left_y, newValue.left_y >> 8, newValue.buttons, newValue.buttons >> 8};
      input->setValue(a, sizeof(a));
      input->notify();
      oldValue = newValue;
    }
  }

  // Connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
  }

  // Disconnecting

  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();
    Serial.println("restart advertising");
    oldDeviceConnected = deviceConnected;
  }

  delay(5);
  return true;
}

bool stopBLEKeyboard()
{
  BLEDevice::deinit(true);
  return true;
}
#else
bool setupBLE()
{
  return true;
}

bool startBLEKeyboard()
{
  return true;
}

bool processBLEKeyboard(PlayerKeys pkeys)
{
  return true;
}

bool stopBLEKeyboard()
{
  return true;
}

#endif

bool readAnalogSensor(int pin, int touch_sense)
{
  inputVal = touchRead(pin);
  return inputVal < touch_sense && inputVal > 0;
}

int readAnalogSensorRaw(int pin)
{
  inputVal = touchRead(pin);
  return inputVal;
}

byte getReadShiftAnalog()
{
  byte buttonVals = 0;
  buttonVals = buttonVals | (readAnalogSensor(32, TOUCH_SENSE[0]) << P1_Left);
  buttonVals = buttonVals | (readAnalogSensor(14, TOUCH_SENSE[1]) << P1_Top);
  buttonVals = buttonVals | (readAnalogSensor(27, TOUCH_SENSE[2]) << P1_Right);
  buttonVals = buttonVals | (readAnalogSensor(33, TOUCH_SENSE[3]) << P1_Bottom);

  buttonVals = buttonVals | (readAnalogSensor(15, TOUCH_SENSE[4]) << P2_Right);
  buttonVals = buttonVals | (readAnalogSensor(13, TOUCH_SENSE[5]) << P2_Bottom);
  buttonVals = buttonVals | (readAnalogSensor(2, TOUCH_SENSE[6]) << P2_Left);
  buttonVals = buttonVals | (readAnalogSensor(12, TOUCH_SENSE[7]) << P2_Top);

  return buttonVals;
}

std::array<int, 8> getRawInput()
{
  std::array<int, 8> rawValues;
  for (int i = 0; i < 8; i++)
  {
    rawValues[i] = 0;
  }

  int i = 0;
  rawValues[i++] = readAnalogSensorRaw(32); // Left
  rawValues[i++] = readAnalogSensorRaw(14); // Up
  rawValues[i++] = readAnalogSensorRaw(27); // Right
  rawValues[i++] = readAnalogSensorRaw(33); // Down
  rawValues[i++] = readAnalogSensorRaw(15); // A
  rawValues[i++] = readAnalogSensorRaw(13); // Start
  rawValues[i++] = readAnalogSensorRaw(2);  // B
  rawValues[i++] = readAnalogSensorRaw(12); // Select

  return rawValues;
}

byte getReadShift()
{
  return getReadShiftAnalog();
}

const static unsigned int RGB332to565lookupTable[256] = {
    0x0000, 0x000a, 0x0015, 0x001f, 0x0120, 0x012a, 0x0135, 0x013f,
    0x0240, 0x024a, 0x0255, 0x025f, 0x0360, 0x036a, 0x0375, 0x037f,
    0x0480, 0x048a, 0x0495, 0x049f, 0x05a0, 0x05aa, 0x05b5, 0x05bf,
    0x06c0, 0x06ca, 0x06d5, 0x06df, 0x07e0, 0x07ea, 0x07f5, 0x07ff,
    0x2000, 0x200a, 0x2015, 0x201f, 0x2120, 0x212a, 0x2135, 0x213f,
    0x2240, 0x224a, 0x2255, 0x225f, 0x2360, 0x236a, 0x2375, 0x237f,
    0x2480, 0x248a, 0x2495, 0x249f, 0x25a0, 0x25aa, 0x25b5, 0x25bf,
    0x26c0, 0x26ca, 0x26d5, 0x26df, 0x27e0, 0x27ea, 0x27f5, 0x27ff,
    0x4800, 0x480a, 0x4815, 0x481f, 0x4920, 0x492a, 0x4935, 0x493f,
    0x4a40, 0x4a4a, 0x4a55, 0x4a5f, 0x4b60, 0x4b6a, 0x4b75, 0x4b7f,
    0x4c80, 0x4c8a, 0x4c95, 0x4c9f, 0x4da0, 0x4daa, 0x4db5, 0x4dbf,
    0x4ec0, 0x4eca, 0x4ed5, 0x4edf, 0x4fe0, 0x4fea, 0x4ff5, 0x4fff,
    0x6800, 0x680a, 0x6815, 0x681f, 0x6920, 0x692a, 0x6935, 0x693f,
    0x6a40, 0x6a4a, 0x6a55, 0x6a5f, 0x6b60, 0x6b6a, 0x6b75, 0x6b7f,
    0x6c80, 0x6c8a, 0x6c95, 0x6c9f, 0x6da0, 0x6daa, 0x6db5, 0x6dbf,
    0x6ec0, 0x6eca, 0x6ed5, 0x6edf, 0x6fe0, 0x6fea, 0x6ff5, 0x6fff,
    0x9000, 0x900a, 0x9015, 0x901f, 0x9120, 0x912a, 0x9135, 0x913f,
    0x9240, 0x924a, 0x9255, 0x925f, 0x9360, 0x936a, 0x9375, 0x937f,
    0x9480, 0x948a, 0x9495, 0x949f, 0x95a0, 0x95aa, 0x95b5, 0x95bf,
    0x96c0, 0x96ca, 0x96d5, 0x96df, 0x97e0, 0x97ea, 0x97f5, 0x97ff,
    0xb000, 0xb00a, 0xb015, 0xb01f, 0xb120, 0xb12a, 0xb135, 0xb13f,
    0xb240, 0xb24a, 0xb255, 0xb25f, 0xb360, 0xb36a, 0xb375, 0xb37f,
    0xb480, 0xb48a, 0xb495, 0xb49f, 0xb5a0, 0xb5aa, 0xb5b5, 0xb5bf,
    0xb6c0, 0xb6ca, 0xb6d5, 0xb6df, 0xb7e0, 0xb7ea, 0xb7f5, 0xb7ff,
    0xd800, 0xd80a, 0xd815, 0xd81f, 0xd920, 0xd92a, 0xd935, 0xd93f,
    0xda40, 0xda4a, 0xda55, 0xda5f, 0xdb60, 0xdb6a, 0xdb75, 0xdb7f,
    0xdc80, 0xdc8a, 0xdc95, 0xdc9f, 0xdda0, 0xddaa, 0xddb5, 0xddbf,
    0xdec0, 0xdeca, 0xded5, 0xdedf, 0xdfe0, 0xdfea, 0xdff5, 0xdfff,
    0xf800, 0xf80a, 0xf815, 0xf81f, 0xf920, 0xf92a, 0xf935, 0xf93f,
    0xfa40, 0xfa4a, 0xfa55, 0xfa5f, 0xfb60, 0xfb6a, 0xfb75, 0xfb7f,
    0xfc80, 0xfc8a, 0xfc95, 0xfc9f, 0xfda0, 0xfdaa, 0xfdb5, 0xfdbf,
    0xfec0, 0xfeca, 0xfed5, 0xfedf, 0xffe0, 0xffea, 0xfff5, 0xffff};

static void sendToScreen()
{
  int colour = -1;
  int counter = 0;

  tft.startWrite();
  tft.setAddrWindow(0, 0, gameBuff->WIDTH, gameBuff->HEIGHT);

  if (!gameBuff->rotate)
  {
    for (int i = 0; i < gameBuff->WIDTH * gameBuff->HEIGHT;)
    {
      counter = 1;

      colour = gameBuff->consoleBuffer[i];
      while (colour == gameBuff->consoleBuffer[i + counter])
      {
        counter++;
      }

      tft.writeColor(RGB332to565lookupTable[colour], counter);
      i += counter;
    }
  }
  else
  {
    counter = 0;
    for (int x = gameBuff->WIDTH - 1; x >= 0; x--)
    {
      for (int y = 0; y < gameBuff->HEIGHT; y++)
      {
        if (counter > 0 && colour != gameBuff->consoleBuffer[x + y * gameBuff->WIDTH])
        {
          tft.writeColor(RGB332to565lookupTable[colour], counter);
          counter = 1;
          colour = gameBuff->consoleBuffer[x + y * gameBuff->WIDTH];
        }
        else
        {
          counter++;
        }
      }
    }
    tft.writeColor(RGB332to565lookupTable[colour], counter);
  }
  tft.endWrite();
}

void format()
{
  // When in doubt... format!
  Serial.println("Format SPIFFS");
  SPIFFS.format();
}

void gameInit()
{
  Serial.println("gameInit");
  // setupBLE();
  Serial.println("Screen Init");

  tft.begin();            // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.fillScreen(TFT_BLACK);

  Serial.println("Input Init");
  TOUCH_SENSE = getRawInput();
  for (int i = 0; i < 8; i++)
  {
    TOUCH_SENSE[i] /= 2;
    if (TOUCH_SENSE[i] > TOUCH_SENSE_MAX)
      TOUCH_SENSE[i] = TOUCH_SENSE_MAX;
    if (TOUCH_SENSE[i] < TOUCH_SENSE_MIN)
      TOUCH_SENSE[i] = TOUCH_SENSE_MIN;
    Serial.print("Touch Sense : ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(TOUCH_SENSE[i]);
  }

  Serial.println("SPIFFS Init");
  if (!SPIFFS.begin(true))
  {
    Serial.println("Mount Failed");
    SPIFFS.format();
    Serial.println("File formatted");
    return;
  }

  Serial.println("File system mounted");
}

char *gameLoadFile(char *fileName)
{
  Serial.print("Loading file : ");
  Serial.println(fileName);
  char *prepend = (char *)malloc(strlen(fileName) + 2);
  sprintf(prepend, "/%s", fileName);
  fs::File file = SPIFFS.open(prepend, "r");
  free(prepend);

  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return nullptr;
  }

  if (file.size() == 0)
  {
    Serial.println("Empty file");
    return nullptr;
  }

  char *fileData = (char *)malloc(file.size() + 1);
  file.readBytes(fileData, file.size());
  fileData[file.size()] = '\0';
  return fileData;
}

bool gameSaveFile(char *fileName, char *data)
{
  Serial.print("Saving file :");
  Serial.println(fileName);
  char *prepend = (char *)malloc(strlen(fileName) + 2);
  sprintf(prepend, "/%s", fileName);
  fs::File file = SPIFFS.open(prepend, FILE_WRITE);
  free(prepend);

  if (!file)
  {
    Serial.println("There was an error opening the file for writing");
    return false;
  }

  if (file.print(data))
  {
    Serial.println("File was written");
  }
  else
  {
    Serial.println("File write failed");
    return false;
  }

  file.close();
  return true;
}

static double getBatteryVoltage()
{
  return (double)analogRead(34) / 4192.0 * 2.0 * 3.3 + 0.4; //
  //                                ADC range // voltage divider // scale to vcc // Eh, not sure why but it's 0.4v too low
}

const char *getDevicePlatform()
{
  return DEVICE;
}
