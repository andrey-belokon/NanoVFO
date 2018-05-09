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

struct TRXState {
  long BandData[BAND_COUNT+1]; 
  int BandIndex;
  long Freq;
};

uint16_t hash_data(uint16_t hval, uint8_t* data, int sz)
{
  while (sz--) {
    hval ^= *data++;
    hval = (hval << 11) | (hval >> 5);
  }
  return hval;
}  

uint16_t TRX::StateHash()
{
  uint16_t hash = 0x5AC3;
  hash = hash_data(hash,(uint8_t*)BandData,sizeof(BandData));
  hash = hash_data(hash,(uint8_t*)&BandIndex,sizeof(BandIndex));
  hash = hash_data(hash,(uint8_t*)&Freq,sizeof(Freq));
  return hash;
}

struct TRXState EEMEM eeState;
uint16_t EEMEM eeStateVer;
#define STATE_VER 0x5F17

void TRX::StateSave()
{
  struct TRXState st;
  for (byte i=0; i <= BAND_COUNT+1; i++)
    st.BandData[i] = BandData[i];
  st.BandIndex = BandIndex;
  st.Freq = Freq;
  eeprom_write_block(&st, &eeState, sizeof(st));
  eeprom_write_word(&eeStateVer, STATE_VER);
}

void TRX::StateLoad()
{
  struct TRXState st;
  uint16_t ver;
  ver = eeprom_read_word(&eeStateVer);
  if (ver == STATE_VER) {
    eeprom_read_block(&st, &eeState, sizeof(st));
    for (byte i=0; i <= BAND_COUNT+1; i++)
      BandData[i] = st.BandData[i];
    SwitchToBand(st.BandIndex);
    Freq = st.Freq;
  }
}
