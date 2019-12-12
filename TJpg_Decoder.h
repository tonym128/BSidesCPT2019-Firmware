/*
TJpg_Decoder.h

JPEG Decoder for Arduino using TJpgDec:
http://elm-chan.org/fsw/tjpgd/00index.html

Incorporated into an Arduino library by Bodmer 18/10/19

Latest version here:
https://github.com/Bodmer/TJpg_Decoder
*/
#ifdef ESP32
  #include "Arduino.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "User_Config.h"
#include "tjpgd.h"

#ifndef TJpg_Decoder_H
  #define TJpg_Decoder_H

enum {
	TJPG_ARRAY = 0,
	TJPG_FS_FILE,
	TJPG_SD_FILE
};

//------------------------------------------------------------------------------

typedef bool (*SketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *data);

class TJpg_Decoder {

public:

  TJpg_Decoder();
  ~TJpg_Decoder();

  static uint16_t jd_output(JDEC* jdec, void* bitmap, JRECT* jrect);
  static uint16_t jd_input(JDEC* jdec, uint8_t* buf, uint16_t len);

  void setJpgScale(uint8_t scale);
  void setCallback(SketchCallback sketchCallback);


  JRESULT drawJpg(int32_t x, int32_t y, const uint8_t array[], uint32_t  array_size);
  JRESULT getJpgSize(uint16_t *w, uint16_t *h, const uint8_t array[], uint32_t  array_size);

  void setSwapBytes(bool swap);

  bool _swap = false;

  const uint8_t* array_data  = nullptr;
  uint32_t array_index = 0;
  uint32_t array_size  = 0;

  // Must align workspace to a 32 bit boundary
  uint8_t workspace[TJPGD_WORKSPACE_SIZE];// __attribute__((aligned(4)));

  uint8_t jpg_source = 0;

  int16_t jpeg_x = 0;
  int16_t jpeg_y = 0;

  uint8_t jpgScale = 0;

  SketchCallback tft_output = nullptr;

  TJpg_Decoder *thisPtr = nullptr;
};

extern TJpg_Decoder TJpgDec;

#endif // TJpg_Decoder_H
