//
// UR5FFR Si5351 NanoVFO
// v1.0 from 2.04.2018
// Copyright (c) Andrey Belokon, 2017-2018 Odessa
// https://github.com/andrey-belokon/
// GNU GPL license
// Special thanks for
// Iambic sensor key http://www.jel.gr/cw-mode/iambic-keyer-with-arduino-in-5-minutes
// vk3hn CW keyer https://github.com/prt459/vk3hn_CW_keyer/blob/master/Basic_CW_Keyer.ino
//

#include <avr/power.h>
#include <avr/eeprom.h> 

// !!! all user setting defined in config.h, config_hw.h and config_sw.h files !!!
#include "config.h"

#include "pins.h"
#include "i2c.h"
#include "TRX.h"
#include "Encoder.h"
#include "si5351a.h"

#ifdef DISPLAY_MAX7219
  #include "disp_MAX7219.h"
#endif
#ifdef DISPLAY_1602
  #include "disp_1602.h"
#endif
#ifdef DISPLAY_OLED128x32
  #include "disp_OLED128x32.h"
#endif
#ifdef DISPLAY_OLED128x64
  #include "disp_OLED128x64.h"
#endif

Si5351 vfo5351;
Encoder encoder(ENCODER_PULSE_PER_TURN, ENCODER_FREQ_LO_STEP, ENCODER_FREQ_HI_STEP, ENCODER_FREQ_HI_LO_TRASH);
TRX trx;

#ifdef DISPLAY_MAX7219
  Display_MAX7219 disp;
#endif
#ifdef DISPLAY_1602
  Display_1602_I2C disp(I2C_ADR_DISPLAY_1602);
#endif
#ifdef DISPLAY_OLED128x32
  Display_OLED128x32 disp;
#endif
#ifdef DISPLAY_OLED128x64
  Display_OLED128x64 disp;
#endif

InputPullUpPin inPTT(PIN_IN_PTT);

OutputBinPin outCW(PIN_OUT_CW, !OUT_CW_ACTIVE_LEVEL, OUT_CW_ACTIVE_LEVEL);
OutputBinPin outPTT(PIN_OUT_PTT, !OUT_PTT_ACTIVE_LEVEL, OUT_PTT_ACTIVE_LEVEL);
OutputBinPin outKEY(PIN_OUT_KEY, !OUT_KEY_ACTIVE_LEVEL, OUT_KEY_ACTIVE_LEVEL);

InputAnalogKeypad keypad(PIN_ANALOG_KEYPAD, 6);

struct _Settings SettingsDef[SETTINGS_COUNT] = {
  SETTINGS_DATA
};

uint16_t EEMEM eeSettingsVersion = 0;
int EEMEM eeSettings[SETTINGS_COUNT] = {0};  

int Settings[SETTINGS_COUNT] = {0};  

void writeSettings()
{
  eeprom_write_block(Settings, eeSettings, sizeof(Settings));
}

void resetSettings()
{
  for (uint8_t j=0; j < SETTINGS_COUNT; j++)
    Settings[j] = SettingsDef[j].def_value;
}

void readSettings()
{
  uint16_t ver;
  ver = eeprom_read_word(&eeSettingsVersion);
  if (ver == ((SETTINGS_VERSION << 8) | SETTINGS_COUNT))
    eeprom_read_block(Settings, eeSettings, sizeof(Settings));
  else {
    // fill with defaults
    resetSettings();
    writeSettings();
    ver = (SETTINGS_VERSION << 8) | SETTINGS_COUNT;
    eeprom_write_word(&eeSettingsVersion, ver);
  }
}

void setup()
{
  clock_prescale_set(clock_div_2);
  //Serial.begin(9600);
  readSettings();
  outCW.setup();
  outPTT.setup();
  outKEY.setup();
  inPTT.setup();
  keypad.setup();
  pinMode(PIN_OUT_BAND0, OUTPUT);
  pinMode(PIN_OUT_BAND1, OUTPUT);
  pinMode(PIN_OUT_BAND2, OUTPUT);
  pinMode(PIN_OUT_BAND3, OUTPUT);
  pinMode(PIN_OUT_TONE, OUTPUT);
  pinMode(PIN_CW_SPEED_POT, INPUT);
  pinMode(PIN_KEY_READ_STROB, OUTPUT);
  digitalWrite(PIN_KEY_READ_STROB,LOW);
  pinMode(PIN_IN_DIT, INPUT);
  pinMode(PIN_IN_DAH, INPUT);
  i2c_init();
  // change for required output level
  vfo5351.setup(
    SI5351_CLK0_DRIVE,
    SI5351_CLK1_DRIVE,
    SI5351_CLK2_DRIVE
  );
  vfo5351.set_xtal_freq((SI5351_CALIBRATION/10000)*10000+Settings[ID_SI5351_XTAL]);
  encoder.setup();
  disp.setup();
}

