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
#include <stdio.h>


#include "MidiTrumpetGlobals.h"
#include "DisplayManager.h"
#include "MidiManager.h"

// Buffer for serial output
char pszBuffer[255];

// HW PIN assignments
int Valve1Pin = 5;
int Valve2Pin = 6;
int Valve3Pin = 10;
int Valve4Pin = 11;
int OctPin = 12;
int BCPin = A0;
int SliderPin = A2;
int XPin = A4;
int YPin = A5;

// Static configuration
int iNoteOnThreshold = 320;
int iNoteOffThreshold = 280;
int iBCDelta = 8;

int iHalfTube=1;
int iLowNote=52;
int iHighNote=79;

// Dynamic configuration

int iCoarseTuning = 0; // low c is C4
int iBaseKey = 60; 

// State variables
int iGoalNote = 0;
int iOldGoalNote = 255;
int OldBC = -1;
bool bNoteOn = false;
int iNote = 0;
int iOldNote = 255;
int iCurrentNote = 255;
int iDisplayNote = 0;
int iOldDisplayNote = 255;

unsigned long lastMillis = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  // make the pushbutton's pin an input:
  pinMode(Valve1Pin, INPUT_PULLUP);
  pinMode(Valve2Pin, INPUT_PULLUP);
  pinMode(Valve3Pin, INPUT_PULLUP);
  pinMode(Valve4Pin, INPUT_PULLUP);
  pinMode(OctPin, INPUT_PULLUP);
//  pinMode(S1Pin, INPUT_PULLUP);
//  pinMode(S2Pin, INPUT_PULLUP);

  init_display();

  Serial.println("MidiTrumpet Copyright (C) 2013-2024  John DeCarli");
  Serial.println("This program comes with ABSOLUTELY NO WARRANTY; for details see <https://www.gnu.org/licenses/>");
  Serial.println("This is free software, and you are welcome to redistribute it under certain conditions.");
  Serial.println();
  Serial.print("Midi Trumpet V4.2 SW v0.03.03 - ");
  Serial.print(F(__DATE__));
  Serial.print(" ");
  Serial.println(F(__TIME__));

  calculate_note_lookups();
  
  lastMillis = millis();
}

void CalculateNote(int iSliderValue, int iValveValue)
{
  int iStartIndex = NUM_HARMONICS*iValveValue+iHalfTube;
  int iEndOfIndex = iStartIndex + NUM_HARMONICS -1;
  int iIndex = iStartIndex;
  int iLastIndex = iIndex;
  bool bDone = false;
  while (!bDone)
  {
    if ( iSliderValue <= iLookupValue[iIndex] )
    {
      // Last index was closer, use it
      bDone = true;
      iIndex = iLastIndex;
    }
    else
    {
      iLastIndex = iIndex;
      if ( iIndex == iEndOfIndex )
      {
        // this is the last index, just use it
        bDone = true;
      }
      else
      {
        iIndex++;
      }
    }
  }
  int iNextIndex = iIndex;
  if (iIndex > iStartIndex)
  { 
    iLastIndex = iIndex - 1;
  }
  if (iIndex < iEndOfIndex)
  {
    iNextIndex = iIndex + 1;
  }
  iDisplayNote = iLookupNote[iIndex];
  iNote = iDisplayNote + iCoarseTuning;
  iGoalNote = achGoalNote[iSliderValue];
}

void display_note()
{
  if ( (iDisplayNote != iOldDisplayNote) || (iGoalNote != iOldGoalNote) )
  {
    update_display(iDisplayNote,iGoalNote);
    iOldDisplayNote = iDisplayNote;
    iOldGoalNote = iGoalNote;
    lastMillis = millis();
  }
}

// local data for Breath control buffer
const int BC_BUFFER_SIZE = 8;
const int BC_ON_SAMPLES = 4;
int BCBuffer[BC_BUFFER_SIZE];
int BCNoteOn[BC_BUFFER_SIZE];
unsigned long BCTimes[BC_BUFFER_SIZE];
int bufIndex = 0;
int iOnIndex = -1;
int iOnCount = 0;

void bcbuf_handler(int iValue, bool bNoteOn)
{
  BCBuffer[bufIndex] = iValue;
  BCTimes[bufIndex] = micros();
  if (bNoteOn)
  {
    BCNoteOn[bufIndex] = 1;
  }
  else
  {
    BCNoteOn[bufIndex] = 0;
  }
  if (iOnIndex >= 0)
  {
    iOnCount += 1;
    if (BC_ON_SAMPLES == iOnCount)
    {
      int i = 0;
      int iIndex = bufIndex + 1;
      if (iIndex >= BC_BUFFER_SIZE)
      {
        iIndex = 0;
      }
//      Serial.println("----------------------");
      for (i=0; i < BC_BUFFER_SIZE; i++)
      {
        /*
        Serial.print(iIndex);
        Serial.print(" ");
        Serial.print(BCTimes[iIndex]);
        Serial.print(" ");
        Serial.print(BCNoteOn[iIndex]);
        Serial.print(" ");
        Serial.println(BCBuffer[iIndex]);
        */
        iIndex++;
        if (iIndex >= BC_BUFFER_SIZE)
        {
          iIndex = 0;
        }
      }
    }
    else if (iOnCount > BC_ON_SAMPLES)
    {
      if (!bNoteOn)
      {
        iOnIndex = -1;
        iOnCount = 0;
      }
    }
  }
  else if (bNoteOn)
  {
    iOnIndex = bufIndex;
    iOnCount = 1;
  }
  bufIndex++;
  if (bufIndex >= BC_BUFFER_SIZE)
  {
    bufIndex = 0;
  }
}
// the loop routine runs over and over again forever:
void loop() {
  // handle BC input:
  int BCValue = analogRead(BCPin);
  bcbuf_handler(BCValue,bNoteOn);

  // read other analog inputs
  int iRawSlider = analogRead(SliderPin);
  int iX = analogRead(XPin);
  int iY = analogRead(YPin); 

  int iSliderValue = 1023 - iRawSlider;

  // read valves
  int V1 = 1-digitalRead(Valve1Pin);
  int V2 = 1-digitalRead(Valve2Pin);
  int V3 = 1-digitalRead(Valve3Pin);
  int V4 = 1-digitalRead(Valve4Pin);
  int iValveValue = V1*2+V2+V3*3+V4*5;

  if (bNoteOn)
  {
    CalculateNote(iSliderValue,iValveValue);
    display_note();
    if (BCValue < iNoteOffThreshold )
    {
      MidiNoteOff(iCurrentNote);
      bNoteOn = false;
    }
    else
    {
      if (iNote != iCurrentNote)
      {
        MidiNoteOn(iNote, 64 /* BCCourse */);
        MidiNoteOff(iCurrentNote);
        iCurrentNote = iNote;
      }
      if ( abs(BCValue - OldBC) > iBCDelta)
      {
        int BCData = (BCValue - 255) / 4;
        if (BCData > 127)
        {
          BCData = 127;
        }
        if (BCData < 0)
        {
          BCData = 0;
        }
          
        BreathControl(BCData);
        OldBC = BCValue;
      }
    }
    lastMillis = millis();
  }
  else
  {
    CalculateNote(iSliderValue,iValveValue);
    display_note();
    if (BCValue > iNoteOnThreshold)
    {
      MidiNoteOn(iNote, 64 /* BCCourse */);
      bNoteOn = true;
      iCurrentNote = iNote;
    }
  }
  if ( ( millis() - lastMillis ) > 5000 )
  {
    clear_display();
  }
}
