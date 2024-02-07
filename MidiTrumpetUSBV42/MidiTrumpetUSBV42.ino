/*
 */
#pragma GCC diagnostic ignored "-Wnarrowing"
#include <MIDIUSB.h>
#pragma GCC diagnostic warning "-Wnarrowing"
#include <stdio.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

char pszBuffer[255];

int Valve1Pin = 5;
int Valve2Pin = 6;
int Valve3Pin = 10;
int Valve4Pin = 11;
int OctPin = 12;

//int S1Pin = 11;
//int S2Pin = 10;

int iNoteOnThreshold = 320;
int iNoteOffThreshold = 280;
int iBCDelta = 8;

#define OLED_DC     1
#define OLED_CS     0
#define OLED_RESET  13
//Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);


const int iLookupValue[168]= {
  82,	202,	348,	463,	553,	655,	770,	860,	962,	1023,	1023,	1023,	1023,	1023,
  75,	189,	323,	438,	527,	630,	745,	834,	937,	1023,	1023,	1023,	1023,	1023,
  69,	177,	297,	412,	502,	604,	719,	809,	911,	1023,	1023,	1023,	1023,	1023,
  62,	164,	272,	387,	476,	579,	694,	783,	885,	1001,	1023,	1023,	1023,	1023,
  56,	151,	246,	361,	451,	553,	668,	758,	860,	975,	1023,	1023,	1023,	1023,
  50,	138,	221,	336,	425,	527,	642,	732,	834,	949,	1023,	1023,	1023,	1023,
  43,	125,	202,	310,	400,	502,	617,	706,	809,	924,	1013,	1023,	1023,	1023,
  37,	113,	189,	284,	374,	476,	591,	681,	783,	898,	988,	1023,	1023,	1023,
  30,	100,	177,	259,	348,	451,	566,	655,	758,	873,	962,	1023,	1023,	1023,
  24,	87,	164,	233,	323,	425,	540,	630,	732,	847,	937,	1023,	1023,	1023,
  18,	78,	151,	209,	297,	400,	515,	604,	706,	822,	911,	1013,	1023,	1023,
  11,	72,	138,	196,	272,	374,	489,	579,	681,	796,	885,	988,	1023,	1023};

const byte iLookupNote[168]= {
  48,	60,	67,	72,	76,	79,	84,	88,	91,	96,	100,	103,	108,	112,
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

const char * achNoteLookup[] = {
  "C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B" };

const char * achOctaveLookup[] = {
  "-1","0","1","2","3","4","5","6","7","8","9" };

int iCoarseTuning = 0; // low c is C4
byte iBaseKey = 60;
byte iOldDisplayNote = 255;
int OldBC = -1;
bool bNoteOn = false;
byte iCurrentNote = 0;

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

  Serial.println("Midi Trumpet V3.2 SW v0.03.00");
  lastMillis = millis();
}
int V1 = 0;
int V2 = 0;
int V3 = 0;
int V4 = 0;

void display_note(int iNote, int iSlider, int iValveIndex, int iValveMap)
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
  
  sprintf(pszBuffer, "V: %i%i%i%i S: %04i Note: %03i(%s%s) Last: %04i Current: %04i Next: %04i",
      V1,V2,V3,V4, iSlider,iNote, achNoteLookup[iNote%12],achOctaveLookup[iNote/12]
//      ,
//      iLookupValue[iLastIndex],
//      iLookupValue[iIndex],
//      iLookupValue[iNextIndex]
     );
  Serial.println(pszBuffer);
  
}

void noteOn(byte channel, byte pitch, byte velocity) 
{
#pragma GCC diagnostic ignored "-Wnarrowing"
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
#pragma GCC diagnostic warning "-Wnarrowing"
  MidiUSB.sendMIDI(noteOn);
}

void MidiNoteOn( int iNote, int iVelocity)
{
  noteOn( 0, iNote, iVelocity);
}