#include "freq_calc.h"

void UpdateBandCtrl()
{
#ifdef BAND_ACTIVE_LEVEL_HIGH
  if (BAND_COUNT <= 4) {
    digitalWrite(PIN_OUT_BAND0, trx.BandIndex == 0);
    digitalWrite(PIN_OUT_BAND1, trx.BandIndex == 1);
    digitalWrite(PIN_OUT_BAND2, trx.BandIndex == 2);
    digitalWrite(PIN_OUT_BAND3, trx.BandIndex == 3);
  } else {
    digitalWrite(PIN_OUT_BAND0, trx.BandIndex & 0x1);
    digitalWrite(PIN_OUT_BAND1, trx.BandIndex & 0x2);
    digitalWrite(PIN_OUT_BAND2, trx.BandIndex & 0x4);
    digitalWrite(PIN_OUT_BAND3, trx.BandIndex & 0x8);
  }
#else
  if (BAND_COUNT <= 4) {
    digitalWrite(PIN_OUT_BAND0, trx.BandIndex != 0);
    digitalWrite(PIN_OUT_BAND1, trx.BandIndex != 1);
    digitalWrite(PIN_OUT_BAND2, trx.BandIndex != 2);
    digitalWrite(PIN_OUT_BAND3, trx.BandIndex != 3);
  } else {
    digitalWrite(PIN_OUT_BAND0, !(trx.BandIndex & 0x1));
    digitalWrite(PIN_OUT_BAND1, !(trx.BandIndex & 0x2));
    digitalWrite(PIN_OUT_BAND2, !(trx.BandIndex & 0x4));
    digitalWrite(PIN_OUT_BAND3, !(trx.BandIndex & 0x8));
  }
#endif
}

struct morse_char_t {
  char ch[7]; 
};

morse_char_t MorseCode[] = {
  {'A', '.', '-',  0,   0,   0,   0},
  {'B', '-', '.', '.', '.',  0,   0},
  {'C', '-', '.', '-', '.',  0,   0},
  {'D', '-', '.', '.',  0,   0,   0},
  {'E', '.',  0,   0,   0,   0,   0},
  {'F', '.', '.', '-', '.',  0,   0},
  {'G', '-', '-', '.',  0,   0,   0},
  {'H', '.', '.', '.', '.',  0,   0},
  {'I', '.', '.',  0,   0,   0,   0},
  {'J', '.', '-', '-', '-',  0,   0},
  {'K', '-', '.', '-',  0,   0,   0},
  {'L', '.', '-', '.', '.',  0,   0},
  {'M', '-', '-',  0,   0,   0,   0},
  {'N', '-', '.',  0,   0,   0,   0},
  {'O', '-', '-', '-',  0,   0,   0},
  {'P', '.', '-', '-', '.',  0,   0},
  {'Q', '-', '-', '.', '-',  0,   0},
  {'R', '.', '-', '.',  0,   0,   0},
  {'S', '.', '.', '.',  0,   0,   0},
  {'T', '-',  0,   0,   0,   0,   0},
  {'U', '.', '.', '-',  0,   0,   0},
  {'V', '.', '.', '.', '-',  0,   0},
  {'W', '.', '-', '-',  0,   0,   0},
  {'X', '-', '.', '.', '-',  0,   0},
  {'Y', '-', '.', '-', '-',  0,   0},
  {'Z', '-', '-', '.', '.',  0,   0},
  {'0', '-', '-', '-', '-', '-',  0},
  {'1', '.', '-', '-', '-', '-',  0},
  {'2', '.', '.', '-', '-', '-',  0},
  {'3', '.', '.', '.', '-', '-',  0},
  {'4', '.', '.', '.', '.', '-',  0},
  {'5', '.', '.', '.', '.', '.',  0},
  {'6', '-', '.', '.', '.', '.',  0},
  {'7', '-', '-', '.', '.', '.',  0},
  {'8', '-', '-', '-', '.', '.',  0},
  {'9', '-', '-', '-', '-', '.',  0},
  {'/', '-', '.', '.', '-', '.',  0},
  {'?', '.', '.', '-', '-', '.', '.'},
  {'.', '.', '-', '.', '-', '.', '-'},
  {',', '-', '-', '.', '.', '-', '-'},
  {0}
};

