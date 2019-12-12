#include "bt.hpp"
#include "headerimages/gh/gh_american.jpg.h"
#include "headerimages/gh/gh_anser.jpg.h"
#include "headerimages/gh/gh_barnacle.jpg.h"
#include "headerimages/gh/gh_chinese.jpg.h"
#include "headerimages/gh/gh_cotton.jpg.h"
#include "headerimages/gh/gh_dansk.jpg.h"
#include "headerimages/gh/gh_embden.jpg.h"
#include "headerimages/gh/gh_oie.jpg.h"
#include "headerimages/gh/gh_pokeri.jpg.h"
#include "headerimages/gh/gh_pomm.jpg.h"
#include "headerimages/gh/gh_roman.jpg.h"
#include "headerimages/gh/gh_whiteseb.jpg.h"
#include "headerimages/gh/gh_goosen.jpg.h"


#ifdef ESP32
void MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
  Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
  //advertisedDevice.toString().c_str();
}
#endif

#define SERVICE_UUID "fff7d507-93d5-4b90-bed3-c730a28c9e33"
#define CHARACTERISTIC_UUID "3587bc19-07f7-4766-8dd2-345c8f0404a1"

#define SCAN_NAME_SIZE (20)

#ifdef ESP32
BLEScan *pBLEScan = nullptr;
#endif

int hapticPin = 35;

unsigned long btFrameTime_last = 0;

bool initup = false;
bool scanStarted = false;

bool btLoop(GameBuff *gameBuff)
{
  if (!initup)
  {
    initup = true;
    int i = 0;
    displayClear(gameBuff, 0x00);
    drawString(gameBuff, (char *)"Wifi ON!", 0, 8 * i++, 0xFF, 0);
#ifdef ESP32
    Serial.println("Scanning...");
    pinMode(hapticPin, OUTPUT);
    digitalWrite(hapticPin, HIGH);

    Serial.println("D");
    BLEDevice::init("Badge McBadgeFace");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value
/*
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    Serial.println("C");
    pCharacteristic->setValue("Hello World says Neil");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    
    
    BLEDevice::startAdvertising();
*/
#endif

    btFrameTime_last = gameBuff->timeInMillis;
  }

  if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select)
  {
    return true;
  }

  if (gameBuff->timeInMillis - btFrameTime_last > 3000)
  {
    btFrameTime_last = gameBuff->timeInMillis;

#ifdef ESP32
    Serial.print("MAIN Executing on core ");
    Serial.println(xPortGetCoreID());
#endif
  }

  displayClear(gameBuff, 0x00);
  //drawString(gameBuff, (char*)"BLUETOOTH", 0, 0, 0xFF, 0);

#ifdef ESP32
    BLEScanResults foundDevices = pBLEScan->start(1, false);
    Serial.println("found device count:");
    Serial.println(foundDevices.getCount());

    int withNameCount = 0;
    for(int i = 0; i < foundDevices.getCount(); i++) {
      if(foundDevices.getDevice(i).haveName()) {
        withNameCount++;
      }
    }
