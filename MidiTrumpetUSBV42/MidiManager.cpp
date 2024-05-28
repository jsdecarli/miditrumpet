#pragma GCC diagnostic ignored "-Wnarrowing"
#include <MIDIUSB.h>
#pragma GCC diagnostic warning "-Wnarrowing"

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