#include "disp_oled_sh1106_128x64.h"
// SSD1306Ascii library https://github.com/greiman/SSD1306Ascii
#include "SSD1306AsciiAvrI2c.h"
#include "fonts\lcdnums14x24mod.h"

#define I2C_ADDRESS 0x3C

SSD1306AsciiAvrI2c oled642;

void Display_OLED_SH1106_128x64::setBright(uint8_t brightness)
{
  if (brightness < 0) brightness = 0;
  if (brightness > 15) brightness = 15;
  if (brightness == 0) 
    oled642.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  else {
    if (last_brightness == 0)
      oled642.ssd1306WriteCmd(SSD1306_DISPLAYON);
    oled642.setContrast(brightness << 4);
  }
  last_brightness = brightness; 
}

void Display_OLED_SH1106_128x64::setup() 
{
  oled642.begin(&SH1106_128x64, I2C_ADDRESS);
  clear();
  last_brightness=0;
}

void Display_OLED_SH1106_128x64::Draw(TRX& trx) 
{
  if (trx.Freq != last_freq) {
    char buf[10];
    long f = trx.Freq/10;
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
    buf[9] = 0;
    oled642.setFont(lcdnums14x24mod);
    oled642.setCursor(1, 3);
    oled642.print(buf);
    last_freq = trx.Freq;
  }

  if (trx.BandIndex != last_BandIndex || trx.TX != last_tx || trx.CW != last_cw) {
    oled642.setFont(X11fixed7x14);
    oled642.setCursor(20,0);
    if (trx.TX) oled642.print("TX");
    else oled642.print("RX");

    int mc = Bands[trx.BandIndex].mc;
    oled642.setCursor(128-20-7*4,0);
    if (mc < 100) oled642.print(' ');
    oled642.print(mc);
    oled642.print('m');

    oled642.setCursor(57,0);
    if (trx.CW) oled642.print("CW");
    else oled642.print("  ");

    last_tx = trx.TX;
    last_cw = trx.CW;
    last_BandIndex = trx.BandIndex;
  }

  if (trx.cw_speed != last_wpm) {
    oled642.setFont(Adafruit5x7);
  
    oled642.setCursor(43,7);
    if (trx.cw_speed < 10) oled642.print(' ');
    oled642.print(trx.cw_speed);
    oled642.print(" wpm");

    last_wpm = trx.cw_speed;
  }
}

void Display_OLED_SH1106_128x64::DrawMenu(int id, char* text, int value, uint8_t edit)
{
  oled642.clear();
  oled642.setFont(Adafruit5x7);
  oled642.setCursor(10,1);
  oled642.print(id);
  oled642.print(' ');
  oled642.print(text);
  if (edit) {
    oled642.setCursor(0,3);
    oled642.print("EDIT:");
  }
  oled642.setCursor(50,3);
  oled642.print(value);
  oled642.print("     ");
}

void Display_OLED_SH1106_128x64::clear()
{
  oled642.clear();
  last_freq=0;
  last_tx=0xFF;
  last_BandIndex=0xFF;
  last_wpm=0;
  last_cw=0xFF;
}

