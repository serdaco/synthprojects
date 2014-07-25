
/*
S1 SEQ // BICYCLE DRUMMER 0.4
25.07.2014
By Serge Defever
Arduino NANO + Dreamblaster S1

Dreamblaster S1 : http://www.serdashop.com/waveblaster
Arduino NANO : http://store.arduino.cc/

*/
#include <Button.h>
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

void midiprogchange(int cmd, int prog) {
  Serial.write(cmd);
  Serial.write(prog);
}

const int bicyclesetup = 0;
const int button1Pin = 2;
const int button2Pin = 3;
const int button3Pin = 4;
const int button4Pin = 5;
const int dreamblaster_enable_pin = 6;
int potentiopin = 2;
long potentiovalue = 0;
int buttonState = 0; 
int prev_buttonState = 0; 
int enabledrumming = 0;
int enablebassline = 0;
int enablesynthline = 0;
long seq_poscnt = 0;


Button button2 = Button(button2Pin,BUTTON_PULLDOWN);
Button button3 = Button(button3Pin,BUTTON_PULLDOWN);
Button button4 = Button(button4Pin,BUTTON_PULLDOWN);


void basssubseq(long relativpos) 
{ 
  switch(relativpos)
    {
    case 0 :
      midiwrite(0x81, 40, 0x00); 
      midiwrite(0x91, 28, 0x70); 
      break;
    case 4 :
      midiwrite(0x81, 28, 0x00); 
      midiwrite(0x91, 40, 0x70);     
      break;
    case 8 :
      midiwrite(0x81, 40, 0x00); 
      midiwrite(0x91, 52, 0x70);     
      break;
    case 12:
      midiwrite(0x81, 52, 0x00); 
      midiwrite(0x91, 40, 0x70);   
      break;        
    default :
      break;
    }    

}

void synthsubseq(long relativpos) 
{ 
  switch(relativpos)
    {
    case 0 :
       midiwrite(0x90, 40, 0x65); 
      break;
    case 7:
      midiwrite(0x80, 40, 0x00);    
      break;
    case 8 :
      midiwrite(0x90, 52, 0x65);     
      break;
    case 15:
      midiwrite(0x80, 52, 0x00);
      break;        
    default :
      break;
    }    

}
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
    if(enablebassline)
    {
      basssubseq(relativpos);
    }
   if(enablesynthline)
   {
      synthsubseq(relativpos);     
   }
  } 
 };


void timercallback()
{
  drumseq(seq_poscnt);
  seq_poscnt++;
  lasttickstamp = millis();
  Timer1.setPeriod(tempodelay * 1000);     
}

void setupvoices(void)
{
    midiprogchange(0xC0,95);
    midiprogchange(0xC1,39);
}
void SomeButtonPressHandler(Button& butt)
{  
 if(enabledrumming)
  {
    if(butt == button2)
    {
      enablesynthline = enablesynthline?0:1;
    };
    if(butt == button3)
    {
      enablebassline = enablebassline?0:1;
    };

  }
  else
  {
    
    if(butt == button2)
    {
       midiwrite(0x90, 40, 0x70); // switch off possible bass notes
    };
    if(butt == button3)
    {  
      midiwrite(0x99, _SNAREDRUM_NOTE, 0x65);   
    }
    if(butt == button4)
    {  
      midiwrite(0x99, _BASSDRUM_NOTE, 0x65);   
    }
  }
}
void stopallnotes(void)
{
  
      midiwrite(0x80, 52, 0x00); // switch off possible synth notes
      midiwrite(0x80, 40, 0x00); // switch off possible synth notes     
      midiwrite(0x80, 28, 0x00); // switch off possible bass notes
      midiwrite(0x80, 40, 0x00); // switch off possible bass notes           
      midiwrite(0x80, 52, 0x00); // switch off possible bass notes
}



void SomeButtonReleaseHandler(Button& butt)
{
  if(enabledrumming)
  {
    //..
  } 
  else
  {
  
    if(butt == button2)
    {  
      midiwrite(0x80, 40, 0x00); // switch off possible bass notes
    }
  }
  
  
}


void SomeButtonHoldHandler(Button& butt)
{
  if(butt==button4)
  {
        stopdrum(); 
  }
}

void setup() {
  Serial.begin(31250);
  pinMode(dreamblaster_enable_pin ,OUTPUT);   // enable the dreamblaster module by pulling high /reset
  digitalWrite(dreamblaster_enable_pin, HIGH);
  delay(300);  // allow 300ms for the dreamblaster to boot
  Timer1.attachInterrupt(timercallback);
  Timer1.initialize(tempodelay*1000);   
  button2.pressHandler(SomeButtonPressHandler);
  button2.releaseHandler(SomeButtonReleaseHandler);
  button3.pressHandler(SomeButtonPressHandler);
  button3.releaseHandler(SomeButtonReleaseHandler);
  button4.pressHandler(SomeButtonPressHandler);
  button4.releaseHandler(SomeButtonReleaseHandler);
  button4.holdHandler(SomeButtonHoldHandler,1000);
  setupvoices();
}

void stopdrum(){
  enabledrumming = 0; 
  prevtimestamp = 0;
  curtimestamp = 0;
  seq_poscnt = 0;
  stopallnotes();
}


void loop() {
  long timesincelastbeat;
  long timetonextbeat;  
  button2.isPressed(); // trigger handling for button
  button3.isPressed(); // trigger handling for button
  button4.isPressed(); // trigger handling for button
  buttonState = digitalRead(button1Pin);
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
          if(bicyclesetup)
            {
               stopdrum(); 
            }
        }
      }
    }
  }

}








