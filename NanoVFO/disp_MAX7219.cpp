  #include "disp_MAX7219.h"

#define DIN_PIN   8
#define CS_PIN    7
#define CLK_PIN   6

#define DECODEMODE_ADDR 9
#define BRIGHTNESS_ADDR  10
#define SCANLIMIT_ADDR  11
#define SHUTDOWN_ADDR 12
#define DISPLAYTEST_ADDR 15

uint8_t charTable [] = {
  B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,B01111111,B01111011
};

void max7219_write(volatile byte address, volatile byte data) {
  digitalWrite(CS_PIN, LOW);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, address);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, data);
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(CLK_PIN, LOW); // this pin also used in touch key read. so turn to low after use
}

void max7219_clear() {
  for (int i = 1; i <= 8; i++) {
    max7219_write(i, B00000000);
  }
}

 void max7219_setDigitLimit(uint8_t limit) {
  max7219_write(DISPLAYTEST_ADDR, 0);
  max7219_write(SCANLIMIT_ADDR, limit-1);
  // 0: Register Format
  // 255: Code B Font (0xff)
  max7219_write(DECODEMODE_ADDR, 0);
  max7219_clear();
  max7219_write(SHUTDOWN_ADDR, 1);
}

void Display_MAX7219::setBright(uint8_t brightness)
{
  if (brightness < 0) brightness = 0;
  if (brightness > 15) brightness = 15;
  if (brightness == 0) 
    max7219_write(SHUTDOWN_ADDR, 0x00);
  else {
    if (last_brightness == 0)
      max7219_write(SHUTDOWN_ADDR, 0x01);
    max7219_write(BRIGHTNESS_ADDR, brightness);
  }
  last_brightness = brightness;
}

void Display_MAX7219::setup() 
{
  pinMode(DIN_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  setBright(7);
  max7219_setDigitLimit(8);
  clear();
}

void Display_MAX7219::Draw(TRX& trx) 
{
  long freq = trx.Freq;
  if (freq != lastfreq || trx.CW != last_cw) {
    lastfreq = freq;
    last_cw = trx.CW;
    for (uint8_t i=1; i <= 8; i++) {
      if (freq) {
        uint8_t ch = charTable[freq%10];
        if ((i == 1 && trx.CW) || i == 4 || i == 7) ch |= B10000000;
        max7219_write(i,ch);
        freq /= 10;
      } else {
        max7219_write(i,0);
      }
    }
  }
}

void Display_MAX7219::DrawMenu(int id, char* text, int value, uint8_t edit)
{
  clear();
  uint8_t ch = charTable[id%10];
  if (edit) ch |= B10000000;
  max7219_write(7,ch);
  max7219_write(8,charTable[id/10]);
  if (value == 0) {
    max7219_write(1,charTable[0]);
    return;
  }
  uint8_t sign = 0;
  if (value < 0) {
    sign = 1;
    value = -value;
  }
  for (uint8_t i=1; i <= 5; i++) {
    if (value) {
      uint8_t ch = charTable[value%10];
      max7219_write(i,ch);
      value /= 10;
    } else {
      if (sign) max7219_write(i,1);
      break;
    }
  }
}

void Display_MAX7219::clear()
{
  lastfreq = 0;
  last_cw = 0xFF;
  max7219_clear();
}

