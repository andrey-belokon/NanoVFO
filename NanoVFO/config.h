#ifndef CONFIG_H
#define CONFIG_H

#define LSB 0
#define USB 1

// число диапазонов
#define BAND_COUNT  3

extern const struct _Bands {
  uint8_t   mc;
  long  start, startSSB, end;
  uint8_t sideband;
} Bands[];

#define DEFINED_BANDS \
  {80,   3500000L,  3600000L,  3800000L, LSB}, \
  {40,   7000000L,  7045000L,  7200000L, LSB}, \
  {20,  14000000L, 14100000L, 14350000L, USB}

struct _Settings {
  int id;
  int def_value;
  int min_value;
  int max_value;
  int step;
  char* title;
};

#define SETTINGS_COUNT  17

#define SETTINGS_DATA \
  {10,   8,   0,   60,  1, "PWR DWN DELAY"},    /* через сколько времени переходить в режим сохранения питания, сек*/ \
  {11,   7,   1,   15,  1, "BRIGHT HIGH"},      /* яркость LCD - 0..15 в обычнойм режиме и powerdown (0 - погашен) */ \
  {12,   1,   0,   15,  1, "BRIGHT LOW"},       /* 0 - постоянно включен */ \
  \
  {20, 700, 300, 2000, 10, "CW TONE"},      /* частота самоконтроля и сдвиг частоты на CLK2 для формирования CW-сигнала */ \
  {21,  0,  0,   1, 1, "IAMBIC"},           /* switch between iambic and auto mode of cw keyer */ \
  {22,  6,  2,  30, 1, "TOUCH THRESHOLD"},  /* задержка в мкс при опросе сенсора ключа */ \
  {23,  6,  1,  20, 1, "CW SPEED MIN"},     /* min/max cw speed in WPM */ \
  {24, 25,  5,  35, 1, "CW SPEED MAX"}, \
  {25, 30, 20,  40, 1, "DASH-DOT RATIO"},   /* length of dash (in dots) *10 */ \
  {26, 30, 20,  60, 1, "LETTER SPACE"},     /* length of space between letters (in dots) *10 */ \
  {27, 70, 30, 120, 1, "WORD SPACE"},       /* length of space between words (in dots) *10 */ \
  \
  {30,   1, 0, 1, 1, "CW VOX"},                 /* auto turn into TX on keyer press (on if in CW mode). if undef you need external PTT */ \
  {31, 800, 100, 2000, 10, "BREAK IN DELAY"},   /* break-in hang time (mS) in CW VOX mode. turn off TX if keyer not pressed more than BREAK_IN_DELAY time */ \
  \
  {41, 0, -10000, 10000, 10, "LSB SHIFT"},    /* доп.сдвиг второго гетеродина относительно констант SSBDetectorFreq_LSB/SSBDetectorFreq_USB */ \
  {42, 0, -10000, 10000, 10, "USB SHIFT"}, \
  {99, 3000, -20000, 20000, 5, "SI5351 XTAL"}, \
  {0, 0, 0, 0, 0, "FULL RESET"}

// increase for reset stored to EEPROM settings values to default
#define SETTINGS_VERSION    0x02

// id for fast settings access
enum {
  ID_POWER_DOWN_DELAY = 0,
  ID_DISPLAY_BRIGHT_HIGH,
  ID_DISPLAY_BRIGHT_LOW,
  ID_CW_TONE_HZ,
  ID_CW_IAMBIC,
  ID_CW_TOUCH_THRESHOLD,
  ID_CW_SPEED_MIN,
  ID_CW_SPEED_MAX,
  ID_CW_DASH_LEN,
  ID_CW_LETTER_SPACE,
  ID_CW_WORD_SPACE,
  ID_CW_VOX,
  ID_BREAK_IN_DELAY,
  ID_LSB_SHIFT,
  ID_USB_SHIFT,
  ID_SI5351_XTAL
};

#define MEMO1   "CQ CQ CQ DE UR5FFR UR5FFR UR5FFR K"
#define MEMO2   "CQ CQ CQ DE UR5FFR/P UR5FFR/P UR5FFR/P K"
#define MEMO3   "TNX FER CALL = UR RST 599 599 = NAME IS ANDREY ANDREY QTH ODESSA ODESSA = HW?"

// конфиг "железа"
#include "config_hw.h"

// настройки генерируемых частот 
#include "config_sw.h"

#endif
