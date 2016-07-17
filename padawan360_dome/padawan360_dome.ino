// =======================================================================================
// ///////////////////////// Padawan360 Dome Code v1.6 ///////////////////////////////////////
// =======================================================================================
//                                Robert Corvus
//                        Revised  Date: 08/04/2015
//                                 Dan K
//                        Revised  Date: 08/02/2014
//   Designed to be used with a second Arduino running the Padawan Body code
//
// Much of this code is from various sources including...
// DanF, Paul Murphy (JoyMonkey), John V, Michael Erwin, Michael Smith, Roger Moolay, Chris Reiff and Brad Oakley
//
// EasyTransfer libraries by Bill Porter
//
// Required : http://arduino.cc/playground/uploads/Main/LedControl.zip
// (that's an Arduino Library - it needs to be downloaded and extracted into your
// Libraries folder for this sketch to work)
//
// Logic Display and PSI Boards should be wired up in two chains (to prevent problems that
// some builders ran into when using a single chain setup for an extended period of time).
// In early 2012 a revised RLD board was released with two outputs to make wiring up in a
// two chain setup a little easier.
// V3.1 OUT2 uses Arduino Pro Micro or Pro Mini pins 9,8,7 for the FLDs and front PSI.
// If you're using the older V3 RLD and don't have the OUT2 pins, don't panic! You can
// still wire up a front chain by connecting directly to the Arduino pins.
//
// If using a V3 RLD...
//   RLD OUT -> Rear PSI
//   Arduino Pins 9,8,7 -> FLD IN D,C,L -> FLD -> Front PSI
//   (you will also need to supply +5V and GND to the front chain; it can go to any pins
//   labeled +5V and GND on any of the FLD or front PSI boards)
//
// If using a V3.1 RLD connections are a little simpler...
//   RLD OUT1 -> Rear PSI
//   RLD OUT2 -> FLD -> FLD -> Front PSI
//

//=============================================================================================
//////////////// Here are the things you need to set to your configuration////////////////////
//=============================================================================================

// Type of Arduino you are using
  // 1 = Arduino Pro Mini or Uno or Duemilanove (the cleanest option, but you need an FTDI adapter)
  // 2 = Sparkfun Pro Micro (but be ready to deal with USB and upload issues)
  // 3 = Arduino Micro 
  // 4 = Arduino Nano (my fav of the small boards)
#define BOARDtype 1

// PSItype sets the type of our front and rear PSI's
  // 1 = Teeces original (6 LEDs of each color, arranged side by side)
  // 2 = Teeces original checkerboard (6 LEDs of each color arranged in a checkerboard pattern)
  // 3 = Teeces V3.2 PSI by John V (13 LEDs of each color, arranged side by side)
  // 4 = Teeces V3.2 PSI checkerboard by John V  (13 LEDs of each color, in a checkerboard pattern)
#define PSItype 4

// For flipped bottom logic displays
  // un-comment the following line if your bottom FLD is flipped upside down
//#define flipBottomFLD

// Set pins used for front hp servos
#define HPY A4
#define HPX A5

// Startup Text
char TFLDtext[] =  "PADAWAN   "; //PUT YOUR TOP FRONT STARTUP TEXT HERE.
char BFLDtext[] =  "       BOOTING"; //PUT YOUR BOTTOM FRONT STARTUP TEXT HERE.
char RLDtext[] = "ARTOO DETOO  "; //PUT YOUR REAR STARTUP TEXT HERE.


// =======================================================================================
// Most builders shouldn't have to edit anything below here, 
// but you can tweak the effects by changing the numbers below
// =======================================================================================

// set brightness levels here (a value of 0-15)
byte LDbrightness = 1;   // Logics
#define PSIbrightness   15  // PSI

// time in ms between logic display updates (lower = blink faster)
#define LOGICupdateDelay 100

// default random blinking styles of the logics. 
// Styles 0 to 2 are for special effects (I used them in Failure)
// Style 3 is legacy Teeces ("blocky random", about 50% off, most LED changing at a time)
// Styles 4 and 5 are recommended, more organic random styles (not all LEDs change at the same time, more LEDs on)
// Stlye 6 is most LEDs on (special effect). 
// 0= almost all off, 1= most off, 2= some off, 3= legacy random, 4= stage 1 organic, 5= stage 2 organic, 6 = stage 3 organic
#define LOGICRandomStyle 4

// update speed of bargraph display
#define BARGRAPHupdateDelay 50

// Delay (in ms) between color wipe steps when PSI changes color (lower= wipe faster)
#define PSIwipeDelay  70

// You can make the front or rear PSI get stuck inbetween colors from time to time by setting these parameters to 1 (0 for normal operation
#define FPSIgetsStuck 1
#define RPSIgetsStuck 1

// parameters for how long and how often it gets stuck
#define PSIstuckhowlong 10000  // stuck time in ms
#define PSIstuckhowoften 5     // how often. 5 means 1 out of 5 swipes on average. So the higher the less often.

// text scroll speed (in ms, lower= scroll faster)
#define SCROLLspeed 56

// Leia effect parameters
#define LEIAduration  35000 // in ms
#define LEIAspeed  100 //2200         // in no particular units, lower=faster

// Alarm effect parameters
#define ALARMduration 4000 // in ms
#define ALARMspeed   100  // in ms

// March effect parameters
#define MARCHduration 175000 // in ms
#define MARCHspeed   555  // in ms. 

// Cantina effect parameters
#define CANTINAduration 165000 // in ms

// Failure effect parameters
#define FAILUREduration 10000
#define FAILUREloops 5
#define FAILUREspeed 75

// =========================================================================
// Only make changes below here if you know what you are doing
// =========================================================================

// include libraries
#include <LedControl.h>
#include <avr/pgmspace.h>
#include <EasyTransfer.h>
#include <Servo.h>
#include <Wire.h>
/*
Different Arduino's have different pin numbers that are used for the rear chains.
 Arduino Pro Mini uses pins 12,11,10 for rear D,C,L
 Sparkfun Pro Micro uses pins 14,16,10 for rear D,C,L
 Arduino Micro uses pins A2,A1,A0 for rear D,C,L
 */

#if (BOARDtype==1) 
#define DVAL 12
#define CVAL 11
#define LVAL 10
#elif (BOARDtype==2) 
#define DVAL 14
#define CVAL 16
#define LVAL 10
#elif (BOARDtype==3||BOARDtype==4) 
#define DVAL A2
#define CVAL A1
#define LVAL A0
#endif

#define FDEV 3    //3 devices for front chain (top FLD, bottom FLD, front PSI
#define FPSIDEV 2 //front PSI is device #2 in the front chain
#define RPSIDEV 3 //rear PSI is device #3 in the rear chain

#define LETTERWIDTH 5 // letter with in pixels
#define MAXSTRINGSIZE 64  // maximum number of letters in a logic display message
#define BAUDRATE 57600

///////global variables////////////////////////////

// This is the LED grid for the 3 logic displays. 
// Since an "unsigned long" is 32 bits wide in gcc, this gives
// 3 grids of 5 rows x 32 bits (which could control 5 rows of 32 LEDs)
// Grid 0 if used for the top FLD, uses only the first 5x9 LEDs/bits
// Grid 1 is used for the bottom FLD, uses only the first 5x9 LEDs/bits
// Grid 3 is used for the RLD, uses only 5x27 LEDs/bits
// This LED grid is sent to the actual display in the function showGrid(display)
unsigned long LEDgrid[3][5]; 

