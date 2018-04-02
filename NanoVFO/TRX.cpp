#include "TRX.h"
#include "config.h"

const struct _Bands Bands[BAND_COUNT] = {
  DEFINED_BANDS
};

TRX::TRX() {
  for (byte i=0; i < BAND_COUNT; i++) {
	  if (Bands[i].startSSB != 0)
	    BandData[i] = Bands[i].startSSB;
	  else
	    BandData[i] = Bands[i].start;
  }
  TX = 0;
  CWTX = 0;
  SwitchToBand(0);
}

void TRX::SwitchToBand(int band) {
  BandIndex = band;
  Freq = BandData[BandIndex];
  sideband = Bands[BandIndex].sideband;
  CW = inCW();
}

void TRX::ChangeFreq(long freq_delta)
{
  if (!TX) {
    uint8_t old_cw = inCW();
    Freq += freq_delta;
    // проверяем выход за пределы диапазона
    if (Freq < Bands[BandIndex].start)
      Freq = Bands[BandIndex].start;
    else if (Freq > Bands[BandIndex].end)
      Freq = Bands[BandIndex].end;
    uint8_t new_cw = inCW();
    if (old_cw != new_cw) CW = new_cw;
  }
}

void TRX::NextBand()
{
  if (!TX) {
    BandData[BandIndex] = Freq;
    if (++BandIndex >= BAND_COUNT)
      BandIndex = 0;
    Freq = BandData[BandIndex];
    sideband = Bands[BandIndex].sideband;
    CW = inCW();
  }
}

void TRX::PrevBand()
{
  if (!TX) {
    BandData[BandIndex] = Freq;
    if (--BandIndex < 0)
      BandIndex = BAND_COUNT-1;
    Freq = BandData[BandIndex];
    sideband = Bands[BandIndex].sideband;
    CW = inCW();
  }
}

uint8_t TRX::inCW() {
  return 
    BandIndex >= 0 && Bands[BandIndex].startSSB > 0 &&
    Freq < Bands[BandIndex].startSSB &&
    Freq >= Bands[BandIndex].start;
}

uint8_t TRX::setCWSpeed(uint8_t speed, int dash_ratio)
{
  if (speed != cw_speed) {
    cw_speed = speed;
    dit_time = 1200/cw_speed;
    dah_time = dit_time*dash_ratio/10;
    return 1;
  }
  return 0;
}

