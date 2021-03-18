/*
  xdsp_15_tm1637.ino - Support for TM1637- , TM1638- and MAX7219- based seven-segment displays for Tasmota

  Copyright (C) 2021  Ajith Vasudevan

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_DISPLAY
#ifdef USE_DISPLAY_MAX7219
/*********************************************************************************************\
  This driver enables the display of numbers (both integers and floats) and basic text
  on the inexpensive TM1637- , TM1638- and MAX7219-based seven-segment modules. Additionally, for the TM1638, 
  it allows the TM1638 LEDs to be toggled using its buttons. 
  Useful STAT messages are also sent when the TM1638 buttons are pressed.
  
  Raw segments can also be displayed.
  
  In addition, it is also possible to set brightness (8 levels), clear the display, scroll text,  
  display a rudimentary bar graph, and a Clock (12 hr and 24 hr).

  To use, compile Tasmota with USE_DISPLAY and USE_DISPLAY_TM1637, or build the tasmota-display env.

  For TM1637:
  Connect the TM1637 display module's pins to any free GPIOs of the ESP8266 module
  and assign the pins as follows from Tasmota's GUI:

  DIO hardware pin --> "TM1637 DIO"
  CLK hardware pin --> "TM1637 CLK"

  For TM1638:
  Connect the TM1638 display module's pins to any free GPIOs of the ESP8266 module
  and assign the pins as follows from Tasmota's GUI:

  DIO hardware pin --> "TM1638 DIO"
  CLK hardware pin --> "TM1638 CLK"
  STB hardware pin --> "TM1638 STB"

  For MAX7219:
  Connect the <AX7219 display module's pins to any free GPIOs of the ESP8266 module
  and assign the pins as follows from Tasmota's GUI:

  DIN hardware pin --> "MAX7219 DIN"
  CS hardware pin --> "MAX7219 CS"
  CLK hardware pin --> "MAX7219 CLK"

  
  Once the GPIO configuration is saved and the ESP8266/ESP32 module restarts, set the Display Model to 15 
  using the command "DisplayModel 15"
  
  After this, depending upon your display variant, set your Display Type to 0, 1 or 2 using the command 
     "DisplayType 0"  // for the 4-digit TM1637
  or "DisplayType 1"  // for the 6-digit TM1637
  or "DisplayType 2"  // for the 8-digit TM1638
  or "DisplayType 3"  // for the 8-digit MAX7219
  
  After the ESP8266/ESP32 module restarts again, the following "Display" commands can be used:
  

  DisplayType           type {0|1|2}

                               Sets the display type. 0 => TM1637, 4 digit
                                                      1 => TM1637, 6 digit
                                                      2 => TM1638, 8 digit
                                                      3 => MAX7219, 8 digit

                               Command e.g., "MAX7219Data.display_type 1"   // to enable TM1637 6-digit variant
  


  DisplayClear

                               Clears the display, command: "DisplayClear"


  DisplayNumber         num [,position {0-(MAX7219Data.num_digits-1))} [,leading_zeros {0|1} [,length {1 to MAX7219Data.num_digits}]]]

                               Clears and then displays number without decimal. command e.g., "DisplayNumber 1234"
                               Control 'leading zeros', 'length' and 'position' with  "DisplayNumber 1234, <position>, <leadingZeros>, <length>"
                               'leading zeros' can be 1 or 0 (default), 'length' can be 1 to MAX7219Data.num_digits, 'position' can be 0 (left-most) to MAX7219Data.num_digits (right-most).
                               See function description below for more details.

  DisplayNumberNC       num [,position {0-(MAX7219Data.num_digits-1))} [,leading_zeros {0|1} [,length {1 to MAX7219Data.num_digits}]]]

                               Display integer number as above, but without clearing first. e.g., "DisplayNumberNC 1234". Usage is same as above.



  DisplayFloat          num [,position {0-(MAX7219Data.num_digits-1)} [,precision {0-MAX7219Data.num_digits} [,length {1 to MAX7219Data.num_digits}]]]

                               Clears and then displays float (with decimal point)  command e.g., "DisplayFloat 12.34"
                               See function description below for more details.



  DisplayFloatNC        num [,position {0-(MAX7219Data.num_digits-1)} [,precision {0-MAX7219Data.num_digits} [,length {1 to MAX7219Data.num_digits}]]]

                               Displays float (with decimal point) as above, but without clearing first. command e.g., "DisplayFloatNC 12.34"
                               See function description below for more details.



  DisplayBrightness     num {1-8}

                               Set brightness (1 to 8) command e.g., "DisplayBrightness 2"



  DisplayRaw            position {0-(MAX7219Data.num_digits-1)},length {1 to MAX7219Data.num_digits}, num1 [, num2[, num3[, num4[, ...upto MAX7219Data.num_digits numbers]]]]]

                               Takes upto MAX7219Data.num_digits comma-separated integers (0-255) and displays raw segments. Each number represents a
                               7-segment digit. Each 8-bit number represents individual segments of a digit.
                               For example, the command "DisplayRaw 0, 4, 255, 255, 255, 255" would display "[8.8.8.8.]"



  DisplayText           text [, position {0-(MAX7219Data.num_digits-1)} [,length {1 to MAX7219Data.num_digits}]]

                               Clears and then displays basic text.  command e.g., "DisplayText ajith vasudevan"
                               Control 'length' and 'position' with  "DisplayText <text>, <position>, <length>"
                               'length' can be 1 to MAX7219Data.num_digits, 'position' can be 0 (left-most) to MAX7219Data.num_digits-1 (right-most)
                               A caret(^) symbol in the text input is dispayed as the degrees(°) symbol. This is useful for displaying Temperature!
                               For example, the command "DisplayText 22.5^" will display "22.5°".


  DisplayTextNC         text [, position {0-MAX7219Data.num_digits-1} [,length {1 to MAX7219Data.num_digits}]]

                               Clears first, then displays text. Usage is same as above.



  DisplayScrollText     text

                              Displays scrolling text.



  DisplayScrollDelay delay {0-15}   // default = 4

                               Sets the speed of text scroll. Smaller delay = faster scrolling.



  DisplayLevel          num {0-100}

                               Display a horizontal bar graph (0-100) command e.g., "DisplayLevel 50" will display [||||    ]



  DisplayClock  1|2|0

                               Displays a clock.
                               Commands "DisplayClock 1"     // 12 hr format
                                        "DisplayClock 2"     // 24 hr format
                                        "DisplayClock 0"     // turn off clock



Commands specific to TM1638
====================================

  DisplaySetLEDs        bit_array {0-255} 

                               Sets the 8 LEDs (not the digits!) on the TM1638 module to the binary number represented by the input bit_array.
                               For example, "DisplaySetLEDs 3" would light up the first and second LED (from left), because 3 => 00000011



  DisplaySetLED         position {0-7}, value {0|1}

                                Sets a specified LED to either ON or OFF.  e.g., "DisplaySetLED 2, 1" would light up the 3rd LED (2 => 3rd position)



  DisplayButtons

                                Causes the current state of the buttons/LEDs to be returned as a "STAT" message of the following form:
                                stat/TM1638/RESULT = {"DisplayButtons": {"S1":<s1>, "S2":<s2>, "S3":<s3>, "S4":<s4>, "S5":<s5>, "S6":<s6>, "S7":<s7>, "S8":<s8>, "Array": [<s1>,<s2>,<s3>,<s4>,<s5>,<s6>,<s7>,<s8>], "Byte": <byte>}}
                                where <s1>, <s2>, ... are the states of each of the buttons/LEDs, and <byte> is the decimal representation 
                                of the bit-array that constitutes these states. 

                                For example, if the 5th button is pressed to turn ON LED5, and the "DisplayButtons" command is issued, 
                                the response STAT message will be of the form:
                                stat/TM1638/RESULT = {"DisplayButtons": {"S1":0, "S2":0, "S3":0, "S4":0, "S5":1, "S6":0, "S7":0, "S8":0, "Array": [0,0,0,0,1,0,0,0], "Byte": 16}}
                                because 16 => 2^(5-1)



  Button Functionality (TM1638 only):
======================================
  When this driver is initialized with "DisplayType 2" (TM1638), the buttons on the TM1638 module can be used 
  to toggle the corresponding LEDs.

  In addition, for each button press, a STAT message of the following form is sent:

  stat/<Topic>/RESULT = {"TM1638_BUTTON<x>":<state>}

      where    <x> = button index (starting at 1)
           <state> = state of the corresponding LED. It can be "ON" or "OFF" 

  For example, if Button3 is pressed, LED3 would light up and a STAT message is sent as follows:
  
  stat/TM1638/RESULT = {"TM1638_BUTTON3":"ON"}

  Pressing Button3 again would turn OFF LED3 and a STAT message is sent as follows:

  stat/TM1638/RESULT = {"TM1638_BUTTON3":"OFF"}


\*********************************************************************************************/

