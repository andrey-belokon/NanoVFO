#ifndef DISP_1602_H
#define DISP_1602_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "TRX.h"
#include "LCD1602_I2C.h"

class Display_1602_I2C {
  private:
  	LiquidCrystal_I2C lcd;
    uint8_t last_tx;
    int last_BandIndex;
    long last_freq;
    uint8_t last_wpm;
    uint8_t last_cw;
  public:
	  Display_1602_I2C (int i2c_addr): lcd(i2c_addr,16,2) {}
    void setup();
    void setBright(uint8_t brightness);
    void Draw(TRX& trx);
    void DrawMenu(int id, char* text, int value, uint8_t edit);
    void clear();
};

#endif
