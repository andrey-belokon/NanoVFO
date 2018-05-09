<h2>Nano VFO for portable TRX</h2>

CPU: Arduino ProMini<br>
PLL: Si5351 or/and Si570<br>
Display: OLED 0.91" 128x32, OLED 0.96" 128x64, OLED 1.3" 128/132x64, I2C LCD 1602, MAX7219<br>
Encoder: optical/mechanic rotary encode
Keypad: 0..6 buttons
Support different TRX architecture:<br>
 1. Single  IF superheterodyne.
 2. Direct conversion with 2x or 4x output.
 3. Direct conversion with quadrature output.
Builtin CW key: auto/iamboc mode, 3 phrase memory, CW-VOX, touch mode

Project homepage http://dspview.com/viewtopic.php?t=202
Required libraries:<br>
 1. UR5FFR_Si5351 https://github.com/andrey-belokon/UR5FFR_Si5351
 2. SSD1306Ascii - install from Arduino IDE

What's new in version 2.0<br>
 1. Save current freq/band to EEPROM and restore on star
 2. Improved encoder unit. 4x increase pulses for mechanical encoder
 3. New Si5351 library
 4. Change power to 5v without I2C level conversion
 5. Many minor bugfixes

<img src="https://github.com/andrey-belokon/NanoVFO/raw/master/Doc/Schematic_Nano-VFO.png"></img>

<img src="https://github.com/andrey-belokon/NanoVFO/raw/master/Doc/dislpay_freq.jpg"></img>

Copyright (c) 2016-2020, Andrii Bilokon, UR5FFR<br>
License GNU GPL, see license.txt for more information