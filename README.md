Padawan360
====

Still to come - servo control.


- [Padawan360](#padawan360)
  * [Intro](#intro)
  * [Components](#components)
    + [Arduino Uno (Option 1)](#arduino-uno--option-1-)
    + [Arduino Mega (Option 2 and Recommended!)](#arduino-mega--option-2-and-recommended--)
    + [USB Shield](#usb-shield)
    + [Xbox 360 Wireless USB Receiver](#xbox-360-wireless-usb-receiver)
    + [Xbox 360 Wireless Controller](#xbox-360-wireless-controller)
    + [MP3 Trigger](#mp3-trigger)
    + [Sabertooth Motor Controller - Feet](#sabertooth-motor-controller---feet)
    + [Syren Motor Controller - Dome](#syren-motor-controller---dome)
    + [Amp and Speakers](#amp-and-speakers)
    + [Teeces lights](#teeces-lights)
      - [Optional](#optional)
        * [RSeries RGB HPs.](#rseries-rgb-hps)
        * [Slipring](#slipring)
  * [Setup](#setup)
    + [Arduino IDE](#arduino-ide)
    + [USB Shield](#usb-shield-1)
    + [Sound](#sound)
      - [Troubleshooting](#troubleshooting)
    + [Dome](#dome)
      - [Option 1](#option-1)
      - [Option 2 (Best Option)](#option-2--best-option-)
      - [Dome Troubleshooting](#dome-troubleshooting)
    + [Foot Drive](#foot-drive)
      - [Drive FAQs](#drive-faqs)
    + [Arduino UNO/MEGA](#arduino-uno-mega)
      - [I2C](#i2c)
    + [Controller Pairing](#controller-pairing)
      - [Troubleshooting](#troubleshooting-1)
    + [Teeces Logics](#teeces-logics)
    + [HoloProjectors I2C](#holoprojectors-i2c)
  * [Controls](#controls)
    + [Controller LED Status](#controller-led-status)
    + [Button Guide](#button-guide)
  * [Coming Soon](#coming-soon)
  * [Licensing](#licensing)


## Intro
This is a control system for 1:1 scale remote control R2-D2 powered by Arduinos and controlled with an Xbox 360 Controller. It triggers lights, sounds, and controls foot drive and dome drive motors. It also supports I2C to trigger events in the dome lights or can be extended to interact with anything else that supports I2C

These sketches are heavily based on DanF's Padawan control system that uses Playstation 2 controllers. I found the PS2 controllers to become a bit unreliable and they are increasingly more difficult to come by. I'm also taking advantage of the LEDs around the center Guide button to indicate state of the drive mode (disengaged, engaged w/ speed setting).

The Xbox 360 Controller is over 2.4ghz and uses frequency hopping to avoid interference. At DroidConIII 2014 I was at least over 100ft from the Droid and maintained connection through several walls including two bathrooms with plumbing and tiles. I tested by spinning the dome. When I got out of range, the dome stopped spinning. When I walked a few feet forward, it automatically reconnected and I was able to spin the dome again. Connection status was displayed via LEDs on the controller.

I developed Padawan360 (named with permission from DanF) to use some more easily accessible components with no soldering, wire stripping, etc. It's a bit more plug and play.

A lot of the instructions here are relevant to the original [Padawan PS2](http://astromech.net/droidwiki/index.php?title=PADAWAN) setup instructions. A good chunk of the documentation is reproduced here.

[The thread on Astromech for Padawan360 can be found here ](http://astromech.net/forums/showthread.php?t=19263_)

## Components

- ### Arduino Uno (Option 1)
	For the body. Main Arduino that runs everything. Receivers, code, connection to MP3 trigger, etc runs through here. SparkFun, RadioShack, Amazon, you can find these everywhere these days.

- ### Arduino Mega (Option 2 and Recommended!)
	I run a Mega for the body. It uses the hardware serial pins to connect to the motor controllers. Better performance and memory utilization. The Mega has more memory available too so there's more room to expand and do more if you want. With the Mega, I can also support I2C a bit better.

- ### USB Shield
	Sourced from [Circuits@Home](https://www.circuitsathome.com/products-page/arduino-shields). They've shuffled their links around. Find it on that link labeled "USB Host Shield 2.0 for Arduino – Assembled". They used to offer it assembled and unassembled but as of this writing, they just have it assembled.

	If Amazon is your thing Sainsmart has a USB Host Shield that has been found to be compatible with Padawan360 (Prime elligble!)
	[SainSmart USB Shield](http://www.amazon.com/SainSmart-Compatible-HOST-Shield-Arduino/dp/B006J4G000/ref=sr_1_1?ie=UTF8&qid=1420994477&sr=8-1&keywords=usb+host+shield+2.0+for+arduino). *NOTE* I've seen that some SainSmart isn't sending them with the 2x3 headers that connects the shield to the Arduino board together. This connection is absolutely necessary for the Arduino to talk to the shield over serial (ISCP pins). It will NOT work without this connection. Real dumb. The shield is basically useless without the connection. I spent a while a DroidCon trying to help someone it before realizing that was missing and it's absolutely necessary. You can grab 2x3 headers from Sparkfun for 50 cents. The missing headers seem to be a common issue as the Aamazon reviews are rife with that complaint.

- ### Xbox 360 Wireless USB Receiver
	You can probably source this from Best Buy or something local but it's available on [NewEgg here](http://www.newegg.com/Product/Product.aspx?Item=0NS-000Z-00003)
	I bought a generic one from Microcenter that works fine. Some users have gotten cheap ones from Ebay from off-brands that did not function. I highly recommend buying 1st party official Microsoft receiver. Your mileage may vary with off-brand components here.

- ### Xbox 360 Wireless Controller
	[Controller via Amazon](http://www.amazon.com/Xbox-360-Wireless-Controller-Glossy-Black/dp/B003ZSP0WW).  I have a nice blue one to match R2 personally ;) Like the USB Receiver, I highly recommend buying a 1st party official Microsoft controller. I know one user bought one cheap on Ebay and it didn't even have a sync button and there was no X on the center Home button. Your mileage may vary with off-brand components here.

	**Note:** I have seen the controller bundled with the USB receiver together. It was in the gaming peripheals department in my local Microcenter. It's marketed for PC gaming. Nice to get it in one package if you can if you don't have an extra 360 controller to spare.

- ### MP3 Trigger
	[Sourced from SparkFun](https://www.sparkfun.com/products/11029). Be sure to get a microSD card too. Nothing too big, it's just MP3s.

- ### Sabertooth Motor Controller - Feet
	Depending on your motors you'll want a  [Sabertooth 2x32](https://www.dimensionengineering.com/products/sabertooth2x32), [Sabertooth 2x25](https://www.dimensionengineering.com/products/sabertooth2x25) or [2x12](https://www.dimensionengineering.com/products/sabertooth2x12). The 2x32 and the 2x24 seem to be crossing over price point. Might as well get the 2x32 if you're buying brand new. There's some additional bells and whistles in the 32 and can be programmed via Dimension Engineering's software, but some of those usefuls features are handled in the code, like for speed Ramping for example. My styrene droid with Jaycar motors uses 2x12. Most people tend to use 2x25 or 2x32 for scooter motors and NPC motors. Consult with Dimension Engineering to make sure you get the right one you need. Either one will work with the code

- ### Syren Motor Controller - Dome
	[Syren 10](https://www.dimensionengineering.com/products/syren10)

- ### Amp and Speakers
	Up to you and how big and loud you want. I have a small speaker and a $20 amp from Pyle. A ground loop isolator might be necessary to protect the MP3 trigger and eliminate buzzing from the speaker.

- ### Teeces lights
	The sketch provided here will work for version 3 of the Teeces lighting system for Logic Lights. Use the regular setup and installation instructions for the Teeces system. To control brightness and changing the lighting animations that match some scenes from the films (like the flicker pattern during the Leia Message), the Arduino for the Teeces system needs to be connected to the body Arduino via I2C. Connect the I2C pins from the Body Arduino to the Teeces Arduino. SDA->SDA pins and SCL->SCL pins. On the Uno these are A4 and A5 respectively and on the Mega they are 20 and 21. Verify the I2C pins on your Teeces Arduino. These can be connected via a slipring.

- #### Optional
  - ##### RSeries RGB HPs.
  Sketches provided are for I2C holoprojector boards. The front uses the one with the servo pinouts although the sketch doesn't servo control of the HPs. The top and rear HP are just the regular I2C controlled boards.
  
  - ##### Slipring
  Used to pass power up from the body to the dome and also signal for I2C to control dome lights. The slipring allows wires to go from body to dome and allow the dome to spin 360 degrees without tangling the wires.

## Setup

### Arduino IDE

Be sure you have the latest version of the Arduino IDE installed available [here at the Arduino website](https://www.arduino.cc/en/Main/Software).

Installing libraries and using the Arduino IDE is beyond the scope of this documentation. Refer to the Arduino website's documentation for how to install libraries and upload software to your Arduino.

### USB Shield

Solder the headers on the USB Shield if you purchased the unassembled version. Fit the shield over the Arduino UNO by lining up the pins and pushing in. It should fit snugly. Plug the Xbox 360 Wireless Receiver USB cable into the USB port. That was easy.

If you're using the Mega, orient the USB ports to line up over each other.

### Sound

Connect the following pins from the MP3 Trigger to the Body Arduino

|MP3 Trigger Pin   |Arduino UNO   |
|---|---|
| RX  |1   |
| USBVCC  |5v   |
| GND  |GND   |

**There is a small switch at the bottom of the board labled USB - EXT, make sure that it is pointing to the USB side in order to power it via the Arduino**

In order to load the sounds onto the MP3 Trigger, you will need to grab the sounds files either from [here](http://host-a.net/u/DanF/Padawan_R2_sounds.zip) or [here](http://www.tjcomputers.us/padawan/Padawan_R2_sounds.zip)

Connect the Micro SD card to your computer and upload the files one by one in the numbered order. If you don't do this, you will not be able to control which sound files are triggered. It's picky about file names and ordering. It's annoying but really, do transfer them over one file at a time. You can also use a Windows program called Drivesort to help out here too. Here's a [helpful video from builder Balders on how to load the sounds with Drivesort](https://youtu.be/UsMI2gW7Q40)

Now, insert it into the MP3 Trigger and hook up either the Ground Loop Isolator / Amp / Speaker using the 3.5mm jack on the board.

For anyone with an older version of the MP3Trigger board, you may need to upgrade your firmware before you can use the Arduino to trigger sounds. If your board has the date 3-1-10 on the back (and possibly others) you have the retired version WIG-09715. You can use the board, but it's likely that you'll need updated firmware.

1. Download and unzip the the firmware file available [here](http://astromech.net/droidwiki/images/8/8f/MP3TRIGR.zip).
2. Copy the resulting hex file to a microSD card and rename it to: “MP3TRIGR.HEX”. It does not need to be the only file on the card – it just needs to have that precise filename.
3. Insert the microSD card into your MP3 Trigger and turn the power on while holding down the center navigation switch. Wait for the Status LED to go solid, then cycle the power.
4. You’re now running the new firmware.

#### Troubleshooting
If the MP3 Trigger isn't functioning as expected, or at all, or just behaving a little "odd" - it would be good to make sure you have the most up to date firmware. Version 2.54 is known to work. It can be downloaded [here ](http://robertsonics.com/mp3-trigger-downloads/). The user manual has instruction for installation on the last page (as of this writing) of the instruction manual under "Bootloader". The manual is available from the previous link and also available directly [here](http://robertsonics.com/resources/mp3trigger/MP3TriggerV2UserGuide_2012-02-04.pdf)

Some users had experienced some issues of sounds freezing up and going a bit haywire. It was resolved by using the barrel jack power connector as the Mp3Trigger was browning out. That means that disconnecting the 5v and Ground pins between the Arduino and MP3Trigger and switching flip the switch to EXT.

If the wrong sounds are playing for button presses, they are likely added to the SD card in the wrong order. They must be put on the card in the exact order one at a time or by using Drivesort. Reference [this video](https://youtu.be/UsMI2gW7Q40) for how load the sound files up with Drivesort to ensure they're loaded onto the SD Card in proper order. 

### Dome
Connect the pins for the Syren Motor Controller

|Syren10   |Arduino UNO   |
|---|---|
| S1  |5   |
| 0v  |GND   |

|Syren10   |Arduino Mega   |
|---|---|
| S1  |Serial2 (Tx2)  |
| 0v  |GND   |

|Syren10   |Battery   |
|---|---|
| B+  |Positve   |
| B-  |Negative   |

|Syren10   |Dome Motor   |
|---|---|
| M1  |1 (Positve)   |
| M2  |2 (Negative)   |

If you find that the dome spins the opposite direction, flip M1 and M2. When standing behind the droid, moving left on the left analog stick should rotate the dome left.

If using a Syren speed controller for the dome, you will have choose what type of serial connection to use.
Packetized is the best choice for the Syren, but you might need to change the baud rate . This is done by changing the value of int domeBaudeRate at the beginning of the sketch. All supported rates are listed there.
If you can't get packetized to work, (and some people have had problems) you will have 2 options

#### Option 1
Simplified serial- easy to setup, proven to work, BUT a chance that the dome could start spinning if power is lost to the arduino but not the syren. While I haven't seen this happen without physically yanking the power wire or pressing the reset button the risk is there. I would say now that we know of this possibility, do not use this option if people, especially children, will be close enough to the droid to be injured. To use simplified serial, delete the // in front of the line #define SYRENSIMPLE at the beginning of the sketch.

#### Option 2 (Best Option)
Send the syren to Dimension Engineering to be flashed with the Ver. 2 firmware- This allows the syren to be locked in to a set baud rate and eliminates the auto-bauding problem. I have had this done to my own syren and have tested it with several types of arduinos with no problems. All new syrens are being shipped with this new firmware.

The dip switches on the Syren should be set to 1 and 2 off for Packetized, or 2 and 4 off for Simple.

In some cases, we've noticed that the dome may behave eratically after starting up. If this is the case plugging a 10k resister between the S1 and 0V screw terminals. Simply bend the pins and screw them in along with the wires.

Make sure that you use at least a 14 Guage wire between the motor and the Syren 10. Anything hire can cause problems (fire!)

#### Dome Troubleshooting
**"My Dome just randomly spins!"** **My Dome won't spin!** **My Dome doesn't spin right** etc etc..

Depending on the firmware on the Syren, sometimes you need to change the serial baud rate it communicates at. Change this like `const int DOMEBAUDRATE = 2400;` For packetized options are: 2400, 9600, 19200 and 38400

**"The left analog stick is centered but the dome still spins!"**

You need to just adjust the deadzone `const byte DOMEDEADZONERANGE = 20;` Increase this number until you can let the stick go neutral and nothing moves. The code has some more info on that above that line.


### Foot Drive

|Sabertooth ((2x32 2x25 or 2x12)   |Arduino UNO   |
|---|---|
| S1  |4   |
| S2  |3   |
| 0v  |GND   |

|Sabertooth ((2x32 2x25 or 2x12)   |Arduino Mega   |
|---|---|
| S1  |Serial1 (Tx1)  |
| S2  |Serial1 (Rx1)   |
| 0v  |GND   |

|Sabertooth (2x32 2x25 or 2x12)   |Battery   |
|---|---|
| B+  |Positve   |
| B-  |Negative   |

|Sabertooth ((2x32 2x25 or 2x12)   |Foot Motors   |
|---|---|
| M1A	  |Right Motor Terminal 1   |
| M1B	  |Right Motor Terminal 2   |
| M2A  |Left Motor Terminal 1  |
| M2B  |Left Motor Terminal 2  |

People often seem to get stuck here once they power everything up and find that they push forward on the stick and it drives to left, stick to the left drives drives backwards, etc. Don't fret, motors can be wired positive/negative. It doesn't matter. Start flipping the motor connections to the Sabertooth. First flip  M1A and M1B. If that doesn't fix it, flip it back and try the other. If it still isn't right, then try flipping both sets. R2 is driven like a tank. Spin the right motor forward to go left, left motor forward to go right. Both motors forward, drive forward. Keep that in mind as you troubleshoot.


Please use a maximum of 12 Gauge for the wires going to the Sabertooth and motors/power.

Sabertooth 2x32 / 2x25 / 2x12 dip switches should 1 & 2 OFF and all others ON if using a regular SLA battery

If you're using the 2x32 and you've tinkered with Dimension Engineering's DEScribe Software to tweak settings on your motor controller, under the Serial and USB tab, make sure the Baud Rate is set to 9600. This should be the default, but you should verify.

If you're using 5v to power some components on your I2C device chain, you can use the 5V terminal on the Sabertooth and connect to the positive I2C pin header on the slipring board and 0V on the Sabertooth to GND on the slipring board.

**Note:** If you're using Lithium-Ion batteries (you oddball!) set switch #3 off also.

#### Drive FAQs

Q: The right analog stick is centered but it still drives(turns, drives forward, drives reverse, etc)!

A: You need to just adjust the deadzone `const byte DRIVEDEADZONERANGE = 20;` Increase this number until you can let the stick go neutral and nothing moves. The code has some more info on that above that line.


### Arduino UNO/MEGA
Install the libraries from the Libraries folder. Upload the corresponding padawan360_body sketch from the `padawan360_body` folder for your Arduino (UNO or Mega) sketch to the Arduino. There is one for the UNO and one for the Mega.


#### I2C
This is optional. If you want to trigger light effects in the dome via slipring, connect A4 SLC to SLC on the slipring board and A5 SDA on the Arduino to SDA on the slipring board.

Arduino UNO R3 has separate I2C pins which is really nice but the Circuits@Home USB Shield covers them up. The R3 Board still has I2C pins at A4 and A5.

I've had better performance using the Mega with I2C because of using the hardware serial pins.


### Controller Pairing
Power the Arduino up. It will also power up the Receiver.  Press the big button on the receiver, it will blink. Press the center Guide button the controller to turn the controller too. It will blibk. Press the little sync button located on the top edge of the controller. The controller blink too, then they should sync up and the blinking pattern on the controller will change and swirl. This indicates a connection and that they are paired.

There's also the [Xbox Support Guide](http://support.xbox.com/en-US/xbox-on-other-devices/connections/xbox-360-wireless-gaming-receiver-windows).

#### Troubleshooting
Not triggering sound or getting any movement? Make sure you're paired. The best way is to use the Xbox Wireless library's example sketch.

1. File>Examples>USB Host Shield>Xbox>Xbox Recv
2. Upload that to your Arduino
3. Look at the code and you'll see `Serial.begin`. Take note of that number.
4. Tools>Serial Monitor. Set the baud rate to that number from step 3. 
5. Pair your controller. 

Press buttons, do you see the button names output? If you don't, you're not paired. If it's paired, you'll see the button names in the serial monitor as you press them. 

### Teeces Logics

If you didn't install the libraries in preparation of the body sketch, install the libraries found in the library folder. Upload the padawan360_dome sketch on the Teeces arduino. Connect I2C on the dome end of slipring board. SDA to A4 and SLC to A5.

### HoloProjectors I2C

Mike Erwin put out some great boards with Arduino bootloaders on them. Programmable! I wasn't personally thrilled with some of the lighting effects. I thought some of the colored lights didn't look too authentic. Certainly not entirely fil accurate, but that's ok. Some colored lights didn't really seem to fit random idling and blinking. Using an ISP programmer I tweaked the events more to my liking. Alarms flash yellow and red, otherwise everything else is just white or a cyan color for the Leia message. If you don't want to program them with my versions, you can tweak the `triggerI2C(deviceID, eventNumer)` function calls in the body sketch to match the original event numbers. The originals are found on the RSeries code repo on google code.

## Controls

### Controller LED Status


|LEDs Around Center Guide Button   |Description   |
|---|---|
| 4 Flashing/Blinking in Unison  | Looking for connection to receiver   |
| Rotating/Spinning pattern  | Connected to receiver. Foot motors disengaged  |
| Single LED Top Left - Steady  | Foot Motors Engaged. Speed 1  |
| Single LED Top Right - Steady  | Foot Motors Engaged. Speed 2  |
| Single LED Bottom Left - Steady   | Foot Motors Engaged. Speed 3  |


### Button Guide
Press guide button to turn on the controller.

Press Start button to engage motors!

![Button Guide](https://github.com/dankraus/padawan360/blob/master/xbox360-controller-guide.jpg)

(Button layout image courtesy of LarryJ on Astromech. Thanks!)

## Coming Soon

Dome servos via I2C support.

## Licensing
See the LICENSE file in this repository.
