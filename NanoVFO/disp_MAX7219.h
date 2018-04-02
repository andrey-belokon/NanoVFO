#ifndef DISP_MAX7219_H
#define DISP_MAX7219_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "TRX.h"

class Display_MAX7219 {
  private:
    long lastfreq;
    uint8_t last_cw;
    uint8_t last_brightness;
  public:
	  void setup();
    // 1..15
    void setBright(uint8_t brightness);
	  void Draw(TRX& trx);
    void DrawMenu(int id, char* text, int value, uint8_t edit);
    void clear();
};

#endif