#define XDSP_16           16

#define BRIGHTNESS_MIN    1
#define BRIGHTNESS_MAX    8
#define CMD_MAX_LEN       55
#define LEVEL_MIN         0
#define LEVEL_MAX         100
#define SCROLL_MAX_LEN    50
#define POSITION_MIN    0
#define POSITION_MAX    8
#define LED_MIN      0
#define LED_MAX      255
#define MAX7219_ADDR 0

#include <LedControl.h>

LedControl *max7219display;

struct {
  char scroll_text[CMD_MAX_LEN];
  char msg[60];
  char model_name[8];
  uint8_t num_digits = 4;
  uint8_t scroll_delay = 4;
  uint8_t scroll_index = 0;
  uint8_t iteration = 0;
  uint8_t brightness = 5;
  uint8_t display_type = MAX7219;
  uint8_t prev_buttons;

  bool driver_inited = false;
  bool scroll = false;
  bool show_clock = false;
  bool clock_24 = false;
} MAX7219Data;


/*********************************************************************************************\
* Init function
\*********************************************************************************************/
bool MAXDriverInit(void) {
  if(Settings.display_model == XDSP_16) {
    if(MAX7219Data.driver_inited) return true;

    Settings.display_options.data = 3;
    MAX7219Data.num_digits = 8; 
    MAX7219Data.display_type = MAX7219;

    strcpy(MAX7219Data.model_name, "MAX7219");
    max7219display = new LedControl(Pin(GPIO_MAX7219DIN), Pin(GPIO_MAX7219CLK), Pin(GPIO_MAX7219CS), 1 );
    max7219display->shutdown(MAX7219_ADDR,false);

    Settings.display_size = MAX7219Data.num_digits;   // Can use to check current display size
    maxClearDisplay();
    MAX7219Data.brightness = (Settings.display_dimmer ? Settings.display_dimmer : MAX7219Data.brightness);
    maxSetBrightness(MAX7219Data.brightness);
    MAX7219Data.driver_inited = true;
    AddLog(LOG_LEVEL_INFO, PSTR("DSP: %s display driver initialized with %d digits (DisplayType %d)"), MAX7219Data.model_name, MAX7219Data.num_digits, Settings.display_options.data);    
  }

  return true;
}

