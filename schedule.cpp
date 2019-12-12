#include "schedule.hpp"
#include "headerimages/byron_rudman.jpg.h"
#include "headerimages/kgothatso_ngako.jpg.h"
#include "headerimages/philipp_krenn.jpg.h"
#include "headerimages/silent_dzikiti.jpg.h"
#include "headerimages/bsidesplaceholder.jpg.h"
#include "headerimages/robertf.jpg.h"
#include "headerimages/lightningtalks.jpg.h"

#include "headerimages/chrisk.jpg.h"
#include "headerimages/gerardd.jpeg.h"
#include "headerimages/leonj.jpg.h"
#include "headerimages/amym.jpg.h"
#include "headerimages/christog2.jpg.h"
#include "headerimages/brents.jpg.h"

#define TRACKS 2
#define ITEMS 12
struct Location
{
    int track;
    int item;
    bool showQR = false;
};

struct Schedule
{
    PlayerKeys playerKeys1;
    PlayerKeys playerKeys2;
    Location location;
    bool builtSchedule = false;
};

void setSchedule(Schedule *schedule)
{
    schedule->location.item = 0;
    schedule->location.track = 0;
    schedule->location.showQR = false;
}

Schedule *schedule = nullptr;

struct Item
{
    char start_time[6];
    char end_time[6];
    char speaker[30];
    char title[100];
    char url[100];
    const unsigned char *pic;
    int picSize;
};

struct Track
{
    char name[30];
};

Track setTrack(char *TrackName)
{
    Track t;
    sprintf(t.name, "%s", TrackName);
    return t;
}

Track getTrack(int TrackId)
{
    switch (TrackId)
    {
    case 0:
        return setTrack((char*)"Track 1");
    case 1:
        return setTrack((char*)"Track 2");
    }

    return setTrack((char*)"Track ?");
}

Item setItem(char *start_time, char *end_time, char *speaker, char *title, const unsigned char *pic, int picSize, char *url)
{
    Item i;
    sprintf(i.start_time, "%s", start_time);
    sprintf(i.end_time, "%s", end_time);
    sprintf(i.speaker, "%s", speaker);
    sprintf(i.title, "%s", title);
    sprintf(i.url, "%s", url);
    i.pic = pic;
    i.picSize = picSize;
    return i;
}