long last_event = 0;
long last_pool = 0;
uint8_t in_power_save = 0;
uint8_t last_key = 0;
long last_cw = 0;
uint8_t first_call = 1;

enum {imIDLE,imDIT,imDAH};
uint8_t im_state = imIDLE;

void power_save(uint8_t enable)
{
  if (enable) {
    if (!in_power_save) {
      disp.setBright(Settings[ID_DISPLAY_BRIGHT_LOW]);
      in_power_save = 1;
    }
  } else {
    if (in_power_save) {
      disp.setBright(Settings[ID_DISPLAY_BRIGHT_HIGH]);
      in_power_save = 0;
    }
    last_event = millis();
  }
}

uint8_t readDit()
{
  uint8_t dit;
  digitalWrite(PIN_KEY_READ_STROB,HIGH);
  delayMicroseconds(Settings[ID_CW_TOUCH_THRESHOLD]);
  if(digitalRead(PIN_IN_DIT)) dit=0; else dit=1;
  digitalWrite(PIN_KEY_READ_STROB,LOW);
  if (dit) Serial.println("dit");
  return dit;
}

uint8_t readDah()
{
  uint8_t dah;
  digitalWrite(PIN_KEY_READ_STROB,HIGH);
  delayMicroseconds(Settings[ID_CW_TOUCH_THRESHOLD]);
  if(digitalRead(PIN_IN_DAH)) dah=0; else dah=1;
  digitalWrite(PIN_KEY_READ_STROB,LOW);
  if (dah) Serial.println("dah");
  return dah;
}

void sendDit()
{
  outKEY.Write(1);
  OutputTone(PIN_OUT_TONE,Settings[ID_CW_TONE_HZ]);
  delay(trx.dit_time);
  outKEY.Write(0);
  OutputTone(PIN_OUT_TONE,0);
  delay(trx.dit_time);
  last_cw = millis();
}

void sendDah()
{
  outKEY.Write(1);
  OutputTone(PIN_OUT_TONE,Settings[ID_CW_TONE_HZ]);
  delay(trx.dah_time);
  outKEY.Write(0);
  OutputTone(PIN_OUT_TONE,0);
  delay(trx.dit_time);
  last_cw = millis();
}

uint8_t readCWSpeed()
{
  return trx.setCWSpeed(
    Settings[ID_CW_SPEED_MIN] + (long)analogRead(PIN_CW_SPEED_POT)*(Settings[ID_CW_SPEED_MAX]-Settings[ID_CW_SPEED_MIN]+1)/1024,
    Settings[ID_CW_DASH_LEN]
  );
}

void playMessage(char* msg)
{
  char ch;
  while ((ch = *msg++) != 0) {
    if (ch == ' ')
      delay(trx.dit_time*Settings[ID_CW_WORD_SPACE]/10);
    else {
      for(int i=0; MorseCode[i].ch[0] != 0; i++)  {
        if (ch == MorseCode[i].ch[0]) {
          // play letter
          for (uint8_t j=1; j < 7; j++) {
            switch(MorseCode[i].ch[j]) {
              case '.':
                sendDit();
                break;
              case '-':           
                sendDah();
                break;
            }
          }
          delay(trx.dit_time*Settings[ID_CW_LETTER_SPACE]/10);
          break;
        }
      }
    }
    if (readDit() || readDah()) return;
    readCWSpeed();
  }
}

void cwTXOn()
{
  trx.CWTX = 1;
  if (!trx.TX) {
    trx.TX = 1;
    outPTT.Write(1);
    UpdateFreq();
    disp.Draw(trx);
  }
}

void edit_item(uint8_t mi)
{
  int val = Settings[mi];
  disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,val,1);
  if (SettingsDef[mi].id == 99) vfo5351.out_calibrate_freq();
  keypad.waitUnpress();
  while (readDit() || readDah()) ;
  while (1) {
    uint8_t key = keypad.Read();
    if (key == 0 && readDit()) key = 1;
    if (key == 0 && readDah()) key = 2;
    switch (key) {
      case 1:
        // save value
        Settings[mi] = val;
        writeSettings();
        if (SettingsDef[mi].id == 99) vfo5351.set_xtal_freq((SI5351_CALIBRATION/10000)*10000+Settings[ID_SI5351_XTAL]);
        return;
        break;
      case 2:
        // exit
        return;
    }
    long d=encoder.GetDelta();
    if (d < 0) {
      val -= SettingsDef[mi].step;
      if (val < SettingsDef[mi].min_value) val = SettingsDef[mi].min_value;
      disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,val,1);
    } else if (d > 0) {
      val += SettingsDef[mi].step;
      if (val > SettingsDef[mi].max_value) val = SettingsDef[mi].max_value;
      disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,val,1);
    }
  }
}