// Function to display specified ascii char at specified position for MAX7219
void displayMAX7219ASCII(uint8_t pos, char c) {
  pos = 7 - pos;
  max7219display->setChar(MAX7219_ADDR, pos, c, false);
}

// Function to display specified ascii char with dot at specified position for MAX7219
void displayMAX7219ASCIIwDot(uint8_t pos, char c) {
  pos = 7 - pos;
  max7219display->setChar(MAX7219_ADDR, pos, c, true);
}

// Function to display raw segments at specified position for MAX7219
void displayMAX72197Seg(uint8_t pos, uint8_t seg) {
  bool dec_bit = seg & 128;
  seg = seg << 1;
  seg = seg | dec_bit;    
  uint8_t NO_OF_BITS = 8; 
  uint8_t reverse_num = 0;
  for (uint8_t i = 0; i < NO_OF_BITS; i++) 
  { 
    if((seg & (1 << i))) 
    reverse_num |= 1 << ((NO_OF_BITS - 1) - i);   
  } 
  seg = reverse_num;

  pos = 7 - pos;
  max7219display->setRow(MAX7219_ADDR, pos, seg);
}


/*********************************************************************************************\
* Displays number without decimal, with/without leading zeros, specifying start-position
* and length, optionally skipping clearing display before displaying the number.
* commands: DisplayNumber   num [,position {0-(MAX7219Data.num_digits-1)} [,leading_zeros {0|1} [,length {1 to MAX7219Data.num_digits}]]]
*           DisplayNumberNC num [,position {0-(MAX7219Data.num_digits-1)} [,leading_zeros {0|1} [,length {1 to MAX7219Data.num_digits}]]]    // "NC" --> "No Clear"
\*********************************************************************************************/
bool MAXCmndNumber(bool clear) {
  char sNum[CMD_MAX_LEN];
  char sLeadingzeros[CMD_MAX_LEN];
  char sPosition[CMD_MAX_LEN];
  char sLength[CMD_MAX_LEN];
  uint8_t length = 0;
  bool leadingzeros = false;
  uint8_t position = 0;

  uint32_t num = 0;

  switch (ArgC())
  {
    case 4 :
      subStr(sLength, XdrvMailbox.data, ",", 4);
      length = atoi(sLength);
    case 3 :
      subStr(sLeadingzeros, XdrvMailbox.data, ",", 3);
      leadingzeros = atoi(sLeadingzeros);
    case 2 :
      subStr(sPosition, XdrvMailbox.data, ",", 2);
      position = atoi(sPosition);
    case 1 :
      subStr(sNum, XdrvMailbox.data, ",", 1);
      num = atof(sNum);
  }


  if((position < 0) || (position > (MAX7219Data.num_digits-1))) position = 0;

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: num %d, pos %d, lead %d, len %d"), num, position, leadingzeros, length);

  if(clear) maxClearDisplay();

  char txt[30];
  snprintf_P(txt, sizeof(txt), PSTR("%d"), num);
  if(!length) length = strlen(txt);
  if((length < 0) || (length > MAX7219Data.num_digits)) length = MAX7219Data.num_digits;

  char pad = (leadingzeros ? '0': ' ');
  uint32_t i = position;
  uint8_t rawBytes[1];

  for(; i<position + (length - strlen(txt)); i++) {
    if(i>MAX7219Data.num_digits) break;
    displayMAX7219ASCII(i, pad);
  }

  for(uint32_t j = 0; i< position + length; i++, j++) {
    if(i>MAX7219Data.num_digits) break;
    if(txt[j] == 0) break;
    displayMAX7219ASCII(i, txt[j]);
  }

  return true;
}

