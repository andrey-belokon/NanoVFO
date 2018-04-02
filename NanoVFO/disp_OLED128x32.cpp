#include "disp_oled128x32.h"
// SSD1306Ascii library https://github.com/greiman/SSD1306Ascii
#include "SSD1306AsciiAvrI2c.h"
#include "fonts\lcdnums12x16mod.h"

#define I2C_ADDRESS 0x3C

SSD1306AsciiAvrI2c oled32;

void Display_OLED128x32::setBright(uint8_t brightness)
{
  if (brightness < 0) brightness = 0;
  if (brightness > 15) brightness = 15;
  if (brightness == 0) 
    oled32.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  else {
    if (last_brightness == 0)
      oled32.ssd1306WriteCmd(SSD1306_DISPLAYON);
    oled32.setContrast(brightness << 4);
  }
  last_brightness = brightness; 
}

void Display_OLED128x32::setup() 
{
  oled32.begin(&Adafruit128x32, I2C_ADDRESS);
  clear();
  last_brightness=0;
}

void Display_OLED128x32::Draw(TRX& trx) 
{
  if (trx.Freq != last_freq) {
    char buf[11];
    long f = trx.Freq;
    buf[9] = '0'+f%10; f/=10;
    buf[8] = '0'+f%10; f/=10;
    buf[7] = '0'+f%10; f/=10;
    buf[6] = '.';
    buf[5] = '0'+f%10; f/=10;
    buf[4] = '0'+f%10; f/=10;
    buf[3] = '0'+f%10; f/=10;
    buf[2] = '.';
    buf[1] = '0'+f%10; f/=10;
    if (f > 0) buf[0] = '0'+f;
    else buf[0] = ':';
    buf[10] = 0;
    oled32.setFont(lcdnums12x16mod);
    oled32.setCursor(1, 0);
    oled32.print(buf);
    last_freq = trx.Freq;
  }

  if (trx.TX != last_tx) {
    oled32.setFont(Adafruit5x7);
    oled32.setCursor(7,3);
    if (trx.TX) oled32.print("TX");
    else oled32.print("RX");
    last_tx = trx.TX;
  }

  if (trx.CW != last_cw) {
    oled32.setFont(Adafruit5x7);
    oled32.setCursor(27,3);
    if (trx.CW) oled32.print("CW");
    else oled32.print("  ");
    last_cw = trx.CW;
  }

  if (trx.BandIndex != last_BandIndex) {
    int mc = Bands[trx.BandIndex].mc;
    oled32.setFont(Adafruit5x7);
    oled32.setCursor(47,3);
    if (mc < 100) oled32.print(' ');
    oled32.print(mc);
    oled32.print('m');
    last_BandIndex = trx.BandIndex;
  }
  
  if (trx.cw_speed != last_wpm) {
    oled32.setFont(Adafruit5x7);
    oled32.setCursor(85,3);
    if (trx.cw_speed < 10) oled32.print(' ');
    oled32.print(trx.cw_speed);
    oled32.print(" wpm");
    last_wpm = trx.cw_speed;
  }
}

void Display_OLED128x32::DrawMenu(int id, char* text, int value, uint8_t edit)
{
  oled32.clear();
  oled32.setFont(Adafruit5x7);
  oled32.setCursor(10,0);
  oled32.print(id);
  oled32.print(' ');
  oled32.print(text);
  if (edit) {
    oled32.setCursor(0,2);
    oled32.print("EDIT:");
  }
  oled32.setCursor(50,2);
  oled32.print(value);
  oled32.print("     ");
}

void Display_OLED128x32::clear()
{
  oled32.clear();
  last_freq=0;
  last_tx=0xFF;
  last_BandIndex=0xFF;
  last_wpm=0;
  last_cw=0xFF;
}