void show_menu()
{
  uint8_t mi=0;
  disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,Settings[mi],0);
  keypad.waitUnpress();
  while (readDit() || readDah()) ;
  while (1) {
    uint8_t key = keypad.Read();
    if (key == 0 && readDit()) key = 1;
    if (key == 0 && readDah()) key = 2;
    switch (key) {
      case 1:
        if (SettingsDef[mi].id == 0) {
          resetSettings();
          writeSettings();
        } else // edit value
          edit_item(mi);
        disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,Settings[mi],0);
        keypad.waitUnpress();
        while (readDit() || readDah()) ;
        break;
      case 2:
        // exit
        disp.clear();
        return;
        break;
    }
    long d=encoder.GetDelta();
    if (d < 0) {
      if (mi == 0) mi = SETTINGS_COUNT-1;
      else mi--;
      disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,Settings[mi],0);
    } else if (d > 0) {
      mi++;
      if (mi >= SETTINGS_COUNT) mi = 0;
      disp.DrawMenu(SettingsDef[mi].id,SettingsDef[mi].title,Settings[mi],0);
    }
  }
}

void loop()
{
  if (first_call && (keypad.Read() == 1) || readDit() || readDah()) {
    // show menu on startup
    show_menu();
    keypad.waitUnpress();
    while (readDit() || readDah()) ;
  }
  first_call = 0;
  
  if (millis()-last_pool > POOL_INTERVAL) {
    last_pool = millis();
    uint8_t key = keypad.Read();
    if (!last_key && key) {
      // somewhat pressed
      last_key = key;
      switch (key) {
        case 1:
          trx.NextBand();
          break;
        case 2:
          trx.CW = !trx.CW;
          outCW.Write(trx.CW);
          break;
        case 3:
          show_menu();
          keypad.waitUnpress();
          break;
        case 4:
        case 5:
        case 6:
          // MEMO
          if (trx.CW) {
            cwTXOn();
            switch (key) {
              case 4:
                playMessage(MEMO1);
                break;
              case 5:
                playMessage(MEMO2);
                break;
              case 6:
                playMessage(MEMO3);
                break;
            }
          }
          break;
      }
      power_save(0);
    } else if (!key) {
      last_key = 0;
    }
  
    long delta = encoder.GetDelta();
    if (delta) {
      trx.ChangeFreq(delta);
      power_save(0);
    }

    readCWSpeed();
    
    trx.TX = trx.CWTX || inPTT.Read();
    outPTT.Write(trx.TX);
   
    UpdateFreq();
    UpdateBandCtrl();
    disp.Draw(trx);
    
    if (Settings[ID_POWER_DOWN_DELAY] > 0 && millis()-last_event > Settings[ID_POWER_DOWN_DELAY]*1000)
      power_save(1);
  }

  if (trx.TX || Settings[ID_CW_VOX]) {
    if (Settings[ID_CW_IAMBIC]) {
      if (im_state == imIDLE) {
        if (readDit()) im_state = imDIT;
        else if (readDah()) im_state = imDAH;
      }
      if (im_state == imDIT) {
        cwTXOn();
        sendDit();
        //now, if dah is pressed go there, else check for dit
        if (readDah()) im_state = imDAH;
        else {
          if (readDit()) im_state = imDIT;
          else {
            //delay(trx.dit_time*Settings[ID_CW_LETTER_SPACE]/10-trx.dit_time);
            im_state = imIDLE;
          }
        }     
      } else if (im_state == imDAH) {
        cwTXOn();
        sendDah();
        //now, if dit is pressed go there, else check for dah
        if (readDit()) im_state = imDIT;
        else {
          if (readDah()) im_state = imDAH;
          else {
            //delay(trx.dit_time*Settings[ID_CW_LETTER_SPACE]/10-trx.dit_time);
            im_state = imIDLE;
          }
        }
      }
    } else {
      if (readDit()) {
        cwTXOn();
        sendDit();
      } else if (readDah()) {
        cwTXOn();
        sendDah();
      }
    }
  }

  if (trx.CWTX && millis()-last_cw > Settings[ID_BREAK_IN_DELAY]) {
    trx.CWTX = 0;
    trx.TX = inPTT.Read();
    outPTT.Write(trx.TX);
    if (!trx.TX)
      UpdateFreq();
 }
}
