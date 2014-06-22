// HoloProjector I2C Firmware
// BOARD - UNO - ATmel Atmega 328p @ 16 Mhz
// CCv3 SA BY - 2013 Michael Erwin
// Royal Engineers of Naboo
//
// Visit http://code.google.com/p/rseries-open-control/
//
// Firmware Release History
// v001 - Initial Release
//
//
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
   #include "WProgram.h"
#endif
#include <Wire.h> //used to support I2C
#include <Servo.h>// Supports 2 Servos 1A (hp1) & 1B (hp2)

#define HP 1     // 1=Front FHP (BLUE), 2=Top THP (RED), 3=Rear RHP (GREEN)


#define redPin    10
#define greenPin  11
#define bluePin   9
#define systemLED 13

int g = 100;
int f = 200;

unsigned long t=0;
int randomLEDFX;

long loopCount = 0;
long twitchLEDTime = 4525; // Long??
long twitchHPTime = 4525; // Long??


long LeiaMessageTime = 31*1000; 

Servo hp1servo;   // HP 1A default
Servo hp2servo;   // HP 1B default


void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
 
  if (HP==1) {Wire.begin(25);twitchLEDTime = 1000;twitchHPTime=2500;blueON(5);} // Start I2C Bus as Master I2C Address 25 (Assigned FHP)
  if (HP==2) {Wire.begin(26);twitchLEDTime = 2300;twitchHPTime=1750;redON(5);}  // Start I2C Bus as Master I2C Address 26 (Assigned THP)
  if (HP==3) {Wire.begin(27);twitchLEDTime = 1900;twitchHPTime=4500;greenON(5);}// Start I2C Bus as Master I2C Address 27 (Assigned RHP)

  Wire.onReceive(receiveEvent);            // register event so when we receive something we jump to receiveEvent();

  hp1servo.attach(3);  // attaches the servo on Digital Out pin 3 to the servo object 
  hp2servo.attach(5);  // attaches the servo on Digital Out pin 3 to the servo object 
  delay(1000);         // allow time for the servos to attach
  centerHP();
//testLED();
//testSERVOS();
  
}

void loop(){
  loopCount++;
  t=millis();
   if (t > twitchLEDTime)
    {
     twitchLED();     // call playRandom routine
     loopCount = 0;                       // reset loopCount
     twitchLEDTime = (random(1,10)*500)+millis();   // set the next twitchTime   
  }
   if (t > twitchHPTime)
    {
     twitchHP();     // call playRandom routine
     loopCount = 0;                       // reset loopCount
     twitchHPTime = (random(1,10)*750)+millis();   // set the next twitchTime   
  }
  
  
}


void testLED(){
    redON(5);
    greenON(5);
    blueON(5);
    magentaON(5);
    yellowON(5);
    cyanON(5);
    whiteON(5);
}



void alarm(int timed){
  int HPalarmdelay=1000;
  unsigned long timeNew=millis();
  unsigned long flip = timeNew + (timed*1000);  
      while (timeNew < flip) {
        redON(0);
        delay(HPalarmdelay);
        ledOFF();
        delay(HPalarmdelay/2);
        timeNew=millis();
    }
}



void HoloMessage(int MessageLength){
 if (HP==1){ 
  lookdownHP();
}
  t=millis();
  unsigned long lmt=t + (MessageLength*1000);
  while (t < lmt){
    cyanON(0);
    delay(random(1,10)*10);
    whiteON(0);
    delay(random(1,5)*10);
    blueON(0);
    delay(random(4,9)*10);
     t=millis();
  }
  ledOFF();
}




void Disco(int dancetime, int bpm){
  unsigned long DanceBeatDelay = 60000/bpm;
  unsigned long timeNew= millis();
  unsigned long gottadance = (dancetime*1000) + timeNew;

    while (timeNew < gottadance){
      whiteON(0);
      delay(DanceBeatDelay);
      ledOFF();
      delay(DanceBeatDelay);
      timeNew= millis();
    }
}

void SystemFailure(){
    redON(0);
    delay(f);
    blueON(0);
    delay(f);
    magentaON(0);
    delay(f);
    yellowON(0);
    delay(f);
    cyanON(0);
    delay(f);
    whiteON(0);
    delay(f);
    ledOFF();
    redON(0);
    delay(f);
    blueON(0);
    delay(f);
    magentaON(0);
    delay(f);
    yellowON(0);
    delay(f);
    cyanON(0);
    delay(f);
    whiteON(0);
    delay(f);
    ledOFF();
}


void redON(int timed){
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}

void greenON(int timed){
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}

void blueON(int timed){
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}


void magentaON(int timed){
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}

void yellowON(int timed){
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}

void cyanON(int timed){
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}


void whiteON(int timed){
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
    if (timed>0){
      delay(timed*1000);
      ledOFF();}
}


void ledOFF(){    
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
}

void twitchLED() {
     randomLEDFX = random (1,12);    // Pick a number 1 - 7
     switch (randomLEDFX) {
      case 1:
       whiteON(0);
       break;
      case 2:
       ledOFF();
       break;
      case 3:
        blueON(0);
       break;
      case 4:
        cyanON(0);
       break;
      case 5:
        yellowON(0);
       break;
     case 6:
        ledOFF();
       break;
     case 7:
        ledOFF();
       break;
     case 8:
        ledOFF();
       break;
     case 9:
        ledOFF();
       break;
     case 10:
        ledOFF();
       break;
     case 11:
        ledOFF();
       break;  
      default:
       // if nothing else matches, do the default
       // so we are going to do nothing... for that matter not even waste time
       break;
      }
}



void randomLED() {
     randomLEDFX = random (1,8);    // Pick a number 1 - 8
     switch (randomLEDFX) {
      case 1:
       whiteON(0);
       break;
      case 2:
        redON(0);
       break;
      case 3:
        greenON(0);
       break;
      case 4:
        blueON(0);
       break;
      case 5:
        cyanON(0);
       break;
      case 6:
        yellowON(0);
       break;
      case 7:
        magentaON(0);
       break;       
      default: 
       // if nothing else matches, do the default
       // so we are going to do nothing... for that matter not even waste time
       break;
      }
}



// function that executes whenever data is received from an I2C master
// this function is registered as an event, see setup()
void receiveEvent(int eventCode) {
int i2cEvent=Wire.read();
  sei();
     switch (i2cEvent) {
      case 1:
       whiteON(0);
       break;
      case 2:
       ledOFF();
       break;
      case 3:
       alarm(2); 
//       delay(5000);
//        ledOFF();  
       break;
      case 4:
        redON(5);
       break;
      case 5:
        greenON(0);
       break;
      case 6:
        blueON(0);
       break;
      case 7:
        cyanON(0);
       break;
      case 8:
        yellowON(0);
       break;
      case 9:
        HoloMessage(31);
       break;
      case 10:
        Disco(10,120);
       break;
      case 11:
        SystemFailure();
       break;

      default: 
       // if nothing else matches, do the default
       // so we are going to do nothing... for that matter not even waste time
       break;
      }
}

void testSERVOS(){
  hp1servo.write(90);
  hp2servo.write(90);
  delay(500);          // wait for a .5 secs for the servos to move

}



void twitchHP(){
 hp1servo.write(random(20,150));
 hp2servo.write(random(20,150));
 delay(500);
}

void centerHP(){
  hp1servo.write(90);
  hp2servo.write(90);
  delay(500);          // wait for a .5 secs for the servos to move
}

void lookdownHP(){
  hp1servo.write(20);
  hp2servo.write(90);
  delay(500);          // wait for a .5 secs for the servos to move
}
