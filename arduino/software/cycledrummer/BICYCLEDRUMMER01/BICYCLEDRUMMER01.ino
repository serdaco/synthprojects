/*
BICYCLE DRUMMER 0.02
30.05.2014
By Serge Defever
Arduino NANO + Dreamblaster S1

Dreamblaster S1 : http://www.serdashop.com/waveblaster
Arduino NANO : http://store.arduino.cc/

*/

#include <TimerOne.h>
long calctempodelay = 0;
long tempodelay = 500;
long curtimestamp = 0;
long prevtimestamp = 0;
long lasttickstamp =0;
long wheelfactor = 6; // adjust this to modify the pulseperiod to tempo ratio (wheel radius), higher = faster

#define _BASSDRUM_NOTE 0x24
#define _SNAREDRUM_NOTE 0x26
#define _CLOSEDHIHAT_NOTE 0x2A
#define _PEDALHIHAT_NOTE 0x2C
#define _OPENHIHAT_NOTE 0x2E
#define _CYMBAL_NOTE 0x31

void midiwrite(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}


const int buttonPin = 2;
int potentiopin = 2;
long potentiovalue = 0;
int buttonState = 0; 
int prev_buttonState = 0; 
int enabledrumming = 0;
long seq_poscnt = 0;

void drumseq(long seqpos) 
{ 
  char relativpos;
  char seq_poscnt_mod2;
  relativpos =seqpos%16;
  seq_poscnt_mod2 = seqpos%2;
  if(enabledrumming)
  {
    if((seq_poscnt_mod2 == 0) && (relativpos < 14))
    {
      midiwrite(0x99, _PEDALHIHAT_NOTE, 0x65);     // fast hi hat midi note
    }

    switch(relativpos)
    {
    case 0 :
      midiwrite(0x99, _BASSDRUM_NOTE, 0x65);     
      break;
    case 4:
      midiwrite(0x99, _SNAREDRUM_NOTE, 0x65);    
      break;
    case 6 :
      midiwrite(0x99, _BASSDRUM_NOTE, 0x65);     
      break;
    case 8 :
      midiwrite(0x99, _BASSDRUM_NOTE, 0x65);     
      break;
    case 12:
      midiwrite(0x99, _SNAREDRUM_NOTE, 0x65);    
      break;
    case 14:
      midiwrite(0x99,  _OPENHIHAT_NOTE, 0x65);   

      break;        
    default :
      break;
    }    

  };
}


void timercallback()
{
  drumseq(seq_poscnt);
  bassseq(seq_poscnt);
  seq_poscnt++;
  lasttickstamp = millis();
  Timer1.setPeriod(tempodelay * 1000);     
}

void setup() {
  Serial.begin(31250);
  pinMode(buttonPin, INPUT);  
  Timer1.attachInterrupt(timercallback);
  Timer1.initialize(tempodelay*1000);   
}

void stopdrum(){
  enabledrumming = 0; 
  prevtimestamp = 0;
  curtimestamp = 0;
  seq_poscnt = 0;
  
}

void loop() {
  long timesincelastbeat;
  long timetonextbeat;
  buttonState = digitalRead(buttonPin);
  if(prev_buttonState!=buttonState)
  {
    prev_buttonState = buttonState;
    if(buttonState == 1)
    {
      prevtimestamp =  curtimestamp;
      curtimestamp = millis();
      if(prevtimestamp)
      {           
        calctempodelay = curtimestamp - prevtimestamp;
        if(calctempodelay > 200  && calctempodelay < 5000)  // if tempo is within range, calc tempo and enable drum
        {
          if(enabledrumming == 0)
          {
            seq_poscnt = 0;
            enabledrumming = 1;
          }
          tempodelay  = calctempodelay/wheelfactor;
          timesincelastbeat = curtimestamp - lasttickstamp;
          if(timesincelastbeat > tempodelay)  // is the beat is going faster, tick immediately (within 10ms)
          {
            timetonextbeat = 10; 
          }
          else
          {
            timetonextbeat = tempodelay -timesincelastbeat; // calculate remaining period for next tick
            if (timetonextbeat < 10)
            {
              timetonextbeat = 10;
            }
          }
          Timer1.setPeriod(timetonextbeat * 1000);  // calculate period until next beat            
        }
      }
    }
  } 
  else
  {
    if(enabledrumming)
    {
      if(curtimestamp)
      {
        if( (millis() - curtimestamp  )> 5000)  // automatically stop if no sensor change for longer than 5s
        {
          stopdrum(); 
        }
      }
    }
  }

}








