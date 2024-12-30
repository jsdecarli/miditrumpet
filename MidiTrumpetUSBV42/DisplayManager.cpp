/*

    MidiTrumpet, a Valved Wind Instrument MIDI controller 
    Copyright (C) 2013-2024 John DeCarli

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    https://github.com/jsdecarli/miditrumpet
 */

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

void update_display(int iDisplayNote, int iGoalNote)
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 0);
  display.print(achNoteLookup[iBaseKey%12]);
  display.print(achOctaveLookup[iBaseKey/12]);
  display.setCursor(64,0);
  display.print(achNoteLookup[iGoalNote%12]);
  display.print(achOctaveLookup[iGoalNote/12]);
  display.setTextSize(4);
  display.setCursor(48, 32);
  display.print(achNoteLookup[iDisplayNote%12]);
  display.print(achOctaveLookup[iDisplayNote/12]);
  display.display();
  display.setCursor(0, 0);
}

void clear_display()
{
  display.clearDisplay();
  display.display();  
}