/*********************************************************************************************\
* Displays number with decimal, specifying position, precision and length,
* optionally skipping clearing display before displaying the number.
* commands: DisplayFloat   num [,position {0-(MAX7219Data.num_digits-1)} [,precision {0-MAX7219Data.num_digits} [,length {1 to MAX7219Data.num_digits}]]]
*           DisplayFloatNC num [,position {0-(MAX7219Data.num_digits-1)} [,precision {0-MAX7219Data.num_digits} [,length {1 to MAX7219Data.num_digits}]]]    // "NC" --> "No Clear"
\*********************************************************************************************/
bool MAXCmndFloat(bool clear) {

  char sNum[CMD_MAX_LEN];
  char sPrecision[CMD_MAX_LEN];
  char sPosition[CMD_MAX_LEN];
  char sLength[CMD_MAX_LEN];
  uint8_t length = 0;
  uint8_t precision = MAX7219Data.num_digits;
  uint8_t position = 0;

  float fnum = 0.0f;

  switch (ArgC())
  {
    case 4 :
      subStr(sLength, XdrvMailbox.data, ",", 4);
      length = atoi(sLength);
    case 3 :
      subStr(sPrecision, XdrvMailbox.data, ",", 3);
      precision = atoi(sPrecision);
    case 2 :
      subStr(sPosition, XdrvMailbox.data, ",", 2);
      position = atoi(sPosition);
    case 1 :
      subStr(sNum, XdrvMailbox.data, ",", 1);
      fnum = atof(sNum);
  }


  if((position < 0) || (position > (MAX7219Data.num_digits-1))) position = 0;
  if((precision < 0) || (precision > MAX7219Data.num_digits)) precision = MAX7219Data.num_digits;

  if(clear) maxClearDisplay();

  char txt[30];
  ext_snprintf_P(txt, sizeof(txt), PSTR("%*_f"), precision, &fnum);

  if(!length) length = strlen(txt);
  if((length <= 0) || (length > MAX7219Data.num_digits)) length = MAX7219Data.num_digits;

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: num %4_f, prec %d, len %d"), &fnum, precision, length);

  for(uint32_t i=0, j=0; i<length; i++, j++) {
    if((j+position) > 7) break;
    if(txt[i] == 0) break;
    if(txt[i+1] == '.') { 
      displayMAX7219ASCIIwDot(j+position, txt[i]);
      i++;
      length++;
    }
    else displayMAX7219ASCII(j+position, txt[i]);
  }
  return true;
}


