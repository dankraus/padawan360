// =======================================================================================
// /////////////////////////Padawan Dome Code vT.3///////////////////////////////////////
// =======================================================================================
//                                 danf
//                        Revised  Date: 03/11/13
//   Designed to be used with a second Arduino running the Padawan Body code
//
// Much of this code is from various sources including...
// Paul Murphy (JoyMonkey), John V, Michael Erwin, Michael Smith, Roger Moolay, Chris Reiff and Brad Oakley
//
// EasyTransfer and PS2X_lib libraries by Bill Porter
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
// BOARDtype sets which Arduino we're using
// 1 = Arduino Pro Mini or Uno or Duemilanove ( http://arduino.cc/en/Main/ArduinoBoardProMini )
// 2 = Sparkfun Pro Micro ( https://www.sparkfun.com/products/11098 )
// 3 = Arduino Micro ( http://arduino.cc/en/Main/ArduinoBoardMicro )
#define BOARDtype 1

char TFLDtext[] =  "PADAWAN         "; //PUT YOUR TOP FRONT STARTUP TEXT HERE.
char BFLDtext[] =  "         BOOTING"; //PUT YOUR BOTTOM FRONT STARTUP TEXT HERE.
char RLDtext[] = "ARTOO DETOO  "; //PUT YOUR REAR STARTUP TEXT HERE.

// set brightness levels here (a value of 0-15)...
int RLDbright=1;   //rear Logic
int RPSIbright=15; //rear PSI
int FLDbright=1;   //front Logics
int FPSIbright=15; //front PSI

//delay time of logic display blinkyness (lower = blink faster)
int LogicBlinkTime=75;  

// PSItype sets the type of our front and rear PSI's
// 1 = Teeces original (6 LEDs of each color, arranged side by side)
// 2 = Teeces original checkerboard (6 LEDs of each color arranged in a checkerboard pattern)
// 3 = Teeces V3.2 PSI by John V (13 LEDs of each color, arranged side by side)
// 4 = Teeces V3.2 PSI checkerboard by John V  (13 LEDs of each color, in a checkerboard pattern)
#define PSItype 4
// Wipe speed of PSI's
int wipe = 70;

//#define TESTLOGICS //turns on all logic LEDs at once, useful for troubleshooting

//#define FLDx4 //for an R7 dome with 4 FLDs (if you have 4 FLDs then delete the first // )

// Most builders shouldn't have to edit anything below here. Enjoy!
// =======================================================================================
///////include libs and declare variables////////////////////////////
int scrollPositions[]={
  9,9,27};
long previousMillis[3];
long scrollSpeed = 48;
long scrollCount[4];
#include <LedControl.h>
#include <Wire.h> //used to support I2C
//#include <Servo.h>
struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int hpx; //hp movement
  int hpy; //hp movement
  int hpl; //hp light
  int hpa; // hp automation
  int dsp; // 0 = random, 1 = alarm, 5 = leia, 11 = alarm2
};
RECEIVE_DATA_STRUCTURE domeData;//give a name to the group of data

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
#elif (BOARDtype==3) 
#define DVAL A2
#define CVAL A1
#define LVAL A0
#endif


#if defined(FLDx4)
#define FDEV 5 //5 devices for front chain
#define FPSIDEV 4 //front PSI is device #4 in the chain
#else
#define FDEV 3 //3 devices for front chain
#define FPSIDEV 2 //front PSI is device #2 in the chain
#endif

#define RPSIDEV 3 //rear PSI is device #3 in the chain

//START UP LEDCONTROL...
LedControl lcRear=LedControl(DVAL,CVAL,LVAL,4); //rear chain (Pro Mini/Pro Micro pins)
LedControl lcFront=LedControl(9,8,7,FDEV); //front chain

long flick = 100;
long twitch = 2500;
unsigned long HpMillis = 0;
//Servo HPXservo;
//Servo HPYservo;
const int hpPin =  3;
int displayMode = 0; // 0=random, 5=leia,
int a = 0;
int b = 0;

