#include "disp_1602.h"

byte chInverseT[8] = { 0b11111, 0b11111, 0b11111, 0b10001, 0b11011, 0b11011, 0b11111, 0b11111};
byte chInverseX[8] = { 0b11111, 0b11111, 0b11111, 0b10101, 0b11011, 0b10101, 0b11111, 0b11111};
byte chLock[8] = { 0b00000, 0b01110, 0b10001, 0b11111, 0b10101, 0b10001, 0b11111, 0b00000};
byte chCW[8] = { 0b01100, 0b10000, 0b10000, 0b01100, 0b00000, 0b10001, 0b10101, 0b01010};

void Display_1602_I2C::setup() {
  lcd.init();
  lcd.backlight();// Включаем подсветку дисплея
  lcd.createChar(1, chInverseT);
  lcd.createChar(2, chInverseX);
  lcd.createChar(3, chCW);
  lcd.createChar(4, chLock);
  clear();
}

void Display_1602_I2C::Draw(TRX& trx) {
  char buf[17];

  if (trx.TX != last_tx || trx.BandIndex != last_BandIndex || trx.cw_speed != last_wpm) {
    memset(buf,' ',17);
    if (trx.TX) {
      buf[0] = (char)1;
      buf[1] = (char)2;
    } else {
      buf[0] = 'R';
      buf[1] = 'X';
    }
  
    int mc = Bands[trx.BandIndex].mc;
    if (mc > 99) {
      buf[5] = '0'+mc%10; mc/=10;
      buf[4] = '0'+mc%10; mc/=10;
      buf[3] = '0'+mc;
    } else {
      buf[5] = '0'+mc%10; mc/=10;
      buf[4] = '0'+mc; 
    }
    buf[6] = 'm';
  
    uint8_t wpm = trx.cw_speed;
    if (wpm > 9) {
      buf[10] = '0'+wpm%10; wpm/=10;
      buf[9] = '0'+wpm;
    } else {
      buf[10] = '0'+wpm;
    }
    buf[12] = 'W';
    buf[13] = 'P';
    buf[14] = 'M';
  
    buf[16] = 0; // stop for .print
    lcd.setCursor(0, 0);
    lcd.print(buf);

    last_tx = trx.TX;
    last_BandIndex = trx.BandIndex;
    last_wpm = trx.cw_speed;
  }

  if (trx.CW != last_cw || trx.Freq != last_freq) {
    memset(buf,' ',17);
    if (trx.CW) {
      buf[0] = 'C';
      buf[1] = 'W';
    }
  
    long f = trx.Freq;
    buf[15] = '0'+f%10; f/=10;
    buf[14] = '0'+f%10; f/=10;
    buf[13] = '0'+f%10; f/=10;
    buf[12] = '.';
    buf[11] = '0'+f%10; f/=10;
    buf[10] = '0'+f%10; f/=10;
    buf[9] = '0'+f%10; f/=10;
    buf[8] = '.';
    buf[7] = '0'+f%10; f/=10;
    if (f > 0) buf[6] = '0'+f;

    buf[16] = 0; // stop for .print
    lcd.setCursor(0, 1);
    lcd.print(buf);

    last_cw =  trx.CW;
    last_freq = trx.Freq;
  }
}

void Display_1602_I2C::setBright(uint8_t brightness)
{
  lcd.setBacklight(brightness);
}

void Display_1602_I2C::DrawMenu(int id, char* text, int value, uint8_t edit)
{
  char buf[17],*p;
  memset(buf,' ',17);
  buf[0] = '0'+id/10;
  buf[1] = '0'+id%10;
  for (uint8_t j=3; j <= 15 && *text; j++)
    buf[j] = *text++;
  buf[16] = 0; // stop for .print
  lcd.setCursor(0, 0);
  lcd.print(buf);
  memset(buf,' ',17);
  if (edit) {
    buf[0] = 'E';
    buf[1] = 'D';
    buf[2] = 'I';
    buf[3] = 'T';
    buf[4] = ':';
  }
  p = &buf[6];
  if (value < 0) {
    *p++ = '-';
    value = -value;
  }
  if (value > 9999) {
    *p++ = '0'+value/10000;
    value %= 10000;
  }
  if (value > 999) {
    *p++ = '0'+value/1000;
    value %= 1000;
  }
  if (value > 99) {
    *p++ = '0'+value/100;
    value %= 100;
  }
  if (value > 9) {
    *p++ = '0'+value/10;
    value %= 10;
  }
  *p++ = '0'+value;
  lcd.setCursor(0, 1);
  lcd.print(buf);
}

void Display_1602_I2C::clear()
{
  last_tx=0xFF;
  last_BandIndex=0xFF;
  last_wpm=0xFF;
  last_cw=0xFF;
  last_freq=0xFF;
}