// starting values for text scrolling positions on each display.
int scrollPositions[]={
  9,9,27};

// textScrollCount
// 0,1, and 2 are used in scrollText for each display, counts how many times the string has completely scrolled
long textScrollCount[3];

// display mode
#define NORM 0
#define ALARM 1
#define MARCH 2
#define LEIA 3
#define FAILURE 4
byte displayEffect; // 0 = normal, 1 = alarm, 2 = march, 3 = leia, 4 = failure

// display state for the logics, 0=normal random, 1=text display, 2=bargraph, 3=test, 4=off 
#define RANDOM 0
#define TEXT 1
#define BARGRAPH 2
#define TEST 3
#define OFF 4
byte displayState[3];

// display state for the PSI, 0=normal random, 1=color1, 2=color2, 3=test, 4=off (0, 3 and 4 are shared with displays above)
#define COLOR1 1
#define COLOR2 2
byte psiState[2];

#define BLUE         COLOR1
#define RED          COLOR2
#define YELLOW       COLOR1
#define GREEN        COLOR2

// start with the default display random mode.
byte randomStyle[3]={
  LOGICRandomStyle, LOGICRandomStyle, LOGICRandomStyle};

// tracks if we are in the midst of running an effect
byte effectRunning;

// memory for the text strings to be displayed, add one for the NULL character at the end
char logicText[3][MAXSTRINGSIZE+1];

// Create Maxim devices SPI driver objects for front and rear chains
LedControl lcRear=LedControl(DVAL,CVAL,LVAL,4);  //rear chain, 3 devices for RLD, one for the rear PSI
LedControl lcFront=LedControl(9,8,7,FDEV);       //front chain, 3 devices: TFLD, BFLD, front PSI

// handle to the Serial object, since it's a different one Micro than Minis
HardwareSerial* serialPort;

// the array psiPatterns is used to program the psi wipe
// The first five elements are the LED values for the first color for each row.
// The second five elements are the LED values for the second color for each row.
#if (PSItype==4)  
#define HPROW 5
#define HPPAT1 B10101010
#define HPPAT2 B01010101
int psiPatterns[]={
  HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2};
#elif (PSItype==3)  
#define HPROW 5
#define HPPAT1 B11100000
#define HPPAT2 B00010000
int psiPatterns[]={
  HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT2,HPPAT2,HPPAT2,HPPAT2,HPPAT2};
#elif (PSItype==2) 
#define HPROW 4
#define HPPAT1 B10101000
#define HPPAT2 B01010100
int psiPatterns[]={
  HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2};
#elif (PSItype==1)
#define HPROW 4
#define HPPAT1 B11000000
#define HPPAT2 B00110000
int psiPatterns[]={
  HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT2,HPPAT2,HPPAT2,HPPAT2,HPPAT2};
#endif

// Used for front holo projector
long twitch = 2500;
unsigned long HpMillis = 0;
Servo HPXservo;
Servo HPYservo;
int posY = 90;
int posX = 90;
int hpY = 0;
int hpX = 0;
int hpPin = 3;



// Set up the EasyTransfer communication
EasyTransfer ET;//create object
struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int hpx; //hp movement
  int hpy; //hp movement
  int hpl; //hp light
  int hpa; // hp automation
  int dsp; // 100=no change, 0=random, 1=alarm, 4=whistle, 5=leia, 6=short circut, 10=EQ, 11=alarm2, 21=speed1, 22=speed2, 23=speed3
};
RECEIVE_DATA_STRUCTURE domeData;//give a name to the group of data


//==================
// Initialization //
//==================
void setup() 
{
  // Since the serial names are different on different platforms, need to access through a universal pointer instead
  // Sparkfun Pro Micro and Arduino Micro are based on Atmega32u4 chips, hardware serial is Serial1
#if (BOARDtype==2 || BOARDtype==3) 
  Serial1.begin(BAUDRATE);
  serialPort=&Serial1;
    ET.begin(details(domeData), &Serial1);
#else
  //Arduino Pro Mini is based on an Atmega328, hardware serial is Serial
  Serial.begin(BAUDRATE);
  serialPort=&Serial;
    ET.begin(details(domeData), &Serial);
#endif

  // Used for front holo projector
  HPYservo.attach(HPY);
  HPXservo.attach(HPX);
  randomSeed(analogRead(0));
  pinMode(hpPin, OUTPUT);
  
  Wire.begin(10);
  Wire.onReceive(receiveEvent);
 
 // exit shutdown of each chip and clear displays
  for(int dev=0;dev<lcRear.getDeviceCount();dev++) 
  {
    lcRear.shutdown(dev, false); //take the device out of shutdown (power save) mode
    lcRear.clearDisplay(dev);
  }
  for(int dev=0;dev<lcFront.getDeviceCount();dev++) 
  {
    lcFront.shutdown(dev, false); //take the device out of shutdown (power save) mode
    lcFront.clearDisplay(dev);
  }

  //set intensity of devices in  rear chain...
  lcRear.setIntensity(0, LDbrightness); //RLD
  lcRear.setIntensity(1, LDbrightness); //RLD
  lcRear.setIntensity(2, LDbrightness); //RLD
  lcRear.setIntensity(3, PSIbrightness); //Rear PSI

  //set intensity of devices in front chain...
  for(int dev=0;dev<(lcFront.getDeviceCount()-1);dev++) 
  {
    lcFront.setIntensity(dev, LDbrightness);  //front logics (all but the last dev in chain)
  }
  lcFront.setIntensity(FPSIDEV, PSIbrightness); //Front PSI

   //startup scrolling text. This function blocks until all the strings have scrolled completely off-screen.
  while((textScrollCount[0]<1) || (textScrollCount[1]<1) || (textScrollCount[2]<1)) // keep scrolling until they have all scrolled once
  {
    if (textScrollCount[0]<1) scrollText(0,TFLDtext);  //top front text
    if (textScrollCount[1]<1) scrollText(1,BFLDtext);  //bottom front text
    if (textScrollCount[2]<1) scrollText(2,RLDtext);   //rear text
  }

  // Scrolling is done, switch to random mode
  resetDisplays();
  displayEffect = domeData.dsp;

}///end setup