unsigned long v_grid[3][5]; //this will give 5x32 bits
int posY = 90;
int posX = 90;
int hpY = 0;
int hpX = 0;

long psiMillis = 0;        // will store last time LED was updated
long psiMillis2 = 0;
long psiTime = random(1,5);

#if (PSItype==4)  
#define HPROW 5
#define HPPAT1 B10101010
#define HPPAT2 B01010101
int psiRow[]={
  HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2};
#elif (PSItype==3)  
#define HPROW 5
#define HPPAT1 B11100000
#define HPPAT2 B00010000
int psiRow[]={
  HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT2,HPPAT2,HPPAT2,HPPAT2,HPPAT2};
#elif (PSItype==2) 
#define HPROW 4
#define HPPAT1 B10101000
#define HPPAT2 B01010100
int psiRow[]={
  HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2,HPPAT1,HPPAT2};
#elif (PSItype==1)
#define HPROW 4
#define HPPAT1 B11000000
#define HPPAT2 B00110000
int psiRow[]={
  HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT1,HPPAT2,HPPAT2,HPPAT2,HPPAT2,HPPAT2};
#endif

int psiColor = 0;
int psiNum = 0;
// =======================================================================================

// =======================================================================================

// =======================================================================================
void setup() {
  //HPYservo.attach(A4);
  //HPXservo.attach(A5);
  Serial.begin(57600);//start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 

  //receive I2C events at address 10
  Wire.begin(10);
  Wire.onReceive(receiveEvent); 

  pinMode(hpPin, OUTPUT);
  randomSeed(analogRead(0));

  for(int dev=0;dev<lcRear.getDeviceCount();dev++) {
    lcRear.shutdown(dev, false); //take the device out of shutdown (power save) mode
    lcRear.clearDisplay(dev);
  }
  for(int dev=0;dev<lcFront.getDeviceCount();dev++) {
    lcFront.shutdown(dev, false); //take the device out of shutdown (power save) mode
    lcFront.clearDisplay(dev);
  }
  //set intensity of devices in  rear chain...
  lcRear.setIntensity(0, RLDbright); //RLD
  lcRear.setIntensity(1, RLDbright); //RLD
  lcRear.setIntensity(2, RLDbright); //RLD
  lcRear.setIntensity(3, RPSIbright); //Rear PSI
  //set intensity of devices in front chain...
  for(int dev=0;dev<(lcFront.getDeviceCount()-1);dev++) {
    lcFront.setIntensity(dev, FLDbright);  //front logics (all but the last dev in chain)
  }
  lcFront.setIntensity(FPSIDEV, FPSIbright); //Front PSI
  //HP lights on constantly...
  //lcRear.setRow(3,4,255); //rear psi
  //lcFront.setRow(FPSIDEV,4,255); //front psi

}
// =======================================================================================
void loop() {

  ///startup display  
  if (scrollCount[0]<1)
  {
    scrollingText(0,TFLDtext); //top front text
  }
  if (scrollCount[1]<1)
  {
    scrollingText(1,BFLDtext);//bottom front text
  }
  if (scrollCount[2]<1)
  {
    scrollingText(2,RLDtext);//rear text
    return;
  }

  //
  //unsigned long timeNew= millis();
  //psiFront.Animate(timeNew, lcFront);
  //psiRear.Animate(timeNew, lcRear);
  randomPsi();
  //randomDisplay();

  ////hp movement
//  if (hpY==150)// down
//  {
//    unsigned long currentMillis = millis();
//    if (currentMillis - HpMillis > 3)                                 
//    { 
//      HPYservo.write(posY);               
//      HpMillis = currentMillis;
//      posY += 1;
//      if(posY>= 150) {
//        hpY=1;
//      } // 150 and 30-- adjust these to your application 
//    }
//  } 
//  if (hpY==30)  // up
//  {                                
//    unsigned long currentMillis = millis();
//    if (currentMillis - HpMillis > 3)                                 
//    { 
//      HPYservo.write(posY);               
//      HpMillis = currentMillis;
//      posY-=1;     
//      if(posY<=30) {
//        hpY=0;
//      } // 150 and 30-- adjust these to your application
//    }
//  } 
//
//  if (hpX==150)// right
//  {                                   
//    unsigned long currentMillis = millis();
//    if (currentMillis - HpMillis > 3)                                 
//    { 
//      HPXservo.write(posX);               
//      HpMillis = currentMillis;               
//      posX += 1;
//      if(posX>= 150) {
//        hpX=1;
//      } // 150 and 30-- adjust these to your application 
//    }
//  } 
//  if (hpX==30)  // left
//  {                               
//    unsigned long currentMillis = millis();
//    if (currentMillis - HpMillis > 3)                                 
//    { 
//      HPXservo.write(posX);               
//      HpMillis = currentMillis;
//      posX-=1;     
//      if(posX<=30) {
//        hpX=0;
//      } // 150 and 30-- adjust these to your application
//    }
//  }
//  if (domeData.hpa==0)
//  {
//    autoHP();
//  }
}
// =======================================================================================
/////////////////////////////////////////////////////////////////////////////////////
void randomDisplay()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis[0] > LogicBlinkTime) 
  {   
    previousMillis[0] = currentMillis;
    for (byte device=0; device<3; device++)
      lcRear.setRow(device,random (0,6),random(63,256));
    for (byte device=0; device<FPSIDEV; device++)
      lcFront.setRow(device,random (0,6),random(63,256));
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void leiaDisplay()
{
  unsigned long currentMillis = millis();

  if (scrollCount[3]==2200)

  {    
    for(int address=0;address<3;address++) 
    {        
      lcRear.setRow(address,a,255);
      lcRear.setLed(address,5,a,true);

      lcRear.setRow(address,b,0);
      lcRear.setLed(address,5,b,false);
    }
    for(int address=0;address<FPSIDEV;address++) 
    {        
      lcFront.setRow(address,a,255);
      lcFront.setLed(address,5,a,true);

      lcFront.setRow(address,b,0);
      lcFront.setLed(address,5,b,false);
    } 
    b=a;
    a++;
    if (a>4)
      a=0;
    scrollCount[3]=0;
  }
  else
  {
    scrollCount[3]++;
  }
//  if(currentMillis - previousMillis[0] > 35000)
//  {
//    displayMode = 0;
//    digitalWrite(hpPin, LOW);
//  }

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void alarmDisplay()
{ 
  unsigned long delaytime=100;
  unsigned long currentMillis = millis();
  static unsigned long swtchMillis = millis();
  static byte swtch = 0;
  if (currentMillis - swtchMillis > delaytime)
  { 
    if (swtch == 0) 
    {
      //we have to init all devices in a loop
      for(int row=0;row<5;row++) {

        for(int address=0;address<3;address++) {

          lcRear.setRow(address,row,255);
          lcRear.setLed(address,5,row,true);
        }
        for(int address=0;address<FPSIDEV;address++) {

          lcFront.setRow(address,row,255);
          lcFront.setLed(address,5,row,true);
        }
      }
      swtchMillis = millis();
      swtch = 1;
    }
    else  if (swtch == 1) 
    {  
      for(int row=0;row<5;row++) {

        for(int address=0;address<3;address++) {

          lcRear.setRow(address,row,0);
          lcRear.setLed(address,5,row,false);
        }
        for(int address=0;address<FPSIDEV;address++) {

          lcFront.setRow(address,row,0);
          lcFront.setLed(address,5,row,false);
        }
      }
      swtchMillis = millis();
      swtch = 0;
    }
  }
  if(currentMillis - previousMillis[0] > 2500)
  {
    displayMode = 0;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////// 
void alarm2Display(long playTime)
{
  unsigned long delaytime=200;
  unsigned long currentMillis = millis();
  static unsigned long swtchMillis = millis();
  static byte swtch = 0;
  if (currentMillis - swtchMillis > delaytime)
  { 
    if (swtch == 0) 
    {
      //we have to init all devices in a loop
      for(int row=0;row<5;row++) {
        lcFront.setRow(0,row,15);
        lcFront.setRow(1,row,15);
        lcRear.setRow(1,row,15);
        lcRear.setRow(0,row,0);
        lcRear.setRow(2,row,255);
        lcFront.setLed(0,5,row,true);
        lcFront.setLed(1,5,row,true);
        lcRear.setLed(1,5,row,true);
        lcRear.setLed(2,5,row,true);
        lcRear.setLed(0,5,row,false);
        swtchMillis = millis();
        swtch = 1;
      }
    }
    else  if (swtch == 1) 
    {
      for(int row=0;row<5;row++) {
        lcFront.setRow(0,row,248);
        lcFront.setRow(1,row,248);
        lcRear.setRow(1,row,248);
        lcRear.setRow(0,row,255);
        lcRear.setRow(2,row,0);
        lcFront.setLed(0,5,row,false);
        lcFront.setLed(1,5,row,false);
        lcRear.setLed(1,5,row,false);
        lcRear.setLed(0,5,row,true);
        lcRear.setLed(2,5,row,false);
        swtchMillis = millis();
        swtch = 0;
      }
    }
  }
  if(currentMillis - previousMillis[0] > playTime)
  {
    displayMode = 0;
  }
}
///////////////////////////////////////////////////////////////////  

void showGrid(byte display)
{
  byte minDev, maxDev;
  displayHelper(display, minDev, maxDev);
  //copy from virt coords to device coords
  unsigned char col8=0;
  unsigned char col17=0;
  unsigned char col26=0;



  for (byte row=0; row<5; row++)
  {
    int loops = 0;
    for (byte dev=minDev; dev < maxDev; dev++){
      if(display == 2)
        lcRear.setRow(dev, row, rev(  (v_grid[display][row] & 255L<<(9*loops)) >> (9*loops)    )   );
      else
        lcFront.setRow(dev, row, rev(  (v_grid[display][row] & 255L<<(9*loops)) >> (9*loops)    )   );
      loops++;
    }   
    if ( (v_grid[display][row] & 1L<<8) == 1L<<8)   col8 += 128>>row;
    if (display == 2){
      if ( (v_grid[display][row] & 1L<<17) == 1L<<17) col17 += 128>>row;
      if ( (v_grid[display][row] & 1L<<26) == 1L<<26) col26 += 128>>row;
    }
  }


  if (display == 2){
    lcRear.setRow((minDev), 5, col8);
    lcRear.setRow((minDev+1), 5, col17);
    lcRear.setRow((minDev+2), 5, col26);

  }
  else
    lcFront.setRow(minDev, 5, col8);
}
///////////////////////////////////////////////////////////////////////////////////////
unsigned char rev(unsigned char b) {
  //reverse bits of a byte
  return (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}
void initGrid(byte display)
{
  for (byte row=0; row<6; row++) v_grid[display][row]=0L;
}
int cA[] = { 
  B00000110,
  B00001001,
  B00001111,
  B00001001,
  B00001001 };

int cB[] = { 
  B00000111,
  B00001001,
  B00000111,
  B00001001,
  B00000111 };

int cC[] = { 
  B00000110,
  B00001001,
  B00000001,
  B00001001,
  B00000110 };

int cD[] = { 
  B0000111,
  B0001001,
  B0001001,
  B0001001,
  B0000111 };

int cE[] = { 
  B00001111,
  B00000001,
  B00000111,
  B00000001,
  B00001111 };

int cF[] = { 
  B00001111,
  B00000001,
  B00000111,
  B00000001,
  B00000001 };

int cG[] = { 
  B00001110,
  B00000001,
  B00001101,
  B00001001,
  B00000110 };

int cH[] = { 
  B00001001,
  B00001001,
  B00001111,
  B00001001,
  B00001001 };

int cI[] = { 
  B00000111,
  B00000010,
  B00000010,
  B00000010,
  B00000111 };

int cJ[] = { 
  B00001000,
  B00001000,
  B00001000,
  B00001001,
  B00000110 };

int cK[] = { 
  B00001001,
  B00000101,
  B00000011,
  B00000101,
  B00001001 };

int cL[] = { 
  B00000001,
  B00000001,
  B00000001,
  B00000001,
  B00001111 };

int cM[] = { 
  B00010001,
  B00011011,
  B00010101,
  B00010001,
  B00010001 };

int cN[] = { 
  B00001001,
  B00001011,
  B00001101,
  B00001001,
  B00001001 };

int cO[] = { 
  B00000110,
  B00001001,
  B00001001,
  B00001001,
  B00000110 };

int cP[] = { 
  B00000111,
  B00001001,
  B00000111,
  B00000001,
  B00000001 };

int cQ[] = { 
  B00000110,
  B00001001,
  B00001101,
  B00001001,
  B00010110 };

int cR[] = { 
  B00000111,
  B00001001,
  B00000111,
  B00000101,
  B00001001 };

int cS[] = { 
  B00001110,
  B00000001,
  B00000110,
  B00001000,
  B00000111 };
int cT[] = { 
  B00001111,
  B00000110,
  B00000110,
  B00000110,
  B00000110 };
int cU[] = { 
  B00001001,
  B00001001,
  B00001001,
  B00001001,
  B00000110 };
int cV[] = { 
  B00001001,
  B00001001,
  B00001001,
  B00000110,
  B00000110 };
int cW[] = { 
  B00010001,
  B00010001,
  B00010101,
  B00011011,
  B00010001 };
int cX[] = { 
  B00001001,
  B00001001,
  B00000110,
  B00001001,
  B00001001 };
int cY[] = { 
  B00001001,
  B00001001,
  B00000110,
  B00000110,
  B00000110 };
int cZ[] = { 
  B00001111,
  B00000100,
  B00000010,
  B00000001,
  B00001111 };
int c0[] = { 
  B00000110,
  B00001101,
  B00001011,
  B00001011,
  B00000110 };
//Non-letters
int c1[] = { 
  B00000110,
  B00000101,
  B00000100,
  B00000100,
  B00011111 };
int c2[] = { 
  B00001110,
  B00010001,
  B00000100,
  B00000010,
  B00011111 };  
int c3[] = { 
  B00011111,
  B00010000,
  B00011110,
  B00010000,
  B00011111 };  
int c4[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
int c5[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
int c6[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
int c7[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
int c8[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };      
int c9[] = { 
  B00001100,
  B00010100,
  B00000100,
  B00000100,
  B00011111 };  
// Heart Symbol    
int ch[] = { 
  B00110110,
  B01001001,
  B01000001,
  B00100010,
  B00001000 };
// Tie Fighter Symbol    
int ct[] = { 
  B00100010,
  B00101010,
  B00110110,
  B00101010,
  B00100010 }; 
// R2D2 Symbol    
int cr[] = { 
  B00001110,
  B00011011,
  B00011111,
  B00010101,
  B00010001 } 
; 
// dash - Symbol    
int cd[] = { 
  B00000000,
  B00000000,
  B00001110,
  B00000000,
  B00000000 };
// Film Bar Symbol for use with Leia message
int cf[] = { 
  B00000100,
  B00000100,
  B00000100,
  B00000100,
  B00000100 };

//Blank Symbol
int cb[] = { 
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000 };  

//upSymbol
int cu[] = { 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000 }; 

//down Symbol
int cn[] = { 
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001 }; 
void drawLetter(byte display, char let, int shift)
{
  int *pLetter;
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
  default:
    return;
  }
  //loop thru rows of the letter
  for (byte i=0; i<5; i++) 
  {
    if (shift>0) //positive shift means letter is slid to the right on the display
      v_grid[display][i] += (long)pLetter[i] << shift;
    else //negative shift means letter is slid to the left so that only part of it is visible
    v_grid[display][i] += (long)pLetter[i] >> -shift;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
void scrollingText(byte display, char text[])
{
  unsigned long currentMillis = millis();

  if((currentMillis - previousMillis[display]) >= scrollSpeed) 
  {   
    initGrid(display);
    previousMillis[display] = currentMillis;

    for (int i=0; i<strlen(text); i++)
    {
      drawLetter(display, text[i], i*5 + scrollPositions[display]);
    }

    scrollPositions[display]--;
    if (scrollPositions[display] < -5*(int)strlen(text))
    {
      if (display==2)
      {
        scrollPositions[display]=27;
      }
      else
      {
        scrollPositions[display]=9;
      }

      scrollCount[display]++;

    }
  }
  showGrid(display);

}
//////////////////////////////////////////////////////////////////////////////////////
void displayHelper(byte display, byte &counter, byte &limit){
  if (display == 0){
    counter = 0;
    limit = 1;
  }
  else if(display == 1){
    counter = 1;
    limit = 2;
  }
  else if(display == 2){
    counter = 0;
    limit = 3;
  }
}

///////////////////////////////////////////////////////////////////
void flicker()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis[1] > flick) {
    // save the last time you blinked the LED 
    previousMillis[1] = currentMillis;   

    analogWrite(hpPin, random(255));
    flick = random(100);
  }
}
//////////////////////////////////////////////////////////////////////////////////////

void randomPsi() 
{   
  unsigned long currentMillis = millis();  
  if(currentMillis - psiMillis > psiTime*500) // delay between .5 to 5 seconds
  {
    psiMillis = currentMillis;
    if (psiColor ==0)
    {
      psiColor = 5;
      psiNum=0;
    }
    else
    {
      psiColor = 0;
      psiNum=HPROW-1;
    }
    psiTime = random(1,10);

    // Serial.print("psiTime: ");Serial.println(psiTime);Serial.print("psiColor: ");
    // Serial.println(psiColor);Serial.print("psiNum: ");Serial.println(psiNum);
  }

  unsigned long currentMillis2 = millis();
  if(currentMillis2 - psiMillis2 > wipe) 
  { 
    if (psiNum<HPROW && psiColor == 5)
    {
      psiMillis2 = currentMillis2;
      lcRear.setRow(3, psiNum, psiRow[psiNum+psiColor]);
      lcFront.setRow(2, psiNum, psiRow[psiNum+psiColor]);
      //if (psiNum<3)
      psiNum++;
      //Serial.print("psiNum: "); Serial.println(psiNum);

    }
    else if (psiNum>=0 && psiColor == 0)
    {
      psiMillis2 = currentMillis2;
      lcRear.setRow(3, psiNum, psiRow[psiNum+psiColor]);
      lcFront.setRow(2, psiNum, psiRow[psiNum+psiColor]);
      psiNum--;
      //Serial.print("psiNum: "); Serial.println(psiNum);
    }
  }
}
///////////////////////////////////////////////////////////////////
void autoHP()
{
  if (hpY!=150&&hpY!=30&&hpX!=150&&hpX!=30&&displayMode!=5)
  {  
    unsigned long currentMillis = millis();
    if (currentMillis - HpMillis > twitch)                                 
    {
      hpY=random(31,150);
      hpX=random(31,150);
      HpMillis = currentMillis;
      twitch = random(700,10500);
    }
  }
} 
// =======================================================================================

// =======================================================================================

// function that executes whenever data is received from an I2C master
// this function is registered as an event, see setup()
void receiveEvent(int eventCode) {
  int i2cEvent=Wire.read();
  
  sei();
  switch (i2cEvent) {
    case 1:
      alarmDisplay();
      //randomDisplay();
      break;
   case 5:
     leiaDisplay();
     //randomDisplay();
     break;
   case 11:
     alarm2Display(175000);
     //randomDisplay();
     break;
   default: 
     // if nothing else matches, do the default
     //randomDisplay();
     break;
  }
}
