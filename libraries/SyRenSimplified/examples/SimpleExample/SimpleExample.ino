// Simple Example Sample
// Copyright (c) 2012 Dimension Engineering LLC
// See license.txt for license details.

#include <SyRenSimplified.h>

SyRenSimplified SR; // We'll name the SyRen object SR.
                    // For how to configure the SyRen, see the DIP Switch Wizard for
                    //   http://www.dimensionengineering.com/datasheets/SyrenDIPWizard/start.htm
                    // Be sure to select Simplified Serial Mode for use with this library.
                    // This sample uses a baud rate of 9600.
                    //
                    // Connections to make:
                    //   Arduino TX->1  ->  SyRen S1
                    //   Arduino GND    ->  SyRen 0V
                    //   Arduino VIN    ->  SyRen 5V (OPTIONAL, if you want the SyRen to power the Arduino)
                    //
                    // If you want to use a pin other than TX->1, see the SoftwareSerial example.
                                        
void setup()
{
  Serial.begin(9600); // This is the baud rate you chose with the DIP switches.
}


void loop()
{
  SR.motor(127);  // Go forward at full power.
  delay(2000);    // Wait 2 seconds.
  SR.motor(0);    // Stop.
  delay(2000);    // Wait 2 seconds.
  SR.motor(-127); // Reverse at full power.
  delay(2000);    // Wait 2 seconds.
  SR.motor(0);    // Stop.
  delay(2000);
}
