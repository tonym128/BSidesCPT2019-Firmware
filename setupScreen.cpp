#include "setupScreen.h"
#include "headerimages/hacktheplanet.jpg.h"
#include "headerimages/fine.jpg.h"

struct SetupState {
    int screen = 0;
    int min = 0;
    int max = 0;
    bool enter = false;
};

SetupState *setupState = nullptr;

void initState(SetupState *setupState) {
    setupState->screen = 1;
    setupState->min = 1;
    setupState->max = 7;
    setupState->enter = false;
}

bool updateSetupMenus(GameBuff *gameBuff)
{
	if (gameBuff->playerKeys.debouncedInput)
	{
		if (gameBuff->playerKeys.up)
		{
			setupState->screen -= 1;
		}
		if (gameBuff->playerKeys.down)
		{
			setupState->screen += 1;
		}

		if (gameBuff->playerKeys.a || gameBuff->playerKeys.b) {
			setupState->enter = true;
		}
	}

	if (setupState->screen == 0)
	{
		setupState->screen = setupState->max;
	}
	else if (setupState->screen == setupState->max + 1)
	{
		setupState->screen = 1;
	}

	return true;
}

void displayQRCode(GameBuff *gameBuff)
{
	qrcodeStruct QRPic = drawQRCode((char*)"https://bsidescapetown.co.za/staff/");
	Dimensions dim;
	dim.x = 0;
	dim.y = 0;
	dim.width = QRPic.size;
	dim.height = QRPic.size;

	if (QRPic.size > 0)
	{

		// Centre the QR Code
		int x = gameBuff->WIDTH / 2;
		while (QRPic.size < x)
		{
			x /= 2;
		}

		dim.x = dim.y = x - QRPic.size / 2;

		// Draw the QR Code
		drawObject(gameBuff, dim, QRPic.pic, 0x00, 0xFF, -1);
		free(QRPic.pic);

		// Double the screen till we fill as much as we can.
		x = gameBuff->WIDTH / 2;
		while (QRPic.size < x)
		{
			drawScreenDouble(gameBuff);
			x /= 2;
		}
	}
}

bool hasRunOnce = false;
bool setupScreenLoop(GameBuff *gameBuff) {
    if (setupState == nullptr) {
        setupState = (SetupState *)calloc(1,sizeof(SetupState));
        initState(setupState);
    }

    if (!setupState->enter) { // Menu
        updateSetupMenus(gameBuff);
               
        // Draw menu with selection
        int i = 1;
        displayClear(gameBuff,0x03); // Blue screen joke

        drawString2x(gameBuff,(char*)"   Setup   ",0,16*i++,0xE0,0x00);
        drawString2x(gameBuff,(char*)"-----------",0,16*i++,0xE0,0x00);
        drawString2x(gameBuff,(char*)"1) Name    ",0,16*i++,setupState->screen == 1 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"2) Wifi    ",0,16*i++,setupState->screen == 2 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"3) Firmware",0,16*i++,setupState->screen == 3 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"4) Hack    ",0,16*i++,setupState->screen == 4 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"5) Hit Me  ",0,16*i++,setupState->screen == 5 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"6) Rotate  ",0,16*i++,setupState->screen == 6 ? 0xE0 : 0xFF,0x00);
        drawString2x(gameBuff,(char*)"7) Back    ",0,16*i++,setupState->screen == 7 ? 0xE0 : 0xFF,0x00);
    } 
    else // Otherwise run menu screen code
    {
        switch (setupState->screen) {
            case 1: // Enter Name
                if (nameLoop(gameBuff)) {
                    setupState->enter = false;
                }
            break;
            case 2: // Setup Wifi
                if (!hasRunOnce) {
                  hasRunOnce = true;
                }
                else if (wifiScannerLoop(gameBuff)) {
                    setupState->enter = false;
                    #ifdef ESP32
                        ESP.restart();
                    #endif
                }
            break;
            case 3: // Update Firmware
                if (hasRunOnce)
                {
                    updateMinTime(100);
                } 
                else
                {
                    TJpgDec.drawJpg(0, 0, fine_jpg, sizeof(fine_jpg));
                    hasRunOnce = true; 
                }

                if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.start || gameBuff->playerKeys.select)) {
                    setupState->enter = false;
                    hasRunOnce = false;
                }
            break;
            case 4: // Hack
                if (hasRunOnce) {
                    
                } else {
                    displayClear(gameBuff,0x00);
                    drawString(gameBuff,(char*)"Firmware available",0,10,0xFF,0x00);
                    drawString(gameBuff,(char*)"https://github.com/dodgymike/",10,20,0xFF,0x00);
                    drawString(gameBuff,(char*)"BSidesCPT2019-firmware",10,30,0xFF,0x00);
                    drawString(gameBuff,(char*)"PCB available",0,40,0xFF,0x00);
                    drawString(gameBuff,(char*)"https://github.com/dodgymike/",10,50,0xFF,0x00);
                    drawString(gameBuff,(char*)"BSidesCPT2019",10,60,0xFF,0x00);
                    TJpgDec.drawJpg(0, gameBuff->HEIGHT-140, hacktheplanet_jpg, sizeof(hacktheplanet_jpg));
                    hasRunOnce = true; 
                }

                if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.start || gameBuff->playerKeys.select)) {
                    setupState->enter = false;
                    hasRunOnce = false;
                }

            break;
            case 5: // Hit me
                if (hasRunOnce)
                {
                } else {
                    displayClear(gameBuff, 0x0F);
                    displayQRCode(gameBuff);
                    hasRunOnce = true;
                }

                if (gameBuff->playerKeys.debouncedInput && (gameBuff->playerKeys.start || gameBuff->playerKeys.select)) {
                    setupState->enter = false;
                    hasRunOnce = false;
                    }
            break;
            case 6: // Rotate
                if (gameBuff->playerKeys.debouncedInput) {
                    gameBuff->rotate = !gameBuff->rotate;
                    setupState->enter = false;
                }
                break;
            case 7: // Go Back
                free(setupState);
                setupState = nullptr;
                return true;
            break;
        }
    }

    return false;
}