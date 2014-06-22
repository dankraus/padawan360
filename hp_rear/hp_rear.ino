// HoloProjector LED RGB I2C Firmware
// CCv3 SA BY - 2013 Michael Erwin
// Royal Engineers of Naboo
//
// Visit http://code.google.com/p/rseries-open-control/
//
// Firmware Release History
// v001 - Initial Release
// Revisions by Dan Kraus
// Board: ATiny85 8mhz
//
//
/* ATtiny85 as an I2C Slave
 * ATtiny Pin 1 = (RESET)                          ATtiny Pin 8 = VCC (2.7-5.5V)
 * ATtiny Pin 2 = (D1)                             ATtiny Pin 7 = I2C SCK on DS1621  & GPIO
 * ATtiny Pin 3 = (D4)                             ATtiny Pin 6 = (D3)
 * ATtiny Pin 4 = GND                              ATtiny Pin 5 = I2C SDA on DS1621  & GPIO
 * Current Rx & Tx buffers set at 32 bytes - see usiTwiSlave.h
 * Credit and thanks to BroHogan TinyWireS.h & Don Blake for his usiTwiSlave code. 
 * More on TinyWireS usage - see TinyWireS.h
 */


#include "TinyWireS.h"                  // wrapper class for I2C slave routines
#define HP 3 // 1=Front FHP(BLUE), 2=Top THP(RED), 3=Rear RHP(GREEN)

#define delaymulti 2 // set to 2 if ATtiny85

//#define I2C_SLAVE_ADDR  0x19            // FHP i2c slave address (25)
//#define I2C_SLAVE_ADDR  0x1a            // THP i2c slave address (26)
//#define I2C_SLAVE_ADDR  0x1b            // RHP i2c slave address (27)

#define bluePin        1                 // ATtiny Pin 6 LED Pirahna LOWER RIGHT
#define redPin         3                 // ATtiny Pin 2 LED Pirahna UPPER LEFT
#define greenPin       4                 // ATtiny Pin 3 LED Pirahna UPPER RIGHT

unsigned long t=0;
int randomLEDFX;
long twitchLEDTime = 4525; // Long??
//int g = 100;
int f = 200;




void setup(){
  pinMode(bluePin,OUTPUT);               // for general DEBUG use
  pinMode(redPin,OUTPUT);                // for verification
  pinMode(greenPin,OUTPUT);              // for verification

  if (HP==1) {
    TinyWireS.begin(0x19);
    twitchLEDTime = 1000;
    blueON(5);
  } // Start I2C Bus SLAVE I2C Address 25/0x19 (Assigned FHP)
  if (HP==2) {
    TinyWireS.begin(0x1a);
    twitchLEDTime = 2300;
    redON(5);
  }  // Start I2C Bus SLAVE I2C Address 26/0x1a (Assigned THP)
  if (HP==3) {
    TinyWireS.begin(0x1b);
    twitchLEDTime = 1900;
    greenON(5);
  }// Start I2C Bus SLAVE I2C Address 27/0x1b (Assigned RHP)

  delay(2000);

  //  Blink(bluePin,2);                      // show it's alive
  //  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
  //  testLED();
  //  HoloMessage(30);
  //alarm(15);
}


void loop(){
  int i2cEvent = 0;
  if (TinyWireS.available()){           // got I2C input!
    i2cEvent = TinyWireS.receive();     // get the byte from master
    Blink(bluePin,i2cEvent);           // master must wait for this to finish before calling Wire.requestFrom
    receiveEvent(i2cEvent);
  }

  t=millis();
  if (t > twitchLEDTime)
  {
    twitchLED();     // call playRandom routine
    twitchLEDTime = (random(1,10)*500)+millis();   // set the next twitchTime   
  }

}


void Blink(byte led, byte times){ // poor man's display
  for (byte i=0; i< times; i++){
    digitalWrite(led,HIGH);
    delay (250);
    digitalWrite(led,LOW);
    delay (175);
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



void HoloMessage(int MessageLength){
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
    ledOFF();
  }
}

void greenON(int timed){
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}

void blueON(int timed){
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}


void magentaON(int timed){
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}

void yellowON(int timed){
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}

void cyanON(int timed){
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}


void whiteON(int timed){
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);
  if (timed>0){
    delay(timed*1000);
    ledOFF();
  }
}


void ledOFF(){    
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
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
    redON(0); 
    break;
  case 4:
    ledOFF();
    break;
  case 5:
    ledOFF();
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
  int i2cEvent=eventCode;
  sei();
  switch (i2cEvent) {
  case 1:
    whiteON(0);
    break;
  case 2:
    ledOFF();
    break;
  case 3:
    alarm(6); 
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

