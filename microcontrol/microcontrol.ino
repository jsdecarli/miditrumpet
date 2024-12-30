/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDIUSB.h>

#define SERIAL_DEBUG 1

//#define BOOT_DOUBLE_TAP_ADDRESS     (HSRAM_ADDR + HSRAM_SIZE - 4)
#define BOOT_DOUBLE_TAP_ADDRESS           (0x20007FFCul)
#define BOOT_DOUBLE_TAP_DATA              (*((volatile uint32_t *) BOOT_DOUBLE_TAP_ADDRESS))
#define DOUBLE_TAP_MAGIC 0xf01669ef 


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int iChannel = 3;

typedef struct
{
  int analogPin;
  int iMidSensorValue;
  int iValue;
  int iUpperCCValue;
  int iLowerCCValue;
  int iUpperChannel;
  int iLowerChannel;
  int iUpperCCNum;
  int iLowerCCNum;
} sensor_data;

#define NUM_SENSORS 2
const int analogInPin[NUM_SENSORS] = {A8,A7};  // Analog input pin that the potentiometer is attached to
const int iMidSensorValue[NUM_SENSORS] = {495,446};
const int iSensorUpperCC[NUM_SENSORS] = {91,93};
const int iSensorLowerCC[NUM_SENSORS] = {92,94};

sensor_data sensors[NUM_SENSORS];

void init_sensor(int iIndex)
{
  sensor_data* pSensor = &sensors[iIndex];
  pSensor->analogPin = analogInPin[iIndex];
  pSensor->iMidSensorValue = iMidSensorValue[iIndex];
  pSensor->iValue = 0;
  pSensor->iUpperChannel = iChannel;
  pSensor->iLowerChannel = iChannel;
  pSensor->iUpperCCValue = 0;
  pSensor->iLowerCCValue = 0;
  pSensor->iUpperCCNum = iSensorUpperCC[iIndex];
  pSensor->iLowerCCNum = iSensorLowerCC[iIndex];
}

typedef struct
{
  int swPin;
  int iValue;
  int iOldSwValue;
  int iDebounce;
  int iCCValue;
  int iChannel;
  int iCCNum;
} switch_data;

#define NUM_SWITCHES 4

const int sw[NUM_SWITCHES] = {D3,D6,D10,D9};
const int iSWCC[NUM_SWITCHES] = {80,81,82,83};

switch_data switches[NUM_SWITCHES];

void init_switch(int iIndex)
{
  switch_data * pSwitch = &switches[iIndex];
  pSwitch->swPin = sw[iIndex];
  pinMode(pSwitch->swPin, INPUT_PULLUP);
  pSwitch->iValue = 1;
  pSwitch->iOldSwValue = 1;
  pSwitch->iDebounce = 0;
  pSwitch->iCCValue = 0;
  pSwitch->iChannel = iChannel;
  pSwitch->iCCNum = iSWCC[iIndex]; 
}
#define SLIDER_HEIGHT 16
#define BUTTON_DIMENSION 32

