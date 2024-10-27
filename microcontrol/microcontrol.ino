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


const int analogInPin1 = A7;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A8;  // Analog input pin that the potentiometer is attached to
const int sw1 = D3;
const int sw2 = D6;
const int sw3 = D10;
const int sw4 = D9;

int stickX = 0;  // value read from the pot
int stickY = 0;  // value read from the pot



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

  Serial.print("Microcontrol: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
  pinMode(sw1, INPUT_PULLUP);    // sets the digital pin 7 as input
  pinMode(sw2, INPUT_PULLUP);    // sets the digital pin 7 as input
  pinMode(sw3, INPUT_PULLUP);    // sets the digital pin 7 as input
  pinMode(sw4, INPUT_PULLUP);    // sets the digital pin 7 as input

}

void loop() {
  int sensorValue1 = analogRead(analogInPin1);
  int sensorValue2 = analogRead(analogInPin2);
  int swValue1 = digitalRead(sw1);
  int swValue2 = digitalRead(sw2);
  int swValue3 = digitalRead(sw3);
  int swValue4 = digitalRead(sw4);

    // print the results to the Serial Monitor:
  Serial.print("sensor1 = ");
  Serial.print(sensorValue1);
  Serial.print("\t sensor2 = ");
  Serial.print(sensorValue2);
  Serial.print("\t switches = ");
  Serial.print(swValue1);
  Serial.print(" ");
  Serial.print(swValue2);
  Serial.print(" ");
  Serial.print(swValue3);
  Serial.print(" ");
  Serial.println(swValue4);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(200);

}

