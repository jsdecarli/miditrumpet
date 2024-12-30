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

#ifndef MIDITRUMPET_H
#define MIDITRUMPET_H

extern char pszBuffer[255];
extern byte achGoalNote[1024];

extern int Valve1Pin;
extern int Valve2Pin;
extern int Valve3Pin;
extern int Valve4Pin;
extern int OctPin;

extern int iNoteOnThreshold;
extern int iNoteOffThreshold;
extern int iBCDelta;

extern int iHalfTube;
extern int iLowNote;
extern int iHighNote;

#define NUM_SEGMENTS 3
extern int iBreak[NUM_SEGMENTS];
extern double fFactor[NUM_SEGMENTS];
#define NUM_VALVE_OFFSETS 12
#define NUM_HARMONICS 14

extern int iBaseKey;

extern const byte iLookupNote[NUM_VALVE_OFFSETS*NUM_HARMONICS];

extern int iLookupValue[NUM_VALVE_OFFSETS*NUM_HARMONICS];

extern const char * achNoteLookup[];

extern const char * achOctaveLookup[];

extern void calculate_note_lookups();

#endif
