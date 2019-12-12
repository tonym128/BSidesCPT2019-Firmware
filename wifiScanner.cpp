#include "wifiScanner.h"
bool wifiFirstRun = true;

void connect(GameBuff *gameBuff) {
    #ifdef ESP32
    Serial.println("Connect");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    #endif
}

void displayScan(GameBuff *gameBuff) {
    displayClear(gameBuff,0x00);
    drawString2x(gameBuff,(char*)"Wifi Setup",0,0,0xFF,0x00);
    drawString(gameBuff,(char*)"Scanning",0,16,0xFF,0x00);
}

void scan(GameBuff *gameBuff) {
    displayClear(gameBuff,0x00);
    drawString2x(gameBuff,(char*)"Wifi Setup",0,0,0xFF,0x00);
    drawString(gameBuff,(char*)"Press A or B to scan",0,16,0xFF,0x00);

    #ifdef ESP32
    Serial.println("Scan");
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("No networks");
        drawString(gameBuff,(char*)"No networks found",0,24,0xFF,0x00);
    } else {
        char wifiString[50];
        Serial.println("Networks");
        drawString(gameBuff,(char*)"Networks found",0,24,0xFF,0x00);
        for (int i = 0; i < n; ++i) {
            Serial.println(WiFi.SSID(i));
            // Print SSID and RSSI for each network found
            sprintf(wifiString,"%d:%s (%d):%s",i+1,WiFi.SSID(i).c_str(),WiFi.RSSI(i),(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            drawString(gameBuff,wifiString,0,32+i*8,0xFF,0x00);
            delay(10);
        }
    
        Serial.println("Networks Done");
    }
    #endif
}

int selection = 0;
bool scanNow = false;
bool wifiScannerLoop(GameBuff *gameBuff) {
    // Run scan if pending
    if (scanNow) {
        scan(gameBuff);
        selection = 0;
        scanNow = false;
    }

    // Initiate a scan with a or b button
    if (gameBuff->playerKeys.a || gameBuff->playerKeys.b) {
        displayScan(gameBuff);
        scanNow = true;
    }

    // Exit screen with keypress
    if (gameBuff->playerKeys.debouncedInput && gameBuff->playerKeys.start && gameBuff->playerKeys.select) {
        displayScan(gameBuff);
        wifiFirstRun = true;
        return true;
    }

    // If it's the first run setup the wifi
    if (wifiFirstRun) {
        displayClear(gameBuff,0x00);
        wifiFirstRun = false;
        connect(gameBuff);
        displayScan(gameBuff);
        scanNow = true;
    }

    // Show current setup
    drawString(gameBuff,(char*)"Current Wifi :",10,gameBuff->HEIGHT-30,0xFF,0x00);
    drawString(gameBuff,gameBuff->badgeState->ssid,20,gameBuff->HEIGHT-20,0xFF,0x00);

    // Cancel 
    // Choose Wifi
    // Enter Pw
    // Check setup and verify
    // Save / Cancel
    return false;
}
