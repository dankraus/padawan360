/*
Arduino Library for SyRen/Sabertooth
Copyright (c) 2012 Dimension Engineering LLC
http://www.dimensionengineering.com/arduino

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef Sabertooth_h
#define Sabertooth_h   

#include <Arduino.h>

class Sabertooth
{
public:
  Sabertooth(byte address);
  Sabertooth(byte address, Stream& port);

public:
  inline byte    address() const { return _address; }
  inline Stream& port   () const { return _port;    }

  void autobaud(boolean dontWait = false) const;
  void command (byte command, byte value) const;
  
  static void autobaud(Stream& port, boolean dontWait = false);
  
public:
  void motor(byte motor, int power) const;
  void drive            (int power) const;
  void turn             (int power) const;
  
public:
  void setMinVoltage(byte value) const;
  void setMaxVoltage(byte value) const;
  void setBaudRate  (long baudRate) const;
  void setDeadband  (byte value) const;
  void setRamping   (byte value) const;
  void setTimeout   (int  milliseconds) const;
  
private:
  const byte _address;
  Stream&    _port; 
};

class SabertoothPacket
{
public:
  SabertoothPacket(byte command, byte value);

public:
  inline byte command () const { return _command; }
  inline byte value   () const { return _value;   }
  
  byte checksum(byte address) const;
  void getBytes(byte address, byte bytes[4]) const;

public:
  void send(byte address) const;
  void send(byte address, Stream& port) const;
  void send(const Sabertooth& sabertooth) const;
  
public:
  static SabertoothPacket motor(byte motor, int power);
  static SabertoothPacket drive            (int power);
  static SabertoothPacket turn             (int power);

public:
  static SabertoothPacket setMinVoltage(byte value);
  static SabertoothPacket setMaxVoltage(byte value);
  static SabertoothPacket setBaudRate  (long baudRate);
  static SabertoothPacket setDeadband  (byte value);
  static SabertoothPacket setRamping   (byte value);
  static SabertoothPacket setTimeout   (int  milliseconds);

private:
  static SabertoothPacket throttleType(byte command, int power);
  
private:
  byte _command, _value;
};

#endif