/*
fEdvb3NlfExlZ2VufGlkWSBm
fCBpcyBsfGQgaXMgfHJwak1t
fG9vc2UgfEBmbHVmfGUgUFhv
fFRoZXkgfGZ5cG9ufCBRUSBq
fGhhdmUgfHkgbG9zfENyIFVh
fHRoZSBrfHQgaGlzfCB0aCBh
fGV5cyBGfCBwdnQgfHVNSWJl
fGluZCB0fGtleXMgfCBwSk0g
fGhlbSBhfGluIGJvfFdhZWxr
fG5kIGRlfGF0aW5nfCBkYWdi
fHN0cm95fCBhY2NpfG91ciBY
fCB0aGVtfGRlbnQgfGZjVWFp
*/

    int threshold = -55;
    
    for (int j = 0; j < foundDevices.getCount(); j++)
    {
      BLEAdvertisedDevice foundDevice = foundDevices.getDevice(j);

      if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "b4:e6:2d:9d:bb:07", 17) == 0)) {
        Serial.println("Roman");
        TJpgDec.drawJpg(0, 0, gh_roman_jpg, sizeof(gh_roman_jpg));
        drawString(gameBuff, (char *)"fEdvb3NlfExlZ2VufGlkWSBm", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);
        
        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "b4:e6:2d:9d:bf:e3", 17) == 0)) {
        Serial.println("CottonPatch");      
        TJpgDec.drawJpg(0, 0, gh_cotton_jpg, sizeof(gh_cotton_jpg));
        drawString(gameBuff, (char *)"fCBpcyBsfGQgaXMgfHJwak1t", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "a4:cf:12:6a:4e:4a", 17) == 0)) {
        Serial.println("Toulouse");      

        TJpgDec.drawJpg(0, 0, gh_oie_jpg, sizeof(gh_oie_jpg));
        drawString(gameBuff, (char *)"fG9vc2UgfEBmbHVmfGUgUFhv", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:16:53:4a", 17) == 0)) {
        Serial.println("AmericanBuff");      
        TJpgDec.drawJpg(0, 0, gh_american_jpg, sizeof(gh_american_jpg));
        drawString(gameBuff, (char *)"fFRoZXkgfGZ5cG9ufCBRUSBq", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "a4:cf:12:45:cc:5e", 17) == 0)) {
        Serial.println("Emden");      
        TJpgDec.drawJpg(0, 0, gh_embden_jpg, sizeof(gh_embden_jpg));
        drawString(gameBuff, (char *)"fGhhdmUgfHkgbG9zfENyIFVh", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:16:d8:ea", 17) == 0)) {
        Serial.println("Faroese");      
        TJpgDec.drawJpg(0, 0, gh_pokeri_jpg, sizeof(gh_pokeri_jpg));
        drawString(gameBuff, (char *)"fHRoZSBrfHQgaGlzfCB0aCBh", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:16:df:76", 17) == 0)) {
        Serial.println("Pomeranian");      
        TJpgDec.drawJpg(0, 0, gh_pomm_jpg, sizeof(gh_pomm_jpg));
        drawString(gameBuff, (char *)"fGV5cyBGfCBwdnQgfHVNSWJl", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "a4:cf:12:45:d8:4e", 17) == 0)) {
        Serial.println("Sebastopol");      
        TJpgDec.drawJpg(0, 0, gh_whiteseb_jpg, sizeof(gh_whiteseb_jpg));
        drawString(gameBuff, (char *)"fGluZCB0fGtleXMgfCBwSk0g", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:18:23:d6", 17) == 0)) {
        Serial.println("Barnacle");      
        TJpgDec.drawJpg(0, 0, gh_barnacle_jpg, sizeof(gh_barnacle_jpg));
        drawString(gameBuff, (char *)"fGhlbSBhfGluIGJvfFdhZWxr", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:18:22:ba", 17) == 0)) {
        Serial.println("Chinese");      
        TJpgDec.drawJpg(0, 0, gh_chinese_jpg, sizeof(gh_chinese_jpg));
        drawString(gameBuff, (char *)"fG5kIGRlfGF0aW5nfCBkYWdi", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:18:26:ca", 17) == 0)) {
        Serial.println("Landrace");      

        TJpgDec.drawJpg(0, 0, gh_dansk_jpg, sizeof(gh_dansk_jpg));
        drawString(gameBuff, (char *)"fHN0cm95fCBhY2NpfG91ciBY", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:16:56:56", 17) == 0)) {
        Serial.println("PinkFooted");      
        TJpgDec.drawJpg(0, 0, gh_anser_jpg, sizeof(gh_anser_jpg));
        drawString(gameBuff, (char *)"fCB0aGVtfGRlbnQgfGZjVWFp", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      } else if((foundDevice.getRSSI() > threshold) && (strncmp(foundDevice.getAddress().toString().c_str(), "24:6f:28:16:d9:3e", 17) == 0)) {
        Serial.println("Goosen");
        TJpgDec.drawJpg(0, 0, gh_goosen_jpg, sizeof(gh_goosen_jpg));
        drawString(gameBuff, (char *)"The Golden Gooose", 0, 8, 0xFF, 0);
        drawString(gameBuff, (char *)foundDevice.getName().c_str(), 0, 220, 0xFF, 0);

        return false;
      }
    }      


    drawString(gameBuff, (char *)": WHICH GOOSE?", 0, 8, 0xFF, 0);


    
    if (withNameCount > 0)
    {
      int y = 18;
      for (int i = 0; i < foundDevices.getCount(); i++)
      {
        char scanDisplayText[40];

        BLEAdvertisedDevice foundDevice = foundDevices.getDevice(i);

        if (!foundDevice.haveName())
        {
          continue;
        }

        sprintf(scanDisplayText, "%s (%d)\n", foundDevice.getName().c_str(), foundDevice.getRSSI());
        
        /*
        else
        {
          sprintf(scanDisplayText, "%s (%d)\n", foundDevice.getAddress().toString().c_str(), foundDevice.getRSSI());
        }
        */

        int textColour = 0xFF;

        drawString(gameBuff, scanDisplayText, 10, y, textColour, 0);
        y += 8;
      }
    }
    else
    {
      drawString2x(gameBuff, "NO GOOSE EGGS", 20, 120, 0xFF, 0);
    }
#endif

  return false;
}
