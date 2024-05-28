#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MidiTrumpetGlobals.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_DC     1
#define OLED_CS     0
#define OLED_RESET  13
//Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

void init_display()
{
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done, flip display
  //display.setRotation(2);

  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
}

void display_note(int iNote, int iSlider, int iValveIndex, int iValveMap, byte & iOldDisplayNote)
{
  if (iNote != iOldDisplayNote)
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0, 0);
    display.print(achNoteLookup[iBaseKey%12]);
    display.print(achOctaveLookup[iBaseKey/12]);
    display.setTextSize(4);
    display.setCursor(32, 32);
    display.print(achNoteLookup[iNote%12]);
    display.print(achOctaveLookup[iNote/12]);
    display.display();
    display.setCursor(0, 0);
    iOldDisplayNote = iNote;
  }
  
//  sprintf(pszBuffer, "V: %i%i%i%i S: %04i Note: %03i(%s%s) Last: %04i Current: %04i Next: %04i",
//      V1,V2,V3,V4, iSlider,iNote, achNoteLookup[iNote%12],achOctaveLookup[iNote/12]
//      ,
//      iLookupValue[iLastIndex],
//      iLookupValue[iIndex],
//      iLookupValue[iNextIndex]
//     );
//  Serial.println(pszBuffer);
  
}

void clear_display()
{
  display.clearDisplay();
  display.display();  
}

