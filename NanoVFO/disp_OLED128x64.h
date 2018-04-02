#ifndef DISP_OLED12864_H
#define DISP_OLED12864_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "TRX.h"

class Display_OLED128x64 {
  private:
    long last_freq;
    uint8_t last_tx;
    int last_BandIndex;
    uint8_t last_wpm;
    uint8_t last_cw;
    byte last_brightness;
  public:
	  void setup();
    // 1..15
    void setBright(uint8_t brightness);
	  void Draw(TRX& trx);
    void DrawMenu(int id, char* text, int value, uint8_t edit);
    void clear();
};

#endif