//===============
/// Main Loop ///
//===============
void loop() 
{
  if(ET.receiveData())
  {
    hpY=domeData.hpy;// hp servo up/down
    hpX=domeData.hpx;// hp servo left/right
    if (domeData.dsp != 100)
    {
      displayEffect = domeData.dsp;// logic display
    }
    if (displayEffect != 5)//if not in leia display
    {
      if (domeData.hpl==1)
      {
        digitalWrite(hpPin, HIGH);//turn on hp light
      }
      else
      {
        digitalWrite(hpPin, LOW);//turn off hp light
      }
    }
  }//end ET.receiveData
  
  switch(displayEffect)
  {
    // go to specific effect
    
    case 0:
    resetDisplays();
    for(byte disp=0; disp<3; disp++)
    {
      randomDisplay(disp);
    }
     randomFPSI();
     randomRPSI();
    break;
    case 1:
    alarmDisplay(ALARMduration);
    break;
  case 4:
    while((textScrollCount[0]<1) || (textScrollCount[1]<1) || (textScrollCount[2]<1)) // keep scrolling until they have all scrolled once
    {
      if (textScrollCount[0]<1) scrollText(0,"* * * * ");  //top front text
      if (textScrollCount[1]<1) scrollText(1," * * *  ");  //bottom front text
      if (textScrollCount[2]<1) scrollText(2,"* * * ");   //rear text
    }

    // Scrolling is done, switch to random mode
    resetDisplays();
    break;

  case 5:
    leiaDisplay(LEIAduration);
    break;
  case 6:
    failureDisplay(FAILUREduration);
    break;
  case 10:
    for(byte disp=0; disp<3; disp++)
    {
      bargraphDisplay(disp,CANTINAduration);
    }
    
    break;
  case 11:
    marchDisplay(MARCHduration);
    break;
  case 21:
    while((textScrollCount[0]<1) || (textScrollCount[1]<1) || (textScrollCount[2]<1)) // keep scrolling until they have all scrolled once
    {
      if (textScrollCount[0]<1) scrollText(0," SPEED ");  //top front text
      scrollText(1,"  1  ");  //bottom front text
      if (textScrollCount[2]<1) scrollText(2,"SPEED 1");   //rear text
    }

    // Scrolling is done, switch to random mode
    resetDisplays();
    break;
  case 22:
    while((textScrollCount[0]<1) || (textScrollCount[1]<1) || (textScrollCount[2]<1)) // keep scrolling until they have all scrolled once
    {
      if (textScrollCount[0]<1) scrollText(0," SPEED ");  //top front text
      scrollText(1,"  2  ");  //bottom front text
      if (textScrollCount[2]<1) scrollText(2,"SPEED 2");   //rear text
    }

    // Scrolling is done, switch to random mode
    resetDisplays();
    break;
  case 23:
    while((textScrollCount[0]<1) || (textScrollCount[1]<1) || (textScrollCount[2]<1)) // keep scrolling until they have all scrolled once
    {
      if (textScrollCount[0]<1) scrollText(0," SPEED ");  //top front text
      scrollText(1,"  3  ");  //bottom front text
      if (textScrollCount[2]<1) scrollText(2,"SPEED 3");   //rear text
    }

    // Scrolling is done, switch to random mode
    resetDisplays();
    break; 
   case 24:
    displayEffect = 0; 
     if (LDbrightness < 15)
       LDbrightness ++;
      
     lcRear.setIntensity(0, LDbrightness); //RLD
     lcRear.setIntensity(1, LDbrightness); //RLD
     lcRear.setIntensity(2, LDbrightness); //RLD
     lcFront.setIntensity(0, LDbrightness); //FLD
     lcFront.setIntensity(1, LDbrightness); //FLD
      
    break;
   case 25:
    displayEffect = 0;
     if (LDbrightness > 0)
       LDbrightness --;
            
     lcRear.setIntensity(0, LDbrightness); //RLD
     lcRear.setIntensity(1, LDbrightness); //RLD
     lcRear.setIntensity(2, LDbrightness); //RLD
     lcFront.setIntensity(0, LDbrightness); //FLD
     lcFront.setIntensity(1, LDbrightness); //FLD
    
    break; 
  default:   // default is random, text or test depending on each displayState
    // cycle for each display
    for(byte disp=0; disp<3; disp++)
    {
      randomDisplay(disp);
    }
     randomFPSI();
     randomRPSI();
    break;
       
  }// end switch(displayEffect)

  ////hp movement
  if (hpY>posY)// down
  {
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > 2)                                 
    { 
      HPYservo.write(posY);               
      HpMillis = currentMillis;
      posY += 1;
      if(posY>= 150) {
        hpY=1;
      } // 150 and 30-- adjust these to your application 
    }
  } 
  if (hpY<posY&&hpY>10)  // up
  {                                
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > 2)                                 
    { 
      HPYservo.write(posY);               
      HpMillis = currentMillis;
      posY-=1;     
      if(posY<=30) {
        hpY=0;
      } // 150 and 30-- adjust these to your application
    }
  } 

  if (hpX>posX)// right
  {                                   
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > 2)                                 
    { 
      HPXservo.write(posX);               
      HpMillis = currentMillis;               
      posX += 1;
      if(posX>= 150) {
        hpX=1;
      } // 150 and 30-- adjust these to your application 
    }
  } 
  if (hpX<posX&&hpX>10)  // left
  {                               
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > 2)                                 
    { 
      HPXservo.write(posX);               
      HpMillis = currentMillis;
      posX-=1;     
      if(posX<=30) {
        hpX=0;
      } // 150 and 30-- adjust these to your application
    }
  }
  if (domeData.hpa==0)
  {
    autoHP();
  }
}//end loop


// ===================================================================================
// Logic Display modes (random, text, test, bargraph, off) and random PSI functions//
//====================================================================================

// Utility Random
void randomDisplay(byte disp)
{
  switch(disp)
  {
  case 0:
    randomDisplayTFLD();
    break;
  case 1:
    randomDisplayBFLD();
    break;
  case 2:
    randomDisplayRLD();
    break;
  default:
    break;
  }
}
/////////////////////////
// Utility Scrolling Text
void textDisplay(byte disp)
{
  if(disp>2) return;
  scrollText(disp, logicText[disp]);
}

//////////////////
// Test (all on)
void testDisplay(byte disp)
{
  if(disp>2) return;
  for(byte i=0; i<5; i++)
  {
    LEDgrid[disp][i]=~0L;
  }
  showGrid(disp);
}

//////////////////
// Utility: Off
void offDisplay(byte disp)
{
  if(disp>2) return;
  for(byte i=0; i<5; i++)
  {
    LEDgrid[disp][i]=0L;
  }
  showGrid(disp);
}

//////////////////////
// bargraph
void bargraphDisplay(byte disp,long playTime)
{ 
  static byte bargraphdata[3][27]; // status of bars

  if(disp>2) return;

  // speed control
  static long previousDisplayUpdate[3]={
    0,0,0  };
  unsigned long currentMillis = millis();
  
  
 // entry and exit timing
  static unsigned long enterMillis;
  if(effectRunning==0) 
  {
    enterMillis=currentMillis;
    effectRunning=1;
  }
  // exit on playTime elapsed. playTime=0 means run forever.
  if(playTime && (currentMillis - enterMillis > playTime))
  {
    effectRunning=0;
    displayEffect=0;
    return;
  }
 
   // PSI stay random during effect
  randomFPSI();
  randomRPSI();
  
  
  
  
  if(currentMillis - previousDisplayUpdate[disp] < BARGRAPHupdateDelay) return;
  previousDisplayUpdate[disp] = currentMillis;

  byte maxcol;
  if(disp==0 || disp==1) maxcol=9;
  else maxcol=27;

  // loop over each column
  for(byte column=0; column<maxcol; column++)
  {
    //byte value=random(0,5);
    byte value = updatebar(disp, column, bargraphdata[disp]);
    byte data=0;
    for(int i=0; i<=value; i++) 
    {
      data |= 0x01<<i;
    }
    //data=B00011111;
    fillColumn( disp, column, data);   
  }
  showGrid(disp);
}

// helper for updating bargraph values
byte updatebar(byte disp, byte column, byte* bargraphdata)
{
  // bargraph values go up or down one pixel at a time
  int variation = random(0,3);            // 0= move down, 1= stay, 2= move up
  int value=(int)bargraphdata[column];    // get the previous value
  if (value==5) value=3;                 // special case, staying stuck at maximum does not look realistic, knock it down
  else value += (variation-1);            // vary it
  if (value<=0) value=0;                  // can't belower than 0
  if (value>5) value=5;                   // can't be higher than 5
  bargraphdata[column]=(byte)value;       // store new value, use byte type to save RAM
  return (byte)value;                     // return new value
}

