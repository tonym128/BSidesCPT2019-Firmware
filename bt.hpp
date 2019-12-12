#ifndef BT_H
#define BT_H 1

#ifdef ESP32
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <BLEServer.h>

#endif

#include "game.hpp"

#ifndef MYFONT_H_
#include "myfont.hpp"
#endif //MYFONT_H_

#ifdef ESP32
#include <HTTPClient.h>
#endif

bool btLoop(GameBuff*);

#ifdef ESP32
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

#endif
void startBtScanTask(GameBuff *gameBuff);

#endif
