#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <BLEServer.h>

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

/*
 * 
 */
#define SERVICE_UUID        "fff7d507-93d5-4b90-bed3-c730a28c9e33"
#define CHARACTERISTIC_UUID "3587bc19-07f7-4766-8dd2-345c8f0404a1"

void MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    advertisedDevice.toString().c_str();
}

void setup() {
    Serial.println("A");
    BLEDevice::init("GooseEgg");

    BLEAddress btAddress = BLEDevice::getAddress();
    Serial.println(btAddress.toString().c_str());

    BLEDevice::deinit();
    
    if(strncmp(btAddress.toString().c_str(), "b4:e6:2d:9d:bb:07", 17) == 0) {
      BLEDevice::init("Roman");
    } else if(strncmp(btAddress.toString().c_str(), "b4:e6:2d:9d:bf:e3", 17) == 0) {
      BLEDevice::init("CottonPatch");      
    } else if(strncmp(btAddress.toString().c_str(), "a4:cf:12:6a:4e:4a", 17) == 0) {
      BLEDevice::init("Toulouse");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:16:53:4a", 17) == 0) {
      BLEDevice::init("AmericanBuff");      
    } else if(strncmp(btAddress.toString().c_str(), "a4:cf:12:45:cc:5e", 17) == 0) {
      BLEDevice::init("Emden");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:16:d8:ea", 17) == 0) {
      BLEDevice::init("Faroese");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:16:df:76", 17) == 0) {
      BLEDevice::init("Pomeranian");      
    } else if(strncmp(btAddress.toString().c_str(), "a4:cf:12:45:d8:4e", 17) == 0) {
      BLEDevice::init("Sebastopol");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:18:23:d6", 17) == 0) {
      BLEDevice::init("Barnacle");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:18:22:ba", 17) == 0) {
      BLEDevice::init("Chinese");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:18:26:ca", 17) == 0) {
      BLEDevice::init("Landrace");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:16:56:56", 17) == 0) {
      BLEDevice::init("PinkFooted");      
    } else if(strncmp(btAddress.toString().c_str(), "24:6f:28:16:d9:3e", 17) == 0) {
      BLEDevice::init("Goosen");      
    } else {
      char deviceName[100];
      sprintf(deviceName, "MooseEgg-%s", btAddress.toString().c_str());
      BLEDevice::init(deviceName);
    }

    Serial.println("B");
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
/*
*/
}

void loop() {
  Serial.println("Moo");
}