// /*********************************************************************************************\
// * Clears the display
// * Command:  DisplayClear
// \*********************************************************************************************/
bool MAXCmndClear(void) {
  maxClearDisplay();
  sprintf(MAX7219Data.msg, PSTR("Cleared"));
  XdrvMailbox.data = MAX7219Data.msg;
  return true;
}


// /*********************************************************************************************\
// * Clears the display
// \*********************************************************************************************/
void maxClearDisplay (void) {
  max7219display->clearDisplay(MAX7219_ADDR);
}


/*********************************************************************************************\
* Display scrolling text
* Command:   DisplayMAX7219Data.scroll_text text
\*********************************************************************************************/
bool MAXCmndScrollText(void) {

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: Text %s"), XdrvMailbox.data);

  if(XdrvMailbox.data_len > SCROLL_MAX_LEN) {
    snprintf(MAX7219Data.msg, sizeof(MAX7219Data.msg), PSTR("Text too long. Length should be less than %d"), SCROLL_MAX_LEN);
    XdrvMailbox.data = MAX7219Data.msg;
    return false;
  } else {
    snprintf(MAX7219Data.scroll_text, sizeof(MAX7219Data.scroll_text), PSTR("                                                       "));
    snprintf(MAX7219Data.scroll_text, sizeof(MAX7219Data.scroll_text), PSTR("%s"), XdrvMailbox.data);
    MAX7219Data.scroll_text[XdrvMailbox.data_len] = 0;
    MAX7219Data.scroll_index = 0;
    MAX7219Data.scroll = true;
    return true;
  }

}



/*********************************************************************************************\
* Sets the scroll delay for scrolling text.
* Command:  DisplayMAX7219Data.scroll_delay delay {0-15}    // default = 4
\*********************************************************************************************/
bool MAXCmndScrollDelay(void) {
  if(ArgC() == 0) {
    XdrvMailbox.payload = MAX7219Data.scroll_delay;
    return true;
  }
  if(MAX7219Data.scroll_delay<0) MAX7219Data.scroll_delay=0;
  MAX7219Data.scroll_delay = XdrvMailbox.payload;
  return true;
}



/*********************************************************************************************\
* Scrolls a given string. Called every 50ms
\*********************************************************************************************/
void maxScrollText(void) {
  MAX7219Data.iteration++;
  if(MAX7219Data.scroll_delay) MAX7219Data.iteration = MAX7219Data.iteration %  MAX7219Data.scroll_delay;
  else MAX7219Data.iteration = 0;
  if(MAX7219Data.iteration) return;

  if(MAX7219Data.scroll_index > strlen(MAX7219Data.scroll_text)) {
    MAX7219Data.scroll= false;
    MAX7219Data.scroll_index = 0;
    return;
  }
  uint8_t rawBytes[1];
  for(uint32_t i=0, j=MAX7219Data.scroll_index; i< 1 + strlen(MAX7219Data.scroll_text); i++, j++) {
    if(i > (MAX7219Data.num_digits-1)) { break; }
    rawBytes[0] = tm1637display->encode(MAX7219Data.scroll_text[j]);
    bool dotSkipped = false;
    if(MAX7219Data.scroll_text[j+1] == '.') {
      dotSkipped = true;
      rawBytes[0] = rawBytes[0] | 128;
      j++;
    } else if(MAX7219Data.scroll_text[j] == '^') {
      rawBytes[0] = 1 | 2 | 32 | 64;
    }
    if(!dotSkipped && MAX7219Data.scroll_text[j] == '.') {
      j++; 
      MAX7219Data.scroll_index++; 
      rawBytes[0] = tm1637display->encode(MAX7219Data.scroll_text[j]);
    }
    if(MAX7219Data.scroll_text[j+1] == '.') { rawBytes[0] = rawBytes[0] | 128; }
    displayMAX72197Seg(i, rawBytes[0]);
  }
  MAX7219Data.scroll_index++;
}

