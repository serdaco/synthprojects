#include <MIDI.h>
// Test program for DBSHIELD1 : arduino shield for DreamBlaster X2,S2,..
// see https://www.serdashop.com/DBSHIELD1
// Demo program usage : 
// connect a midi signal with MIDI Clock to the midi input
// the DBSHIELD1 will play a drum beat in sync with the clock


bool clockison = false;
int clockcount = 0;
int seqcount = 0;
int clockinterval = 6;
int patternlength = 4;
MIDI_CREATE_DEFAULT_INSTANCE();

#define GMBASS  36
#define GMCLOSEHIHAT 42

void handleContinue(void)
{
  clockison = true;  
}

void handleStart(void)
{
  clockison = true;
  clockcount = 0;
}

void handleStop(void)
{
  clockison = false;
  
}

void sequencestep(int seqcount)
{
  switch (seqcount)
  {
    case 0:
      MIDI.sendNoteOn(GMBASS, 127, 10); 
      break;
    case 1:
    case 2:
    case 3:
      MIDI.sendNoteOn(GMCLOSEHIHAT, 127, 10); 
      break;      
  }
};  

void handleClock(void)
{
 if(clockison)
 {
    if(clockcount>=clockinterval)
      {
        clockcount=0;        
        seqcount = seqcount % patternlength;
        sequencestep(seqcount);
        seqcount++;
      }
    clockcount++;
 }
  
}
void HandleNoteOn(byte channel, byte pitch, byte velocity) {
// check your pitch, velocity, and channel here
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void HandleNoteOff(byte channel, byte pitch, byte velocity) {
// check your pitch, velocity, and channel here
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}

void setup() {
  // put your setup code here, to run once:
  MIDI.begin();
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleClock(handleClock);
  MIDI.setHandleStart(handleStart);
  MIDI.setHandleStop(handleStop);
  MIDI.setHandleContinue(handleContinue);
  pinMode(LED_BUILTIN, OUTPUT);
}



void loop() {
  // put your main code here, to run repeatedly:
  MIDI.read();
}
