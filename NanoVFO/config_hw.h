#ifndef CONFIG_HW_H
#define CONFIG_HW_H

// раскоментировать используемый дисплей (только один!). 
//#define DISPLAY_1602          // LCD 1602 via I2C
//#define DISPLAY_MAX7219
//#define DISPLAY_OLED128x32     // OLED 0.91" 128x32
#define DISPLAY_OLED128x64     // OLED 0.96" 128x64
//#define DISPLAY_OLED_SH1106_128x64     // OLED 1.3" 128x64 (132x64)

#define I2C_ADR_DISPLAY_1602  0x27

// keypad pool interval in ms
#define POOL_INTERVAL   50

// раскоментировать установленные чипы
#define VFO_SI5351
//#define VFO_SI570

// выбрать в меню калибровку и прописать измеренные частоты на выходах синтезаторов
#define SI5351_CALIBRATION       25000000
#define SI570_CALIBRATION        56319832

// уровень сигнала на выходе Si5351. 0=2mA, 1=4mA, 2=6mA, 3=8mA
#define SI5351_CLK0_DRIVE   0
#define SI5351_CLK1_DRIVE   0
#define SI5351_CLK2_DRIVE   0

// Pin mapping and active levels
#define PIN_OUT_CW    13
#define OUT_CW_ACTIVE_LEVEL  HIGH
#define PIN_OUT_PTT   12
#define OUT_PTT_ACTIVE_LEVEL  HIGH
#define PIN_OUT_KEY   11
#define OUT_KEY_ACTIVE_LEVEL  HIGH

#define PIN_IN_PTT          9
#define PIN_OUT_TONE        10
#define PIN_ANALOG_KEYPAD   A6
#define PIN_CW_SPEED_POT    A7

#define PIN_IN_DIT          4
#define PIN_IN_DAH          5

// строб для "сенсорного" ключа
#define PIN_KEY_READ_STROB  6

#define PIN_OUT_BAND0   17
#define PIN_OUT_BAND1   16
#define PIN_OUT_BAND2   15
#define PIN_OUT_BAND3   14
// HIGH or LOW - active level for PIN_OUT_BAND*
#define BAND_ACTIVE_LEVEL_HIGH
//#define BAND_ACTIVE_LEVEL_LOW

// раскоментировать ТОЛЬКО ОДИН требуемый тип энкодера. закоментировать все если нет
//#define ENCODER_OPTICAL
#define ENCODER_MECHANIC

#if defined(ENCODER_OPTICAL) && defined(ENCODER_MECHANIC)
  #error You need select single encoder
#endif

// изменение частоты в Гц на один оборот в обычном режиме
#define ENCODER_FREQ_LO_STEP      3000
// изменение частоты в Гц на один оборот в ускоренном режиме
#define ENCODER_FREQ_HI_STEP      15000
// порог переключения в ускоренный режим. если частота изменится более
// чем на ENCODER_FREQ_HI_LO_TRASH Гц за секунду то переходим в ускоренный режим
#define ENCODER_FREQ_HI_LO_TRASH  2000

#ifdef ENCODER_OPTICAL
#define ENCODER_ENABLE
// количество импульсов на оборот примененного оптического энкодера
#define ENCODER_PULSE_PER_TURN    360
#endif

#ifdef ENCODER_MECHANIC
#define ENCODER_ENABLE
// количество импульсов на оборот примененного механического энкодера
#define ENCODER_PULSE_PER_TURN    20
#endif

#endif
