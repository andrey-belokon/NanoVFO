#ifndef TRX_H
#define TRX_H

#include <Arduino.h>
#include "config.h"

class TRX {
  public:
    long BandData[BAND_COUNT+1]; 
    int BandIndex;
    long Freq;
    uint8_t sideband;
    uint8_t CW;
    uint8_t TX;
    uint8_t CWTX;
    uint8_t cw_speed;
    uint16_t dit_time;
    uint16_t dah_time;

	  TRX();
    void SwitchToBand(int band);
    void NextBand();
    void PrevBand();
    void ChangeFreq(long freq_delta);
    uint8_t inCW();
    uint8_t setCWSpeed(uint8_t speed, int dash_ratio);

    uint16_t StateHash();
    void StateSave();
    void StateLoad();
};

#endif