void noteOff(byte channel, byte pitch, byte velocity) 
{
#pragma GCC diagnostic ignored "-Wnarrowing"
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
#pragma GCC diagnostic warning "-Wnarrowing"
  MidiUSB.sendMIDI(noteOff);
}

void MidiNoteOff(int iNote)
{
  noteOff( 0, iNote, 64);  
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void aftertouch(byte channel, byte value) 
{
#pragma GCC diagnostic ignored "-Wnarrowing"
  midiEventPacket_t event = {0x0D, 0xD0 | channel, value, 0};
#pragma GCC diagnostic warning "-Wnarrowing"
  MidiUSB.sendMIDI(event);
}

void controlChange(byte channel, byte control, byte value) 
{
#pragma GCC diagnostic ignored "-Wnarrowing"
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
#pragma GCC diagnostic warning "-Wnarrowing"
  MidiUSB.sendMIDI(event);
}

void MidiCC(int iController, int iValue)
{
  controlChange( 0, iController, iValue);
}

void BreathControl(int iValue)
{
  controlChange( 0, 2, iValue);
  aftertouch(0,iValue);
}
int iValveIndex = 14;
//int iValveIndex = 10;



void ReadValves(int & iValveValue, int & iValveMap)
{
  V1 = 1-digitalRead(Valve1Pin);
  V2 = 1-digitalRead(Valve2Pin);
  V3 = 1-digitalRead(Valve3Pin);
  V4 = 1-digitalRead(Valve4Pin);
  iValveValue = V1*2+V2+V3*3+V4*5;
  iValveMap = V4<<3 | V3<<2 | V2<<1 | V1;
}

int CalculateNote(int SliderValue, int iValveValue)
{
//  int iStartIndex = iValveIndex*iValveValue+1;
  int iStartIndex = iValveIndex*iValveValue;
//  int iIndex = Valve1  + Valve2  + Valve3;
  int iEndOfIndex = iStartIndex + iValveIndex -1;
  //int iLastDelta = 9999;
  int iIndex = iStartIndex;
  int iLastIndex = iIndex;
  bool bDone = false;
  while (!bDone)
  {
  //  int iDelta = abs( SliderValue - iBitOffset - iLookupValue[iIndex] );
    if ( SliderValue <= iLookupValue[iIndex] )
    {
      // Last index was closer, use it
      bDone = true;
      iIndex = iLastIndex;
    }
    else
    {
//      iLastDelta = iDelta;
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
  int iNote = iLookupNote[iIndex];
  return( iLookupNote[iIndex] + iCoarseTuning);
}
// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int BCValue = analogRead(A0);
  
//  int iBCFiltValue = analogRead(A1);
  int iRawSlider = analogRead(A2);
  int iX2 = analogRead(A4);
  int iY2 = analogRead(A5); 

  int SliderValue = 1023 - iRawSlider;
  int iValveValue = 0;
  int iValveMap = 0;

  ReadValves(iValveValue,iValveMap);
  if (bNoteOn)
  {
    int iNewNote = CalculateNote(SliderValue,iValveValue);
    display_note(iCurrentNote,SliderValue,iValveValue,iValveMap);
    if (BCValue < iNoteOffThreshold )
    {
      MidiNoteOff(iCurrentNote);
      bNoteOn = false;
    }
    else
    {
      if (iNewNote != iCurrentNote)
      {
        MidiNoteOn(iNewNote, 64 /* BCCourse */);
        MidiNoteOff(iCurrentNote);
        iCurrentNote = iNewNote;
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
    byte iPreviousNote = iCurrentNote;
    iCurrentNote = CalculateNote(SliderValue,iValveValue);
    display_note(iCurrentNote,SliderValue,iValveValue,iValveMap);
    if (BCValue > iNoteOnThreshold)
    {
      MidiNoteOn(iCurrentNote, 64 /* BCCourse */);
      bNoteOn = true;
    }
    if (iCurrentNote != iPreviousNote)
    {
      lastMillis = millis();
    }
  }
  if ( ( millis() - lastMillis ) > 5000 )
  {
    display.clearDisplay();
    display.display();
  }
}