/*********************************************************************************************\
* Displays a horizontal bar graph. Takes a percentage number (0-100) as input
* Command:   DisplayLevel level {0-100}
\*********************************************************************************************/
bool MAXCmndLevel(void) {
  uint16_t val = XdrvMailbox.payload;
  if((val < LEVEL_MIN) || (val > LEVEL_MAX)) {
    Response_P(PSTR("{\"Error\":\"Level should be a number in the range [%d, %d]\"}"), LEVEL_MIN, LEVEL_MAX);
    return false;
  }

  uint8_t totalBars = 2*MAX7219Data.num_digits;
  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: MAX7219Data.model_name %s MAXCmndLevel totalBars=%d"), MAX7219Data.model_name, totalBars);
  float barsToDisplay = totalBars * val / 100.0f;
  char txt[5];
  ext_snprintf_P(txt, sizeof(txt), PSTR("%*_f"), 1, &barsToDisplay);
  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: MAX7219Data.model_name %s MAXCmndLevel barsToDisplay=%s"), MAX7219Data.model_name, txt);
  char s[4];
  ext_snprintf_P(s, sizeof(s), PSTR("%0_f"), &barsToDisplay);
  uint8_t numBars = atoi(s);
  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: CmndTM1637Level numBars %d"), numBars);

  maxClearDisplay();
  uint8_t rawBytes[1];
  for(int i=1; i<=numBars; i++) {
    uint8_t digit = (i-1) / 2;
    uint8_t value = (((i%2) == 0) ? 54 : 48);
    displayMAX72197Seg(digit, value);
  }
  return true;
}

/*********************************************************************************************\
* Display arbitrary data on the display module
* Command:   DisplayRaw position {0-(MAX7219Data.num_digits-1)},length {1 to MAX7219Data.num_digits}, a [, b[, c[, d[...upto MAX7219Data.num_digits]]]]
* where a,b,c,d... are upto MAX7219Data.num_digits numbers in the range 0-255, each number (byte)
* corresponding to a single 7-segment digit. Within each byte, bit 0 is segment A,
* bit 1 is segment B etc. The function may either set the entire display
* or any desired part using the length and position parameters.
\*********************************************************************************************/
bool MAXCmndRaw(void) {
  uint8_t DATA[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  char as[CMD_MAX_LEN];
  char bs[CMD_MAX_LEN];
  char cs[CMD_MAX_LEN];
  char ds[CMD_MAX_LEN];
  char es[CMD_MAX_LEN];
  char fs[CMD_MAX_LEN];
  char gs[CMD_MAX_LEN];
  char hs[CMD_MAX_LEN];

  char sLength[CMD_MAX_LEN];
  char sPos[CMD_MAX_LEN];


  uint32_t position = 0;
  uint32_t length = 0;

  switch (ArgC())
  {
    case 10 :
      subStr(hs, XdrvMailbox.data, ",", 10);
      DATA[7] = atoi(hs);
    case 9 :
      subStr(gs, XdrvMailbox.data, ",", 9);
      DATA[6] = atoi(gs);
    case 8 :
      subStr(fs, XdrvMailbox.data, ",", 8);
      DATA[5] = atoi(fs);
    case 7 :
      subStr(es, XdrvMailbox.data, ",", 7);
      DATA[4] = atoi(es);
    case 6 :
      subStr(ds, XdrvMailbox.data, ",", 6);
      DATA[3] = atoi(ds);
    case 5 :
      subStr(cs, XdrvMailbox.data, ",", 5);
      DATA[2] = atoi(cs);
    case 4 :
      subStr(bs, XdrvMailbox.data, ",", 4);
      DATA[1] = atoi(bs);
    case 3 :
      subStr(as, XdrvMailbox.data, ",", 3);
      DATA[0] = atoi(as);
    case 2 :
      subStr(sLength, XdrvMailbox.data, ",", 2);
      length = atoi(sLength);
    case 1 :
      subStr(sPos, XdrvMailbox.data, ",", 1);
      position = atoi(sPos);
  }

  if(!length) length = ArgC() - 2;
  if(length < 0 || length > MAX7219Data.num_digits) length = MAX7219Data.num_digits;
  if(position < 0 || position > (MAX7219Data.num_digits-1)) position = 0;

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: a %d, b %d, c %d, d %d, e %d, f %d, g %d, h %d, len %d, pos %d"),
    DATA[0], DATA[1], DATA[2], DATA[3], DATA[4], DATA[5], DATA[6], DATA[7], length, position);

  for(uint32_t i=position; i<position+length; i++ ) {
    if(i>7) break;
    displayMAX72197Seg(i, DATA[i-position]);
  }

  return true;
}

