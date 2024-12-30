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
#include "MidiTrumpetGlobals.h"

int iBreak[NUM_SEGMENTS] = {67,72,1024};
double fFactor[NUM_SEGMENTS] = {3.0,1.5,1.0};

const byte iLookupNote[NUM_VALVE_OFFSETS*NUM_HARMONICS]= {
  48,	60,	67,	72,	76,	79,	84,	88,	91,	96,	100,103,	108,	112,
  47,	59,	66,	71,	75,	78,	83,	87,	90,	95,	99,	102,	107,	111,
  46,	58,	65,	70,	74,	77,	82,	86,	89,	94,	98,	101,	106,	110,
  45,	57,	64,	69,	73,	76,	81,	85,	88,	93,	97,	100,	105,	109,
  44,	56,	63,	68,	72,	75,	80,	84,	87,	92,	96,	99,	104,	108,
  43,	55,	62,	67,	71,	74,	79,	83,	86,	91,	95,	98,	103,	107,
  42,	54,	61,	66,	70,	73,	78,	82,	85,	90,	94,	97,	102,	106,
  41,	53,	60,	65,	69,	72,	77,	81,	84,	89,	93,	96,	101,	105,
  40,	52,	59,	64,	68,	71,	76,	80,	83,	88,	92,	95,	100,	104,
  39,	51,	58,	63,	67,	70,	75,	79,	82,	87,	91,	94,	99,	103,
  38,	50,	57,	62,	66,	69,	74,	78,	81,	86,	90,	93,	98,	102,
  37,	49,	56,	61,	65,	68,	73,	77,	80,	85,	89,	92,	97,	101 };

int iLookupValue[NUM_VALVE_OFFSETS*NUM_HARMONICS];

const char * achNoteLookup[] = {
  "C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B" };

const char * achOctaveLookup[] = {
  "-1","0","1","2","3","4","5","6","7","8","9" };
  
byte achGoalNote[1024];

void calculate_note_lookups() {
  Serial.println("Start Array Generation");

  int iPitch;
  double fGoalPitch = iLowNote;
  int iSegmentBase = 0;
  int iValveOffset = 0;
  int iHarmonic = 0;
  double fDelta = 0.0;
  double fBaseDelta = (iHighNote-iLowNote)/1024.0;
  int iNoteIndexList[NUM_VALVE_OFFSETS];

  // Initialize arrays
  for (iValveOffset = 0; iValveOffset < NUM_VALVE_OFFSETS; iValveOffset++)
  {
    iNoteIndexList[iValveOffset] = iHalfTube;
    for (iHarmonic = 0; iHarmonic < NUM_HARMONICS; iHarmonic++)
    {
      iLookupValue[iValveOffset*iHarmonic] = 0;
    }
  }
  for (iPitch = 0; iPitch < 1024; iPitch++)
  {
    achGoalNote[iPitch] = byte(fGoalPitch+0.5);
    Serial.print("Index ");
    Serial.print(iPitch);
    Serial.print("  ");
    Serial.println(achGoalNote[iPitch]);
    for (iValveOffset = 0; iValveOffset < NUM_VALVE_OFFSETS; iValveOffset++)
    {
      int iHarmonic = iNoteIndexList[iValveOffset];
      bool bDone = false;
      int iBaseIndex = NUM_HARMONICS*iValveOffset+iHarmonic;
      while (!bDone)
      {
        double fCheckValue = (iLookupNote[iBaseIndex] + iLookupNote[iBaseIndex+1])/2.0;
        if (fGoalPitch <= fCheckValue)
        {
          iLookupValue[iBaseIndex] = iPitch;
          iHarmonic++;
          iBaseIndex++;
          if (iHarmonic >= (NUM_HARMONICS - 1))
          {
            bDone = true;
            iLookupValue[iBaseIndex] = 1023;
          }
        }
        else
        {
          iNoteIndexList[iValveOffset] = iHarmonic+1;
          bDone = true;
        }
      }
    }
    int iSegmentIndex = iSegmentBase;
    bool bDone = false;
    while (!bDone)
    {
      if (fGoalPitch <= iBreak[iSegmentIndex])
      {
        iSegmentBase = iSegmentIndex;
        fDelta = fFactor[iSegmentIndex]*fBaseDelta;
        bDone = true;
      }
      else
      {
        iSegmentIndex++;
        if (iSegmentIndex >= NUM_SEGMENTS)
        {
          bDone = true;
          fDelta = fBaseDelta;
        }
      }
    }
    fGoalPitch += fDelta;
  }

  Serial.println("Array generation complete:");
  for (iValveOffset = 0; iValveOffset < NUM_VALVE_OFFSETS; iValveOffset++)
  {
    for (iHarmonic = 0; iHarmonic < NUM_HARMONICS; iHarmonic++)
    {
      Serial.print(iLookupValue[iHarmonic+iValveOffset*NUM_HARMONICS]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  return;
}