// helper for dealing with setting LEDgrid by column instead of by row
void fillColumn(byte disp, byte column, byte data)
{
  if (disp==2 && column>27) return;
  if (disp!=2 && column>9) return;
  for(byte row=0; row<5; row++)
  {
    // test if LED is on
    byte LEDon=(data & 1<<row);
    if(LEDon)
      LEDgrid[disp][4-row] |= (1L << column);    // set column bit
    else
      LEDgrid[disp][4-row] &= ~(1L << column); // reset column bit
  }
}

// helper for generating more interesting random patterns for the logics
long randomRow(byte randomMode)
{
  switch(randomMode)
  {
  case 0:  // stage -3
    return (random(256)&random(256)&random(256)&random(256));
    break;
  case 1:  // stage -2
    return (random(256)&random(256)&random(256));
    break;
  case 2:  // stage -1
    return (random(256)&random(256));
    break;
  case 3: // legacy "blocky" mode
    return random(256);
    break;
  case 4:  // stage 1
    return (random(256)|random(256));
    break;
  case 5:  // stage 2
    return (random(256)|random(256)|random(256));
    break;
  case 6:  // stage 3
    return (random(256)|random(256)|random(256)|random(256));
    break;
  default:
    return random(256);
    break;
  }


}


/////////////////////////
// Random Rear Logic
void randomDisplayRLD()
{
  // static parameter for each display
  static long previousDisplayUpdate=0;

  // wait until delay before randomizing again
  unsigned long currentMillis = millis();
  if(currentMillis - previousDisplayUpdate < LOGICupdateDelay) return;
  previousDisplayUpdate = currentMillis;

#if defined(TESTLOGICS) //turn on all logic LEDs to make sure they're all working

  for (int dev=0; dev<3; dev++) // loop on all devices, all rows, rear chain
  {
    for (int row=0; row<6; row++) 
      lcRear.setRow(dev,row,255);
  }


#else  // regular random code

  // loop on all devices, all rows for RLD
  for (int dev=0; dev<3; dev++)
  {
    for (int row=0; row<6; row++)
      lcRear.setRow(dev,row,randomRow(randomStyle[2]));   
    // Or-ing 3 times prevents too many LEDs from blinking off together
    // nice trick from BHD...
  }

#endif

}

//////////////////////
// random top FLD
void randomDisplayTFLD()
{
  // static parameter for each display
  static long previousDisplayUpdate=0;

  // wait until delay before randomizing again
  unsigned long currentMillis = millis();
  if(currentMillis - previousDisplayUpdate < LOGICupdateDelay) return;
  previousDisplayUpdate = currentMillis;

#if defined(TESTLOGICS) //turn on all logic LEDs to make sure they're all working

  int dev=0; // loop on FLD, all rows, front chain
  for (int row=0; row<6; row++)
    lcFront.setRow(dev,row,255); 

#else  // regular random code

  // loop on top FLD devices, all rows on front chain
  int dev=0;
  for (int row=0; row<6; row++)
    lcFront.setRow(dev,row,randomRow(randomStyle[0])); 

#endif

}


//////////////////////
// random bottom FLD
void randomDisplayBFLD()
{
  // static parameter for each display
  static long previousDisplayUpdate=0;

  // wait until delay before randomizing again
  unsigned long currentMillis = millis();
  if(currentMillis - previousDisplayUpdate < LOGICupdateDelay) return;
  previousDisplayUpdate = currentMillis;

#if defined(TESTLOGICS) //turn on all logic LEDs to make sure they're all working

  int dev=1; // loop on FLD, all rows, front chain
  for (int row=0; row<6; row++)
    lcFront.setRow(dev,row,255); 

#else  // regular random code

  // loop on top FLD devices, all rows on front chain
  int dev=1;
  for (int row=0; row<6; row++)
    lcFront.setRow(dev,row,randomRow(randomStyle[1])); 

#endif

}

/////////////////////////////////////////////////////////////////
// PSI Modes
/////////////////////////////////////////////////////////////////

////////////////////////////
// Front PSI static modes (on, off, color1, color2




void setFPSI(byte mode)
{
  switch(mode)
  {
  case OFF:
    for(byte row=0; row<HPROW; row++)
    {
      lcFront.setRow(2, row, 0x00);
    }
    break;
  case TEST:  // all on
    for(byte row=0; row<HPROW; row++)
    {
      lcFront.setRow(2, row, 0xFF);
    }
    break;
  case COLOR1:
    for(byte row=0; row<HPROW; row++)
    {
      lcFront.setRow(2, row, psiPatterns[row]);
    }
    break;
  case COLOR2:
    for(byte row=0; row<HPROW; row++)
    {
      lcFront.setRow(2, row, psiPatterns[row+5]);
    }
    break;
  default:
    break;
  }
}

void setRPSI(byte mode)
{
  switch(mode)
  {
  case OFF:
    for(byte row=0; row<HPROW; row++)
    {
      lcRear.setRow(3, row, 0x00);
    }
    break;
  case TEST:
    for(byte row=0; row<HPROW; row++)
    {
      lcRear.setRow(3, row, 0xFF);
    }
    break;
  case COLOR1:
    for(byte row=0; row<HPROW; row++)
    {
      lcRear.setRow(3, row, psiPatterns[row]);
    }
    break;
  case COLOR2:
    for(byte row=0; row<HPROW; row++)
    {
      lcRear.setRow(3, row, psiPatterns[row+5]);
    }
    break;
  default:
    break;
  }
}



///////////////////////////
// Front PSI random swipe
void randomFPSI() 
{   
  // Static variables per PSI function
  // PSI wipe timers
  static long psiMillisChangeDir=0;       // wait time between change of directions
  static long psiMillisSwipe=0;           // wait time between swipe tests
  // psi delay timer, currently only one, both PSI change at the same time
  static long psiChangeColorDelay=0;              // variable time for changing colors
  // psi color and row counter numbers
  static int psiColor=0;
  static int psiCurrentSwipeRow=0;
  // "stuck" flag
  static byte isStuck=0;

  unsigned long currentMillis = millis(); 
  if(isStuck==1)
  {
    // put direction changing on hold
    psiMillisChangeDir = currentMillis;
  }
  else
  {
    // if time has elapsed, reverse color and direction of swipe, chose another random time
    if(currentMillis - psiMillisChangeDir > psiChangeColorDelay*500) // delay between .5 to 5 seconds
    {
      // choose another random delay
      psiMillisChangeDir = currentMillis;
      psiChangeColorDelay = random(1,11);

      // reverse color and swipe direction
      if (psiColor ==0)
      {
        psiColor = 5;
        psiCurrentSwipeRow=0;
      }
      else
      {
        psiColor = 0;
        psiCurrentSwipeRow=HPROW-1;
      }
    }
  }

  // do next swipe step only if time has elapsed. 
  if(isStuck==1) // pause swipe with long time if PSI is stuck.
  {
    if (currentMillis - psiMillisSwipe < PSIstuckhowlong) return;
  }
  else          // regular swipe speed if not
  {
    if(currentMillis - psiMillisSwipe < PSIwipeDelay) return;
  }

  // get unstuck
  if(isStuck)  isStuck=0;  

  // if we are going to color 2 and haven't reach the end row, do next row
  if (psiCurrentSwipeRow<HPROW && psiColor == 5)
  {
    psiMillisSwipe = currentMillis;
    lcFront.setRow(2, psiCurrentSwipeRow, psiPatterns[psiCurrentSwipeRow+psiColor]);
    psiCurrentSwipeRow++; 
  }
  // if we are going to color 1 and haven't reached the first row, do next row
  else if (psiCurrentSwipeRow>=0 && psiColor == 0)
  {
    psiMillisSwipe = currentMillis;
    lcFront.setRow(2, psiCurrentSwipeRow, psiPatterns[psiCurrentSwipeRow+psiColor]);
    psiCurrentSwipeRow--;
  }

  // let's get stuck once in a while
  if(FPSIgetsStuck && psiCurrentSwipeRow==2 && psiColor==5)
    // && isStuck==0 && isStuck!=2)
  {
    byte onceinawhile=random(PSIstuckhowoften);  // one chance out of 20
    if(onceinawhile==1) isStuck=1;
  }
}