Item getTrack1Item(int ItemId)
{
    int counter = 0;
    switch (ItemId)
    {
    case 0:
        return setItem((char*)"08:00", (char*)"09:00", (char*)"Organisers", (char*)"Registration", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg), (char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 1:
        return setItem((char*)"09:00", (char*)"09:30", (char*)"Organisers", (char*)"Opening Remarks & Badge  Talk", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg), (char*)"https://www.youtube.com/watch?v=EXOIqVmxHWA");
    case 2:
        return setItem((char*)"09:30", (char*)"10:20", (char*)"Chris Kubeka", (char*)"The Woman Who Squashed   Terrorists When an       Embassy gets Hacked", (const unsigned char *)&chrisk_jpg, sizeof(chrisk_jpg),(char*)"https://bsidescapetown.co.za/speaker/chris-kubecka/");
    case 3:
        return setItem((char*)"10:30", (char*)"11:20", (char*)"Gerard de Jong", (char*)"Hacking satellites with  Software Defined Radio   (SDR)", (const unsigned char *)&gerardd_jpeg, sizeof(gerardd_jpeg),(char*)"https://bsidescapetown.co.za/speaker/gerard-de-jong/");
    case 4:
        return setItem((char*)"11:30", (char*)"12:20", (char*)"Philipe Delteil", (char*)"Macabre stories of a     hacker in the public     health sector", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 5:
        return setItem((char*)"12:30", (char*)"13:20", (char*)"Leon Jacobs", (char*)"Meticulously Modern      Mobile Manipulations", (const unsigned char *)&leonj_jpg, sizeof(leonj_jpg),(char*)"https://bsidescapetown.co.za/speaker/leon-jacobs/");
    case 6:
        return setItem((char*)"13:20", (char*)"14:15", (char*)"Lunch", (char*)"Lunch", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 7:
        return setItem((char*)"14:15", (char*)"15:00", (char*)"Christo Goosen", (char*)"Natural Language Process-ing and Anomaly detectionin System call logs", (const unsigned char *)&christog2_jpg, sizeof(christog2_jpg),(char*)"https://bsidescapetown.co.za/speaker/christo-goosen-2/");
    case 8:
        return setItem((char*)"15:30", (char*)"16:30", (char*)"Amy Mania", (char*)"Put Words In My Mouth", (const unsigned char *)&amym_jpg, sizeof(amym_jpg),(char*)"https://bsidescapetown.co.za/speaker/amy-mania/");
    case 9:
        return setItem((char*)"16:30", (char*)"17:30", (char*)"Brent Shaw", (char*)"Hashing the $#!+ out of  firmware”", (const unsigned char *)&brents_jpg, sizeof(brents_jpg),(char*)"https://bsidescapetown.co.za/speaker/brent-shaw-2/");
    case 10:
        return setItem((char*)"17:30", (char*)"18:00", (char*)"Organisers", (char*)"Closing Ceremony", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 11:
        return setItem((char*)"18:00", (char*)"20:00", (char*)"Organisers", (char*)"Lightning Talks / After  Drinks", (const unsigned char *)&lightningtalks_jpg, sizeof(lightningtalks_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    }

    return setItem((char*)"00:00", (char*)"00:00", (char*)"No one", (char*)"NO TALK", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://www.youtube.com/watch?v=Vv_Jszi1Qiw");
}

Item getTrack2Item(int ItemId)
{
    int counter = 0;
    switch (ItemId)
    {
    case 0:
        return setItem((char*)"08:00", (char*)"09:00", (char*)"Organisers", (char*)"Registration", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg), (char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 1:
        return setItem((char*)"09:00", (char*)"09:30", (char*)"Organisers", (char*)"Opening Remarks & Badge  Talk", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg), (char*)"https://www.youtube.com/watch?v=EXOIqVmxHWA");
    case 2:
        return setItem((char*)"09:30", (char*)"10:20", (char*)"No one", (char*)"NO TALK", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://www.youtube.com/watch?v=Vv_Jszi1Qiw");
    case 3:
        return setItem((char*)"10:30", (char*)"11:20", (char*)"Byron Rudman", (char*)"Gollum - One anti-phish  bot to rule them all", (const unsigned char *)&byron_rudman_jpg, sizeof(byron_rudman_jpg),(char*)"https://bsidescapetown.co.za/speaker/byron-rudman/");
    case 4:
        return setItem((char*)"11:30", (char*)"12:20", (char*)"Liam Smit", (char*)"The Nation State Actor   Ate My Homework!", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speaker/liam-smit/");
    case 5:
        return setItem((char*)"12:30", (char*)"13:20", (char*)"Silent Dzikiti", (char*)"How the application of   machine learning and AI  can help reduce the cyber-security attacks.", (const unsigned char *)&silent_dzikiti_jpg, sizeof(silent_dzikiti_jpg),(char*)"https://bsidescapetown.co.za/speaker/silent-dzikiti/");
    case 6:
        return setItem((char*)"13:20", (char*)"14:15", (char*)"Lunch", (char*)"Lunch", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 7:
        return setItem((char*)"14:15", (char*)"15:00", (char*)"Philipp Krenn", (char*)"seccomp - Your Next Layer of Defense", (const unsigned char *)&philipp_krenn_jpg, sizeof(philipp_krenn_jpg),(char*)"https://bsidescapetown.co.za/speaker/philipp-krenn/");
    case 8:
        return setItem((char*)"15:30", (char*)"16:30", (char*)"Kgothatso Ngako", (char*)"Authentication is Broken. Can We Try Fix It?”", (const unsigned char *)&kgothatso_ngako_jpg, sizeof(kgothatso_ngako_jpg),(char*)"https://bsidescapetown.co.za/speaker/kgothatso-ngako/");
    case 9:
        return setItem((char*)"16:30", (char*)"17:30", (char*)"Robert Feeney", (char*)"Web Application Vulnerabi-lity Scanners – An Int-roduction & Discussion  on their limitations", (const unsigned char *)&robertf_jpg, sizeof(robertf_jpg),(char*)"https://bsidescapetown.co.za/speaker/robert-feeney/");
    case 10:
        return setItem((char*)"17:30", (char*)"18:00", (char*)"Organisers", (char*)"Closing Ceremony", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    case 11:
        return setItem((char*)"18:00", (char*)"20:00", (char*)"Organisers", (char*)"Lightning Talks / After  Drinks", (const unsigned char *)&lightningtalks_jpg, sizeof(lightningtalks_jpg),(char*)"https://bsidescapetown.co.za/speakers-2/programme-2019/");
    }

    return setItem((char*)"00:00", (char*)"00:00", (char*)"No one", (char*)"NO TALK", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://www.youtube.com/watch?v=Vv_Jszi1Qiw");
}

Item getTrackItem(int TrackId, int ItemId)
{
    switch (TrackId)
    {
    case 0:
        return getTrack1Item(ItemId);
    case 1:
        return getTrack2Item(ItemId);
    }

    return setItem((char*)"00:00", (char*)"00:00", (char*)"No one", (char*)"NO TALK", (const unsigned char *)&bsidesplaceholder_jpg, sizeof(bsidesplaceholder_jpg),(char*)"https://www.youtube.com/watch?v=Vv_Jszi1Qiw");
}

void displayItemQRCode(GameBuff *gameBuff,Item item)
{
	qrcodeStruct QRPic = drawQRCode(item.url);
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

	drawString(gameBuff,item.speaker,3,3,0xFF,0);
}

void drawScheduleItem(GameBuff *gameBuff, Location location)
{
    int xOffset = 20;
    int yOffset = 20;

    uint8_t lineColour = 0b01001001;
    uint8_t textColour = 0xFF;

    Track track = getTrack(location.track);
    Item item = getTrackItem(location.track, location.item);
    if (location.showQR) {
        displayItemQRCode(gameBuff, item);
        drawString(gameBuff, (char*)"Press A for profile",xOffset,gameBuff->HEIGHT - 8,textColour,-1);
        return;
    }

    drawString2x(gameBuff, track.name, xOffset, yOffset + 0, textColour, 0x00);

    drawLine(gameBuff, 0, gameBuff->WIDTH, yOffset - 3, yOffset - 3, lineColour);
    drawLine(gameBuff, 0, gameBuff->WIDTH, yOffset + 15, yOffset + 15, lineColour);
    drawLine(gameBuff, 0, gameBuff->WIDTH, yOffset + 20 + 16, yOffset + 20 + 16, lineColour);
    drawLine(gameBuff, 0, gameBuff->WIDTH, yOffset + 40 + 16, yOffset + 40 + 16, lineColour);
    drawLine(gameBuff, 0, gameBuff->WIDTH, yOffset + 110, yOffset + 110, lineColour);
    drawLine(gameBuff, 0, gameBuff->WIDTH, gameBuff->HEIGHT - (yOffset - 3), gameBuff->HEIGHT - (yOffset - 3), lineColour);

    drawLine(gameBuff, xOffset - 3, xOffset - 3, 0, 240, lineColour);
    drawLine(gameBuff, gameBuff->WIDTH - (xOffset - 3), gameBuff->WIDTH - (xOffset - 3), 0, 240, lineColour);

    drawString2x(gameBuff, item.start_time, xOffset, yOffset + 19, textColour, 0x00);
    drawString2x(gameBuff,(char*)"-", 88, yOffset + 19, textColour, 0x00);
    drawString2x(gameBuff, item.end_time, 112, yOffset + 19, textColour, 0x00);

    drawString(gameBuff, item.speaker, xOffset, yOffset + 40, textColour, 0x00);
    drawString(gameBuff, (char*)"Press A for QR Code",xOffset,gameBuff->HEIGHT - 8,textColour,-1);

    int characters = 25;
    char title[30] = "";
    unsigned int currentPos = 0;
    int lineCounter = 0;
    while (currentPos < strlen(item.title)) {
        strncpy(title, item.title + currentPos, characters);
        drawString(gameBuff, title, xOffset, yOffset + 60 + lineCounter * 8, textColour, 0x00);
        currentPos += characters;
        ++lineCounter;
    }

    TJpgDec.drawJpg(xOffset, yOffset + 112, item.pic, item.picSize);
}

bool displaySchedule(GameBuff *gameBuff)
{

    if (schedule == nullptr)
    {
        if (!gameBuff->achievementData->scheduleView) {
            schedule = (Schedule *)calloc(1, sizeof(Schedule));
            setSchedule(schedule);
            displayClear(gameBuff, 0x00);
            drawScheduleItem(gameBuff, schedule->location);
            gameBuff->achievementData->scheduleView = true;
            gameBuff->achievementData->newAchievment = true;
            saveAchievements(gameBuff->achievementData);
            free(schedule);
            schedule = nullptr;
            return false;
        } else {
            schedule = (Schedule *)calloc(1, sizeof(Schedule));
            setSchedule(schedule);
            displayClear(gameBuff, 0x00);
            drawScheduleItem(gameBuff, schedule->location);
        }
    };

    if ((
            gameBuff->playerKeys.up || gameBuff->playerKeys.down || gameBuff->playerKeys.left || gameBuff->playerKeys.right) &&
        gameBuff->playerKeys.debouncedInput)
    {
        if (gameBuff->playerKeys.left)
            schedule->location.track -= 1;
        if (gameBuff->playerKeys.right)
            schedule->location.track += 1;

        if (gameBuff->playerKeys.up)
            schedule->location.item -= 1;
        if (gameBuff->playerKeys.down)
            schedule->location.item += 1;

        if (schedule->location.item < 0)
            schedule->location.item = ITEMS - 1;
        if (schedule->location.item >= ITEMS)
            schedule->location.item = 0;

        if (schedule->location.track < 0)
            schedule->location.track = TRACKS - 1;
        if (schedule->location.track >= TRACKS)
            schedule->location.track = 0;

        displayClear(gameBuff, 0x00);
        drawScheduleItem(gameBuff, schedule->location);
    }

    if ((gameBuff->playerKeys.a || gameBuff->playerKeys.b) &&
        gameBuff->playerKeys.debouncedInput)
    {
        schedule->location.showQR = !schedule->location.showQR;
        displayClear(gameBuff, 0x00);
        drawScheduleItem(gameBuff, schedule->location);
    }

    if ((gameBuff->playerKeys.start || gameBuff->playerKeys.select) &&
        gameBuff->playerKeys.debouncedInput)
    {
        schedule->builtSchedule = false;
        free(schedule);
        schedule = nullptr;
        return true;
    }

    return false;
}