void serial_version()
{
  Serial.print("Microcontrol: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
}
void setup() {
//  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(100); // Pause for 100 ms

  // Clear the buffer
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setTextSize(1);             // Draw 2X-scale text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Microcontrol"));
  display.print(F(__DATE__));
  display.print(F(" "));
  display.println(F(__TIME__));

  display.display();

  delay(1000);
  display.clearDisplay();
  display.display();
#if SERIAL_DEBUG
  serial_version();
#endif
  int i = 0;
  for (i=0; i < NUM_SENSORS; i++)
  {
    init_sensor(i);
    display.drawRect(0, i*SLIDER_HEIGHT, SCREEN_WIDTH, SLIDER_HEIGHT, SSD1306_WHITE);
  }
  for (i=0; i < NUM_SWITCHES; i++)
  {
    init_switch(i);
    display.drawRect(i*BUTTON_DIMENSION, BUTTON_DIMENSION, BUTTON_DIMENSION, BUTTON_DIMENSION, SSD1306_WHITE);
  }
  display.display();
}

void controlChange(byte channel, byte control, byte value) 
{
#pragma GCC diagnostic ignored "-Wnarrowing"
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
#pragma GCC diagnostic warning "-Wnarrowing"
  MidiUSB.sendMIDI(event);
}

#define MAX_CC_VALUE 127
#define CC_ON_VALUE 64
#define CC_OFF_VALUE 0
#define MIDDLE_BUFFER 50

bool handle_sensor(int iIndex)
{
  sensor_data * pSensor = &sensors[iIndex];
  pSensor->iValue = analogRead(pSensor->analogPin);
  int iLowerBuffer = pSensor->iMidSensorValue - MIDDLE_BUFFER;
  int iUpperBuffer = pSensor->iMidSensorValue + MIDDLE_BUFFER;

  int iNewLower = 0;
  int iNewUpper = 0;
  bool bReturn = false;
  if ( pSensor->iValue <= iUpperBuffer )
  {
    // Upper value is 0
    // Calculate new lower value
    if (pSensor->iValue <= iLowerBuffer)
    {
      iNewLower = (iLowerBuffer - pSensor->iValue) / 2;
      if (iNewLower > MAX_CC_VALUE)
      {
        iNewLower = MAX_CC_VALUE;
      }
    }
  }
  else
  {
    // Lower value is 0
    // Calculate new upper value
    iNewUpper = (pSensor->iValue - iUpperBuffer) / 2;
    if (iNewUpper > MAX_CC_VALUE)
    {
      iNewUpper = MAX_CC_VALUE;
    }
  }
  if (iNewUpper != pSensor->iUpperCCValue)
  {
    pSensor->iUpperCCValue = iNewUpper;
    controlChange(
      pSensor->iUpperChannel, 
      pSensor->iUpperCCNum, 
      iNewUpper);
    bReturn = true;
  }
  if (iNewLower != pSensor->iLowerCCValue)
  {
    pSensor->iLowerCCValue = iNewLower;
    controlChange(
      pSensor->iLowerChannel, 
      pSensor->iLowerCCNum, 
      iNewLower);
    bReturn = true;
  }
  return bReturn;
}

int iControllerState = 0;

#define BOUNCE_COUNT 5

bool handle_switch(int iIndex)
{
  switch_data* pSwitch = &switches[iIndex];
  pSwitch->iValue = digitalRead(pSwitch->swPin);
  bool bReturn = false;
  int iNewValue = pSwitch->iCCValue;
  if (pSwitch->iValue != pSwitch->iOldSwValue)
  {
    if (0 == pSwitch->iOldSwValue)
    {
      // debouce switch release
      pSwitch->iDebounce++;
      if (pSwitch->iDebounce >= BOUNCE_COUNT)
      {
        pSwitch->iDebounce = 0;
        pSwitch->iOldSwValue = 1;
        // toggle CC value
        if (pSwitch->iCCValue < CC_ON_VALUE)
        {
          // Currently off, turn on
          iNewValue = CC_ON_VALUE;
        }
        else
        {
          // Currently on, turn off
          iNewValue = CC_OFF_VALUE;
        }
      }
    }
    else
    {
      pSwitch->iOldSwValue = 0;
      pSwitch->iDebounce = 0;
    }
  }
  if (iNewValue != pSwitch->iCCValue)
  {
    pSwitch->iCCValue = iNewValue;
    controlChange(
      pSwitch->iChannel, 
      pSwitch->iCCNum, 
      iNewValue);
    bReturn = true;
  }
  return bReturn;
}

#define MIDI_BUFFER_SIZE 128
int iMidiState = 0;
uint8_t sysex_buf[MIDI_BUFFER_SIZE];
int iSysex = 0;

midiEventPacket_t rx_buf[MIDI_BUFFER_SIZE];
int iBufIndex = 0;

void midi_print()
{
  int i = 0;
  if (0 == iBufIndex)
  {
    return;
  }
  for (i = 0; i < iBufIndex; i++)
  {
    midiEventPacket_t msg = rx_buf[i];
    if (0 != msg.header)
    {
      int midiCIN = msg.header & 0xf;
      switch(midiCIN)
      {
        case 0x5:
        case 0xf:
          Serial.print(msg.header, HEX);
          Serial.print(" ");
          Serial.println(msg.byte1, HEX);
          break;
        case 0x2:
        case 0x6:
        case 0xc:
        case 0xd:
          Serial.print(msg.header, HEX);
          Serial.print(" ");
          Serial.print(msg.byte1, HEX);
          Serial.print(" ");
          Serial.println(msg.byte2, HEX);
          break;
        default:
          Serial.print(msg.header, HEX);
          Serial.print(" ");
          Serial.print(msg.byte1, HEX);
          Serial.print(" ");
          Serial.print(msg.byte2, HEX);
          Serial.print(" ");
          Serial.println(msg.byte3, HEX);
          break;
      }
    }
  }
  Serial.println();
}

bool MidiReceive()
{
  bool bReturn = false;
  bool bDone = false;
  midiEventPacket_t rx = MidiUSB.read();
  int header = rx.header;
  while (!bDone)
  {
    while (0 != header)
    {
      bReturn = true;    
      rx_buf[iBufIndex++] = rx;
      if (iBufIndex < MIDI_BUFFER_SIZE)
      {
        rx = MidiUSB.read();
        header = rx.header;
      }
      else
      { 
        header = 0;
      }
    }
    int i = 0;
    for (i=0; i < iBufIndex; i++)
    {
      rx = rx_buf[i];
      int midiCIN = rx.header & 0xf;
      bResetIndex = false;
      int iNumBytes = 3;
      switch (midiCIN)
      {
        case 0x5:
        case 0xf:
          iNumBytes = 1;
          break;
        case 0x2:
        case 0x6:
        case 0xc:
        case 0xd:
          iNumBytes = 2;
          break;
        case 0x0:
        case 0x1:
        case 0x
        default:
          break;
      }
      switch(iMidiState)
      {
        case 0:
          if 
      }
    }
  #if SERIAL_DEBUG
    midi_print();
  #endif
    iBufIndex = 0;
    rx = MidiUSB.read();
    header = rx.header;
    if (0 == header)
    {
      bDone = true;
    }
  }
  return bReturn;
}

int iRxState = 0;

void loop() {

  // print the results to the Serial Monitor:
  bool bUpdate = false;
  int i = 0;
  for (i=0; i< NUM_SENSORS; i++)
  {
    bool update_sensor = handle_sensor(i);
    if (update_sensor)
      bUpdate = true;
      display.fillRect(
        1, i*SLIDER_HEIGHT+1, 
        SCREEN_WIDTH-2, SLIDER_HEIGHT-2, SSD1306_BLACK);
      if (sensors[i].iLowerCCValue > 0)
      {
        display.fillTriangle(
          min(SCREEN_WIDTH-sensors[i].iLowerCCValue,SCREEN_WIDTH-2),
          i*SLIDER_HEIGHT+1,
          min(SCREEN_WIDTH-sensors[i].iLowerCCValue,SCREEN_WIDTH-2), 
          (i+1)*SLIDER_HEIGHT-2,
          SCREEN_WIDTH - 2,
          (i+1)*SLIDER_HEIGHT-2, 
          SSD1306_WHITE);
      }
      else if (sensors[i].iUpperCCValue > 0)
      {
        display.fillTriangle(
          1, (i+1)*SLIDER_HEIGHT-2,
          max(sensors[i].iUpperCCValue-1,1), 
          i*SLIDER_HEIGHT+1, 
          max(sensors[i].iUpperCCValue-1,1), 
          (i+1)*SLIDER_HEIGHT-2, 
          SSD1306_WHITE);
      }
  }
  for (i=0; i< NUM_SWITCHES; i++)
  {
    bool bUpdateSwitch = handle_switch(i);
    if (bUpdateSwitch)
    {
      bUpdate = true;
      int iFillColor = SSD1306_WHITE;
      if (switches[i].iCCValue < CC_ON_VALUE)
      {
        iFillColor = SSD1306_BLACK;
      }
      display.fillRect(
        i*BUTTON_DIMENSION+1, 
        BUTTON_DIMENSION+1, 
        BUTTON_DIMENSION-2, 
        BUTTON_DIMENSION-2, 
        iFillColor);
    }
  }
  if (bUpdate)
  {
    display.display();
#if SERIAL_DEBUG
    Serial.print("Controllers 1: ");
    Serial.print(sensors[0].iValue);
    Serial.print("[");
    Serial.print(sensors[0].iValue - sensors[0].iMidSensorValue);
    Serial.print("] -> ");
    Serial.print(sensors[0].iUpperCCValue);
    Serial.print("/");
    Serial.print(sensors[0].iLowerCCValue);
    Serial.print("; 2: ");
    Serial.print(sensors[1].iValue);
    Serial.print("[");
    Serial.print(sensors[1].iValue - sensors[1].iMidSensorValue);
    Serial.print("] -> ");
    Serial.print(sensors[1].iUpperCCValue);
    Serial.print("/");
    Serial.print(sensors[1].iLowerCCValue);
    Serial.print("; 2: ");
    Serial.print("\t switches = ");
    Serial.print(switches[0].iCCValue);
    Serial.print(" ");
    Serial.print(switches[1].iCCValue);
    Serial.print(" ");
    Serial.print(switches[2].iCCValue);
    Serial.print(" ");
    Serial.println(switches[3].iCCValue);
#endif
  }
  if (Serial.available() > 0)
  {
    u_char rxChar = Serial.read();
    switch (iRxState)
    {
    case 0:
      if ('B' == rxChar) 
      {
        iRxState = 1;
      }
      else 
      {
        serial_version();
      }
      break;
    case 1:
      if ('O' == rxChar)
      {
        iRxState = 2;
      }
      else
      {
        iRxState = 0;
      }
      break;
    case 2:
      if ('O' == rxChar)
      {
        iRxState = 3;
      }
      else
      {
        iRxState = 0;
      }
      break;
    case 3:
      if ('T' == rxChar)
      {
        Serial.println("Enter Bootloader");
        Serial.flush();
        delay(100);
        BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;    // Start in bootloader mode after reset
        NVIC_SystemReset();                         // Reset the microcontroller
      }
      else
      {
        iRxState = 0;
      }
      break;
    default:
      iRxState  = 0;
      break;
    }
  }
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  if (!MidiReceive())
  {
    delay(2);
  }
}