////////////////////////
// Rear PSI random swipe
void randomRPSI() 
{   
  // Static variables, don't need to be globals
  // PSI wipe timers
  static long psiMillisChangeDir=0;       // wait time between change of directions
  static long psiMillisSwipe=0;           // wait time between swipe tests
  // psi delay timer, currently only one, both PSI change at the same time
  static long psiChangeColorDelay=0;              // variable time for changing colors
  // psi color and row counter numbers
  static int psiColor=0;
  static int psiCurrentSwipeRow=0;
  // "stuck" flag
  static byte isStuck=0;  


  unsigned long currentMillis = millis(); 
  if(isStuck==1)
  {
    // put direction changing on hold
    psiMillisChangeDir = currentMillis;
  }
  else
  {
    // if time has elapsed, reverse color and direction of swipe, chose another random time
    if(currentMillis - psiMillisChangeDir > psiChangeColorDelay*500) // delay between .5 to 5 seconds
    {
      // choose another random delay
      psiMillisChangeDir = currentMillis;
      psiChangeColorDelay = random(1,11);

      // reverse color and swipe direction
      if (psiColor ==0)
      {
        psiColor = 5;
        psiCurrentSwipeRow=0;
      }
      else
      {
        psiColor = 0;
        psiCurrentSwipeRow=HPROW-1;
      }
    }
  }

  // do next swipe step only if time has elapsed. 
  if(isStuck==1) // pause swipe with long time if PSI is stuck.
  {
    if (currentMillis - psiMillisSwipe < PSIstuckhowlong) return;
  }
  else          // regular swipe speed if not
  {
    if(currentMillis - psiMillisSwipe < PSIwipeDelay) return;
  }

  // get unstuck
  if(isStuck)  isStuck=0;  

  // if we are going to color 2 and haven't reach the end row, do next row
  if (psiCurrentSwipeRow<HPROW && psiColor == 5)
  {
    psiMillisSwipe = currentMillis;
    lcRear.setRow(3, psiCurrentSwipeRow, psiPatterns[psiCurrentSwipeRow+psiColor]);
    psiCurrentSwipeRow++; 
  }
  // if we are going to color 1 and haven't reached the first row, do next row
  else if (psiCurrentSwipeRow>=0 && psiColor == 0)
  {
    psiMillisSwipe = currentMillis;
    lcRear.setRow(3, psiCurrentSwipeRow, psiPatterns[psiCurrentSwipeRow+psiColor]);
    psiCurrentSwipeRow--;
  }

  // let's get stuck once in a while
  if(RPSIgetsStuck && psiCurrentSwipeRow==2 && psiColor==5)
    // && isStuck==0 && isStuck!=2)
  {
    byte onceinawhile=random(PSIstuckhowoften);  // one chance out of 20
    if(onceinawhile==1) isStuck=1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Special Effect Routines
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////
// Reset Utilities

// resets text scrolling except alphabet
void resetText(byte display)
{
  // reset text
  scrollPositions[display]= (display==2? 27 : 9);
  textScrollCount[display]=0;  
}

// same for all the displays
void resetAllText()
{
  for(byte disp=0; disp<3; disp++) 
  {
    resetText(disp);
  } 
}

// forces exit from effects immediately
void exitEffects()
{
  displayEffect=NORM;
  effectRunning=0;
}

// exit effects, reset scrolling, alphabet back to latin, mode to random
void resetDisplays()
{
  resetAllText();
  exitEffects();
  for(byte disp=0; disp<3; disp++)
  {
    displayState[disp]=NORM;
  }
}



/////////////////////////////////////////////////
// Leia Display: horizontal moving line on all displays
void leiaDisplay(long playTime)
{
  static byte a = 0;     // row counter a for writes
  static byte b = 0;     // row counter b for erase
  static byte color = 0; // counter for PSI color

  unsigned long currentMillis = millis();
  static long swtchMillis;
  static unsigned long enterMillis;

  // entry and exit timing
  if(effectRunning==0) 
  {
    enterMillis=currentMillis;
    effectRunning=1;
    // blank display out
    clearGrid(0); 
    showGrid(0);
    clearGrid(1); 
    showGrid(1);
    clearGrid(2); 
    showGrid(2);

  }
  // exit on playTime elapsed. playTime=0 means run forever.
  if(playTime && (currentMillis - enterMillis > playTime))
  {
    effectRunning=0;
    displayEffect=0;
    digitalWrite(hpPin, LOW);
    return;
  }

  // PSI stay random during effect
  randomFPSI();
  randomRPSI();
   //start holo flicker
  hpFlicker();
  // move the line when it's time
  if (currentMillis - swtchMillis > LEIAspeed)
  {    
    swtchMillis=currentMillis;
    // draw moving line on RLD  
    for(int dev=0;dev<3;dev++) 
    {        
      // set line LEDs on
      lcRear.setRow(dev,a,255);
      lcRear.setLed(dev,5,a,true);

      // set previous line LEDs off 
      lcRear.setRow(dev,b,0);
      lcRear.setLed(dev,5,b,false);
    }

    // draw moving line on TFLD
    int dev=0; 
    {        
      // set line LEDs on
      lcFront.setRow(dev,a,255);
      lcFront.setLed(dev,5,a,true);

      // set line LEDs off
      lcFront.setRow(dev,b,0);
      lcFront.setLed(dev,5,b,false);
    } 

    // draw moving line inverted on BFLD
    dev=1;
    #if defined(flipBottomFLD)
    {        
      // set line LEDs on
      lcFront.setRow(dev,4-a,255);
      lcFront.setLed(dev,5,4-a,true);

      // set line LEDs off
      lcFront.setRow(dev,4-b,0);
      lcFront.setLed(dev,5,4-b,false);
    } 
    #else
    // draw moving line upright on BFLD
    {        
      // set line LEDs on
      lcFront.setRow(dev,a,255);
      lcFront.setLed(dev,5,a,true);

      // set line LEDs off
      lcFront.setRow(dev,b,0);
      lcFront.setLed(dev,5,b,false);
    } 
    #endif
    

    // update row count
    b=a; // remember last row
    a++; // go to next row
    if (a>4) 
    {
      a=0; 
    }
  }
}

///////////////////////////////////////////// 
//alarmDisplay, alternating full on/full off

void alarmDisplay(long playTime)
{ 
  static byte swtch = 0;


  static unsigned long swtchMillis;
  unsigned long currentMillis = millis();

  // entry and exit timing
  static unsigned long enterMillis;
  if(effectRunning==0) 
  {
    enterMillis=currentMillis;
    effectRunning=1;
  }
  // exit on playTime elapsed. playTime=0 means run forever.
  if(playTime && (currentMillis - enterMillis > playTime))
  {
    effectRunning=0;
    displayEffect=0;
    return;
  }

  // wait for the next period
  if (currentMillis - swtchMillis > ALARMspeed)
  {    
    if (swtch == 0) 
    {
      clearGrid(0);
      clearGrid(1);
      clearGrid(2);

      // everything on
      for(int row=0;row<5;row++)
      {
        LEDgrid[0][row]=0xFFFFFFFFL;      
        LEDgrid[1][row]=0xFFFFFFFFL;     
        LEDgrid[2][row]=0xFFFFFFFFL;   
      }

      showGrid(0);
      showGrid(1);
      showGrid(2);

      setFPSI(RED);
      setRPSI(YELLOW);

      swtchMillis = millis();
      swtch = 1;
    }
    else  if (swtch == 1) 
    {
      // everything off
      clearGrid(0);
      clearGrid(1);
      clearGrid(2); 

      showGrid(0);
      showGrid(1);
      showGrid(2);

      setFPSI(OFF);
      setRPSI(OFF);      

      swtchMillis = millis();
      swtch = 0;
    }
  }
}

///////////////////////////////////////////////////
// March Effect, blocks alternating sideways 
void marchDisplay(long playTime)
{
  unsigned long currentMillis = millis();
  static unsigned long swtchMillis = millis();
  static byte swtch = 0;

  // entry and exit timing
  static unsigned long enterMillis;
  if(effectRunning==0) 
  {
    enterMillis=currentMillis;
    effectRunning=1;
  }
  // exit on playTime elapsed. playTime=0 means run forever.
  if(playTime && (currentMillis - enterMillis > playTime))
  {
    effectRunning=0;
    displayEffect=0;
    return;
  }

  if (currentMillis - swtchMillis > MARCHspeed)
  { 
    if (swtch == 0) 
    {
      clearGrid(0);
      clearGrid(1);
      clearGrid(2);

      for(int row=0;row<5;row++)
      {
        LEDgrid[0][row]=31L;      // first 5 column on
        LEDgrid[1][row]=31L;     // first 5 column on
        LEDgrid[2][row]=16383L;   // first 14 column on
      }

      showGrid(0);
      showGrid(1);
      showGrid(2);

      setFPSI(RED);
      setRPSI(YELLOW);      

      swtchMillis = millis();
      swtch = 1;
    }
    else  if (swtch == 1) 
    {
      clearGrid(0);
      clearGrid(1);
      clearGrid(2); 

      for(int row=0;row<5;row++)
      {
        LEDgrid[0][row]= ~15L;      // first 4 column off
        LEDgrid[1][row]= ~15L;     // first 4 column off
        LEDgrid[2][row]= ~8191L;   // first 13 column off
      }

      showGrid(0);
      showGrid(1);
      showGrid(2);

      setFPSI(BLUE);
      setRPSI(GREEN); 

      swtchMillis = millis();
      swtch = 0;
    }
  }
}


/////////////////////////////////////////////////////
// Failure: screen having less and less dots

// failure helper function
void showFailure(byte style)
{
  // TLFLD
  for (int row=0; row<6; row++)
    lcFront.setRow(0,row,randomRow(style));
  // BFLD
  for (int row=0; row<6; row++)
    lcFront.setRow(1,row,randomRow(style));
  // RLD
  for (int dev=0; dev<3; dev++)
  {
    for (int row=0; row<6; row++)
      lcRear.setRow(dev,row,randomRow(style));
  }
  // FPSI
  for (int row=0; row<HPROW; row++)
    lcFront.setRow(2,row,randomRow(style));
  // RPSI
  for (int row=0; row<HPROW; row++)
    lcRear.setRow(3,row,randomRow(style));
}

// failure main function
void failureDisplay(long playTime)
{
  static int loopCount=0;  // number of loops
  static unsigned long lastMillis;
  unsigned long currentMillis = millis();
  static long blinkSpeed = FAILUREspeed;

  // entry and exit timing
  static unsigned long enterMillis;
  if(effectRunning==0) 
  {
    blinkSpeed = FAILUREspeed;
    loopCount=0;
    enterMillis=currentMillis;
    effectRunning=1;
  }
  // exit on playTime elapsed. playTime=0 means run forever.
  if(playTime && (currentMillis - enterMillis > playTime))
  {
    effectRunning=0;
    displayEffect=0;
    return;
  }

  // speed control
  if (currentMillis - lastMillis < blinkSpeed) return;
  lastMillis = currentMillis;
  loopCount++;

  // every 2200 counts, move the line
  if (loopCount<FAILUREloops)
  {    
    blinkSpeed=FAILUREspeed;
    showFailure(4);
  }
  else if(loopCount<2*FAILUREloops)
  {
    blinkSpeed=2*FAILUREspeed;
    showFailure(3);
  }
  else if(loopCount<3*FAILUREloops)
  {
    blinkSpeed=3*FAILUREspeed;
    showFailure(2);
  }
  else if(loopCount<4*FAILUREloops)
  {
    blinkSpeed=4*FAILUREspeed;
    showFailure(1);
  }
  else if(loopCount<5*FAILUREloops)
  {
    showFailure(0);    
  }
  else
  {
    // just stay stuck at the end.
  }
}



//////////////////////////////////////////////////////
// showGrid: main function to display LED grid on Logics
/////////////////////////////////////////////////////

// byte reversal fast RAM lookup table
uint8_t revlookup[16] = {
  0x0, 0x8, 0x4, 0xC,
  0x2, 0xA, 0x6, 0xE,
  0x1, 0x9, 0x5, 0xD,
  0x3, 0xB, 0x7, 0xF };
// byte reversal function
uint8_t rev( uint8_t n )
{
   return (revlookup[n&0x0F] << 4) | revlookup[n>>4];
}

// blank grid, to turn of all LEDs
void clearGrid(byte display)
{
  for (byte row=0; row<5; row++) LEDgrid[display][row]=0L;
}

// Sends LED Grid to actual LEDs on the logic displays
void showGrid(byte display)
{
  // didplay=0 Top FLD
  // display=1 Bottom FLD
  // display=2 RLD

  // Every 9th column of the displays maps to the 6th row of the Maxim chip
  unsigned char col8=0;  // 9th column of FLDs and RLD, maps to 6th row of device 0
  unsigned char col17=0; // 18th column of RLD, goes to 6th row of RLD device 1
  unsigned char col26=0; // 27th column of RLD, goes to 6th row of RLD device 2 

  // Colums 0-7 map with a byte reversal

  switch(display)
  {
  case 0:  // Top FLD

    for (byte row=0; row<5; row++) // loop on first 5 rows
    {
      // extract first 8 bits, reverse, send to device 0, front chain which is top FLD
      lcFront.setRow(0, row, rev(LEDgrid[display][row] & 255L));
      // If the LED at column 8 is on, add it to the extra row (starting "left" at MSB)
      if ( (LEDgrid[display][row] & 1L<<8) == 1L<<8)   col8 += 128>>row;
    }
    // send the 9th column (logical 8) as a 6th row or the Maxim (logical 5)
    lcFront.setRow(0, 5, col8);

    break;

  case 1:  // Bottom FLD
    #if defined(flipBottomFLD)
    // Bottom FLD is upside down. So rows are inverted. Top is bottom, left is right
    for (byte row=0; row<5; row++) // loop on first 5 rows
    {
      // extract bits 2-9, do not reverse, send to device 1, start with device row 4 (invert top and bottom)
      lcFront.setRow(1, 4-row, (LEDgrid[display][row] & 255L<<1) >> 1);
      // If the LED at first column is on, add it to the extra row (starting "left" at MSB)
      // we still call it col8, but with the inverted display it really is col 0
      // we fill in the forward direction starting from bit 3
      if ( (LEDgrid[display][row] & 1L) == 1L)   col8 += 8<<row;
    }
    // send the column 0 as a 6th row or the Maxim (logical row 5)
    lcFront.setRow(1, 5, col8);
    
    #else
    for (byte row=0; row<5; row++) // loop on first 5 rows
    {
      // extract first 8 bits, reverse, send to device 0, front chain which is top FLD
      lcFront.setRow(1, row, rev(LEDgrid[display][row] & 255L));
      // If the LED at column 8 is on, add it to the extra row (starting "left" at MSB)
      if ( (LEDgrid[display][row] & 1L<<8) == 1L<<8)   col8 += 128>>row;
    }
    // send the 9th column (logical 8) as a 6th row or the Maxim (logical 5)
    lcFront.setRow(1, 5, col8);
   #endif
    break;


  case 2:  // RLD
    for (byte row=0; row<5; row++) // loop on first 5 rows
    {
      int loops = 0;
      for (byte dev=0; dev < 3; dev++) // RLD has 3 Maxim chip devices
      {
        // extract byte at column 0, 9 and 18, reverse byte order, and send to device
        lcRear.setRow(dev, row, rev(  (LEDgrid[display][row] & 255L<<(9*loops)) >> (9*loops) ));
        loops++;
      }
      // If the LED at column 8, 17 or 26 is on, add it to the extra row (starting "left" at MSB)
      if ( (LEDgrid[display][row] & 1L<<8) == 1L<<8)   col8 += 128>>row;
      if ( (LEDgrid[display][row] & 1L<<17) == 1L<<17) col17 += 128>>row;
      if ( (LEDgrid[display][row] & 1L<<26) == 1L<<26) col26 += 128>>row;
    }
    // send the extra columns as a 6th row or the Maxim (logical row 5)
    lcRear.setRow(0, 5, col8);
    lcRear.setRow(1, 5, col17);
    lcRear.setRow(2, 5, col26);
    break;

  default:
    break;
  }
}
///////////////////////end showGrid routines/////////////////////////////////



//////////////////////////////////////
// Text Display Routines
/////////////////////////////////////

//////////////////////
// Set String
void setText(byte disp, char* message)
{
  strncpy(logicText[disp], message, MAXSTRINGSIZE);
  logicText[disp][MAXSTRINGSIZE]=0; // just in case
}

///////////////////////////////
// Latin Alphabet, put in PROGMEM so save RAM
const int cA[] PROGMEM = { 
  B00000110,
  B00001001,
  B00001111,
  B00001001,
  B00001001 };

const int cB[] PROGMEM = { 
  B00000111,
  B00001001,
  B00000111,
  B00001001,
  B00000111 };

const int cC[] PROGMEM = { 
  B00000110,
  B00001001,
  B00000001,
  B00001001,
  B00000110 };

const int cD[] PROGMEM = { 
  B0000111,
  B0001001,
  B0001001,
  B0001001,
  B0000111 };

const int cE[] PROGMEM = { 
  B00001111,
  B00000001,
  B00000111,
  B00000001,
  B00001111 };

const int cF[] PROGMEM = { 
  B00001111,
  B00000001,
  B00000111,
  B00000001,
  B00000001 };

const int cG[] PROGMEM = { 
  B00001110,
  B00000001,
  B00001101,
  B00001001,
  B00000110 };

const int cH[] PROGMEM = { 
  B00001001,
  B00001001,
  B00001111,
  B00001001,
  B00001001 };

const int cI[] PROGMEM = { 
  B00000111,
  B00000010,
  B00000010,
  B00000010,
  B00000111 };

const int cJ[] PROGMEM = { 
  B00001000,
  B00001000,
  B00001000,
  B00001001,
  B00000110 };

const int cK[] PROGMEM = { 
  B00001001,
  B00000101,
  B00000011,
  B00000101,
  B00001001 };

const int cL[] PROGMEM = { 
  B00000001,
  B00000001,
  B00000001,
  B00000001,
  B00001111 };

const int cM[] PROGMEM = { 
  B00010001,
  B00011011,
  B00010101,
  B00010001,
  B00010001 };

const int cN[] PROGMEM = { 
  B00001001,
  B00001011,
  B00001101,
  B00001001,
  B00001001 };

const int cO[] PROGMEM = { 
  B00000110,
  B00001001,
  B00001001,
  B00001001,
  B00000110 };

const int cP[] PROGMEM = { 
  B00000111,
  B00001001,
  B00000111,
  B00000001,
  B00000001 };

const int cQ[] PROGMEM = { 
  B00000110,
  B00001001,
  B00001101,
  B00001001,
  B00010110 };

const int cR[] PROGMEM = { 
  B00000111,
  B00001001,
  B00000111,
  B00000101,
  B00001001 };

const int cS[] PROGMEM = { 
  B00001110,
  B00000001,
  B00000110,
  B00001000,
  B00000111 };
const int cT[] PROGMEM = { 
  B00001111,
  B00000110,
  B00000110,
  B00000110,
  B00000110 };
const int cU[] PROGMEM = { 
  B00001001,
  B00001001,
  B00001001,
  B00001001,
  B00000110 };
const int cV[] PROGMEM = { 
  B00001001,
  B00001001,
  B00001001,
  B00000110,
  B00000110 };
const int cW[] PROGMEM = { 
  B00010001,
  B00010001,
  B00010101,
  B00011011,
  B00010001 };
const int cX[] PROGMEM = { 
  B00001001,
  B00001001,
  B00000110,
  B00001001,
  B00001001 };
const int cY[] PROGMEM = { 
  B00001001,
  B00001001,
  B00000110,
  B00000110,
  B00000110 };
const int cZ[] PROGMEM = { 
  B00001111,
  B00000100,
  B00000010,
  B00000001,
  B00001111 };
const int c0[] PROGMEM = { 
  B00000110,
  B00001101,
  B00001011,
  B00001011,
  B00000110 };
//Non-letters
const int c1[] PROGMEM = { 
  B00000110,
  B00000101,
  B00000100,
  B00000100,
  B00011111 };
const int c2[] PROGMEM = { 
  B00001110,
  B00010001,
  B00000100,
  B00000010,
  B00011111 };  
const int c3[] PROGMEM = { 
  B00011111,
  B00010000,
  B00011110,
  B00010000,
  B00011111 };  
const int c4[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
const int c5[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
const int c6[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
const int c7[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
const int c8[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };      
const int c9[] PROGMEM = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
// Heart Symbol    
const int ch[] PROGMEM = { 
  B00001010,
  B00010101,
  B00010001,
  B00001010,
  B00000100 };
// Tie Fighter Symbol    
const int ct[] PROGMEM = { 
  B00100010,
  B00101010,
  B00110110,
  B00101010,
  B00100010 }; 
// R2D2 Symbol    
const int cr[] PROGMEM = { 
  B00001110,
  B00011011,
  B00011111,
  B00010101,
  B00010001 } 
; 
// dash - Symbol    
const int cd[] PROGMEM = { 
  B00000000,
  B00000000,
  B00001110,
  B00000000,
  B00000000 };
// Film Bar Symbol for use with Leia message
const int cf[] PROGMEM = { 
  B00000100,
  B00000100,
  B00000100,
  B00000100,
  B00000100 };

//Blank Symbol
const int cb[] PROGMEM = { 
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000 };  

//upSymbol
const int cu[] PROGMEM = { 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000 }; 

//down Symbol
const int cn[] PROGMEM = { 
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001 }; 

//Dot Symbol
const int cdot[] PROGMEM = { 
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000100 };  

//////////////////////////////////////////////////////////////////
// retrieve latin alphabet letter from progam memory
void getLatinLetter(int* letterbitmap, char let)
{
  // pLetter will be a pointer to program memory
  const int* pLetter;

  // get pointer to program memory from character
  switch (let)
  {
  case 'A': 
    pLetter=cA; 
    break;
  case 'B': 
    pLetter=cB; 
    break;
  case 'C': 
    pLetter=cC; 
    break;
  case 'D': 
    pLetter=cD; 
    break;
  case 'E': 
    pLetter=cE; 
    break;
  case 'F': 
    pLetter=cF; 
    break;
  case 'G': 
    pLetter=cG; 
    break;
  case 'H': 
    pLetter=cH; 
    break;
  case 'I': 
    pLetter=cI; 
    break;
  case 'J': 
    pLetter=cJ; 
    break;
  case 'K': 
    pLetter=cK; 
    break;
  case 'L': 
    pLetter=cL; 
    break;
  case 'M': 
    pLetter=cM; 
    break;
  case 'N': 
    pLetter=cN; 
    break;
  case 'O': 
    pLetter=cO; 
    break;
  case 'P': 
    pLetter=cP; 
    break;
  case 'Q': 
    pLetter=cQ; 
    break;
  case 'R': 
    pLetter=cR; 
    break;
  case 'S': 
    pLetter=cS; 
    break;
  case 'T': 
    pLetter=cT; 
    break;
  case 'U': 
    pLetter=cU; 
    break;
  case 'V': 
    pLetter=cV; 
    break;
  case 'W': 
    pLetter=cW; 
    break;
  case 'X': 
    pLetter=cX; 
    break;
  case 'Y': 
    pLetter=cY; 
    break;
  case 'Z': 
    pLetter=cZ; 
    break;
    //non-letters
    //numbers
  case '0': 
    pLetter=c0; 
    break;
  case '1': 
    pLetter=c1; 
    break;
  case '2': 
    pLetter=c2; 
    break;
  case '3': 
    pLetter=c3; 
    break;
  case '4': 
    pLetter=c4; 
    break;
  case '5': 
    pLetter=c5; 
    break;
  case '6': 
    pLetter=c6; 
    break;
  case '7': 
    pLetter=c7; 
    break;
  case '8': 
    pLetter=c8; 
    break;
  case '9': 
    pLetter=c9; 
    break;
    //special characters
  case '*': 
    pLetter=ch; 
    break;
  case '#': 
    pLetter=ct; 
    break;
  case '@': 
    pLetter=cr; 
    break;
  case '-': 
    pLetter=cd; 
    break;
  case '|': 
    pLetter=cf; 
    break;
  case '.': 
    pLetter=cdot; 
    break;
    //whitespace
  case ' ': 
    pLetter=cb; 
    break;
  case '<': 
    pLetter=cu; 
    break;
  case '>': 
    pLetter=cn; 
    break;
  default : 
    pLetter=cb; 
    break;
    break;
  }

  // move data back from program memory to RAM
  for(byte i=0; i<5; i++)
  {
    letterbitmap[i]=(int)pgm_read_word(&(pLetter[i]));
  }
}

//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Draws in a letter on the LED grid     
// shift=0 draws starting from the left edge (column 0)
// shift>0 slide letter further towards the right
// shift<0 slide letter further towards the left (becomes only partly visible)
void drawLetter(byte display, char let, int shift)
{

  // return immediately if the letter won't show
  if(shift < -LETTERWIDTH ||  shift>27) return;

  // allocate RAM space for the bitmap
  int letterBitmap[5];
  getLatinLetter(letterBitmap, let);
  
  //loop thru rows of the letter
  // shift=0 draws starting from the left edge (column 0)
  // shift>0 slide letter further towards the right
  // shift<0 slide letter further towards the left (becomes only partly visible)
  for (byte i=0; i<5; i++) 
  {
    if (shift>0) //positive shift means letter is slid to the right on the display
      LEDgrid[display][i] |= ((long)letterBitmap[i]) << shift;
    else //negative shift means letter is slid to the left so that only part of it is visible
    LEDgrid[display][i] |= ((long)letterBitmap[i]) >> -shift;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Scrolls the given text string on the given display
// Call repeatedly to keep the text scrolling.
// A scrolling count global tells you how many times the string has scrolled entirely
// There is no reset of the original position, should be accomplished elsewhere
// (9, 9 and 27 respectively)
void scrollText(byte display, char text[])
{
  static long previousTextScroll[3];

  // wait until next update cycle
  unsigned long currentMillis = millis();
  if((currentMillis - previousTextScroll[display]) < SCROLLspeed) return;
  previousTextScroll[display] = currentMillis;

  // LED grid to all off
  clearGrid(display);

  // draw all letters in the grid, scrolled according to the global scrollPosition[display]
  // each letter is moved 5 pixels from the next letter.
  // Positive scroll means moves towards the right
  // So scrollPosition should start at the last column and be decremented
  for (int i=0; i<strlen(text); i++)
  {
    int shift=i*LETTERWIDTH + scrollPositions[display];
    //if(shift > -(LETTERWIDTH+1) &&  shift<28)
    {
      drawLetter(display, text[i], shift);
    }
  }

  // this moves the text one step to the left, it will eventually become negative
  // and some text will start to disappear
  scrollPositions[display]--;

  // if the whole text is off screen to the left
  if (scrollPositions[display] < -LETTERWIDTH*(int)strlen(text))
  {
    // resets the scroll to just off screen to the right
    if (display==2) scrollPositions[display]=27;
    else scrollPositions[display]=9;

    // increment global scroll count
    // Right now this is used once at startup to stop calling this in the loop 
    // once the startup text has scrolled once
    // warning there is no reset to this
    textScrollCount[display]++;
  }

  // show text on logics
  showGrid(display);
}
//////////////////////////////////////////////////
///HP flicker
void hpFlicker()
{
  static unsigned long swtchMillis = millis();
  unsigned long currentMillis = millis();
  static long flick = 100;
  if(currentMillis - swtchMillis > flick) {
    // save the last time you blinked the LED 
    swtchMillis = currentMillis;   

    analogWrite(hpPin, random(255));
    flick = random(100);
  }
}
///////////////////////////////////////////////////////////////////
void autoHP()
{
  if (hpY!=150&&hpY!=30&&hpX!=150&&hpX!=30&&displayEffect!=5)
  {  
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > twitch){
      hpY=random(31,150);
      hpX=random(31,150);
      HpMillis = currentMillis;
      twitch = random(700,10500);
    }
  }
}
// =======================================================================================

// function that executes whenever data is received from an I2C master
// this function is registered as an event, see setup()
void receiveEvent(int eventCode) {
  int i2cEvent = Wire.read();
  sei();
  
  if (i2cEvent != 100){
    // logic display
    displayEffect = i2cEvent;
  }
}