/*********************************************************************************************\
* Display a given string.
* Text can be placed at arbitrary location on the display using the length and
* position parameters without affecting the rest of the display.
* Command:   DisplayText text [, position {0-(MAX7219Data.num_digits-1)} [,length {1 to MAX7219Data.num_digits}]]
\*********************************************************************************************/
bool MAXCmndText(bool clear) {
  char sString[CMD_MAX_LEN + 1];
  char sPosition[CMD_MAX_LEN];
  char sLength[CMD_MAX_LEN];
  uint8_t length = 0;
  uint8_t position = 0;

  switch (ArgC())
  {
    case 3 :
      subStr(sLength, XdrvMailbox.data, ",", 3);
      length = atoi(sLength);
    case 2 :
      subStr(sPosition, XdrvMailbox.data, ",", 2);
      position = atoi(sPosition);
    case 1 :
      subStr(sString, XdrvMailbox.data, ",", 1);
  }


  if((position < 0) || (position > (MAX7219Data.num_digits-1))) position = 0;

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: sString %s, pos %d, len %d"), sString, position, length);

  if(clear) maxClearDisplay();

  if(!length) length = strlen(sString);
  if((length < 0) || (length > MAX7219Data.num_digits)) length = MAX7219Data.num_digits;

  uint32_t i = position;
  uint8_t rawBytes[1];
  for(uint32_t j = 0; i< position + length; i++, j++) {
    if(i > (MAX7219Data.num_digits-1)) break;
    if(sString[j] == 0) break;
    rawBytes[0] = tm1637display->encode(sString[j]);
    bool dotSkipped = false;
    if(sString[j+1] == '.') {
      dotSkipped = true;
      rawBytes[0] = rawBytes[0] | 128;
      j++;
    } else if(sString[j] == '^') {
      rawBytes[0] = 1 | 2 | 32 | 64;
    }
    if(!dotSkipped && sString[j] == '.') rawBytes[0] = 128;
    displayMAX72197Seg(i, rawBytes[0]);
  }

  return true;
}


/*********************************************************************************************\
* Sets brightness of the display.
* Command:  DisplayBrightness {1-8}
\*********************************************************************************************/
bool MAXCmndBrightness(void) {

  uint16_t val = XdrvMailbox.payload;
  if(ArgC() == 0) {
    XdrvMailbox.payload = MAX7219Data.brightness;
    return true;
  }

  if((val < BRIGHTNESS_MIN) || (val > BRIGHTNESS_MAX)) {
    Response_P(PSTR("{\"Error\":\"Brightness should be a number in the range [%d, %d]\"}"), BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    return false;
  }
  MAX7219Data.brightness = val;
  maxSetBrightness(MAX7219Data.brightness);
  return true;
}



void maxSetBrightness(uint8_t val) {
  if((val < BRIGHTNESS_MIN) || (val > BRIGHTNESS_MAX)) val = 5;
  Settings.display_dimmer = val;
  max7219display->setIntensity(MAX7219_ADDR, val-1);
}





/*********************************************************************************************\
* Displays a clock.
* Command: DisplayClock 1   // 12-hour format
*          DisplayClock 2   // 24-hour format
*          DisplayClock 0   // turn off clock and clear
\*********************************************************************************************/
bool MAXCmndClock(void) {

  MAX7219Data.show_clock = XdrvMailbox.payload;

  if(ArgC() == 0) XdrvMailbox.payload = 1;
  if(XdrvMailbox.payload > 1) MAX7219Data.clock_24 = true;
  else if(XdrvMailbox.payload == 1) MAX7219Data.clock_24 = false;

  AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: MAX7219Data.show_clock %d, MAX7219Data.clock_24 %d"), MAX7219Data.show_clock, MAX7219Data.clock_24);

  maxClearDisplay();
  return true;
}


/*********************************************************************************************\
* refreshes the time if clock is displayed
\*********************************************************************************************/
void maxShowTime() {
  uint8_t hr = RtcTime.hour;
  uint8_t mn = RtcTime.minute;
  // uint8_t hr = 1;
  // uint8_t mn = 0;
  char z = ' ';
  if(MAX7219Data.clock_24) {
    z = '0';
  } else {
    if(hr > 12) hr -= 12;
    if(hr == 0) hr = 12;
  }

  char tm[5];
  if(hr < 10) {
    if(mn < 10) snprintf(tm, sizeof(tm), PSTR("%c%d0%d"), z, hr, mn);
    else snprintf(tm, sizeof(tm), PSTR("%c%d%d"), z, hr, mn);
  } else {
    if(mn < 10) snprintf(tm, sizeof(tm), PSTR("%d0%d"), hr, mn);
    else snprintf(tm, sizeof(tm), PSTR("%d%d"), hr, mn);
  }

  for(uint32_t i = 0; i< 4; i++) {
    if((millis() % 1000) > 500 && (i == 1)) displayMAX7219ASCIIwDot(i, tm[i]);
    else displayMAX7219ASCII(i, tm[i]);
  }

}



/*********************************************************************************************\
* This function is called for all Display functions.
\*********************************************************************************************/
bool MAXMainFunc(uint8_t fn) {
  bool result = false;

  if(XdrvMailbox.data_len > CMD_MAX_LEN) {
    Response_P(PSTR("{\"Error\":\"Command text too long. Please limit it to %d characters\"}"), CMD_MAX_LEN); 
    return false;   
  }

  switch (fn) {
    case FUNC_DISPLAY_CLEAR:
      result = MAXCmndClear();
      break;
    case FUNC_DISPLAY_NUMBER :
      result = MAXCmndNumber(true);
      break;
    case FUNC_DISPLAY_NUMBERNC :
      result = MAXCmndNumber(false);
      break;
    case FUNC_DISPLAY_FLOAT :
      result = MAXCmndFloat(true);
      break;
    case FUNC_DISPLAY_FLOATNC :
      result = MAXCmndFloat(false);
      break;
    case FUNC_DISPLAY_BRIGHTNESS:
      result = MAXCmndBrightness();
      break;
    case FUNC_DISPLAY_RAW:
      result = MAXCmndRaw();
      break;
    case FUNC_DISPLAY_SEVENSEG_TEXT:
      result = MAXCmndText(true);
      break;
    case FUNC_DISPLAY_SEVENSEG_TEXTNC:
      result = MAXCmndText(false);
      break;
    case FUNC_DISPLAY_LEVEL:
      result = MAXCmndLevel();
      break;
    case FUNC_DISPLAY_SCROLLTEXT:
      result = MAXCmndScrollText();
      break;
    case FUNC_DISPLAY_SCROLLDELAY:
      result = MAXCmndScrollDelay();
      break;
   case FUNC_DISPLAY_CLOCK:
      result = MAXCmndClock();
      break;
  }

  return result;
}


/*********************************************************************************************\
 * Interface
\*********************************************************************************************/
bool Xdsp16(uint8_t function)
{
  bool result = false;

  if(function == FUNC_DISPLAY_MODEL) {
    return true;
  }

  if (Settings.display_model == XDSP_16) {
    switch (function) {
      case FUNC_DISPLAY_INIT_DRIVER:
        result = MAXDriverInit();              // init 
        break;
      case FUNC_DISPLAY_INIT:
        AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: %s: FUNC_DISPLAY_INIT: Display depends on MAX7219Data.display_type, currently %d"), MAX7219Data.model_name, Settings.display_options.data);
        result = true;  
        break;
      case FUNC_DISPLAY_TYPE:
        AddLog(LOG_LEVEL_DEBUG, PSTR("MAX: %s: FUNC_DISPLAY_TYPE: MAX7219Data.display_type set to %d, restarting to take effect ..."), MAX7219Data.model_name, Settings.display_options.data);
        TasmotaGlobal.restart_flag = 2;
        break;
      case FUNC_DISPLAY_SEVENSEG_TEXT:
      case FUNC_DISPLAY_CLEAR:
      case FUNC_DISPLAY_NUMBER:
      case FUNC_DISPLAY_FLOAT:
      case FUNC_DISPLAY_NUMBERNC:
      case FUNC_DISPLAY_FLOATNC:
      case FUNC_DISPLAY_RAW:
      case FUNC_DISPLAY_LEVEL:
      case FUNC_DISPLAY_SEVENSEG_TEXTNC:
      case FUNC_DISPLAY_SCROLLTEXT:
      case FUNC_DISPLAY_SCROLLDELAY:
      case FUNC_DISPLAY_CLOCK:
        MAX7219Data.show_clock = false;
      case FUNC_DISPLAY_BRIGHTNESS:
        result = MAXMainFunc(function);
        break;
      case FUNC_DISPLAY_EVERY_50_MSECOND:
        if(MAX7219Data.scroll) maxScrollText();
        if(MAX7219Data.show_clock) maxShowTime();
        break;
    }
  }
  return result;
}

#endif  // USE_DISPLAY_MAX7219
#endif  // USE_DISPLAY
