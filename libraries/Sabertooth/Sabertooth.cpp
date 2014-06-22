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

#include "Sabertooth.h"

Sabertooth::Sabertooth(byte address)
  : _address(address), _port(Serial)
{
  
}

Sabertooth::Sabertooth(byte address, Stream& port)
  : _address(address), _port(port)
{

}

void Sabertooth::autobaud(boolean dontWait) const
{
  autobaud(port(), dontWait);
}

void Sabertooth::autobaud(Stream& port, boolean dontWait)
{
  if (!dontWait) { delay(1500); }
  port.write(0xAA);
  port.flush();
  if (!dontWait) { delay(500); }
}

void Sabertooth::command(byte command, byte value) const
{
  SabertoothPacket(command, value).send(*this);
}

void Sabertooth::motor(byte motor, int power) const
{
  SabertoothPacket::motor(motor, power).send(*this);
}

void Sabertooth::drive(int power) const
{
  SabertoothPacket::drive(power).send(*this);
}

void Sabertooth::turn(int power) const
{
  SabertoothPacket::turn(power).send(*this);
}

void Sabertooth::setMinVoltage(byte value) const
{
  SabertoothPacket::setMinVoltage(value).send(*this);  
}

void Sabertooth::setMaxVoltage(byte value) const
{
  SabertoothPacket::setMaxVoltage(value).send(*this);  
}

void Sabertooth::setBaudRate(long baudRate) const
{
  port().flush();
  SabertoothPacket::setBaudRate(baudRate).send(*this);
  port().flush();
  
  // (1) flush() does not seem to wait until transmission is complete.
  //     As a result, a Serial.end() directly after this appears to
  //     not always transmit completely. So, we manually add a delay.
  // (2) Sabertooth takes about 200 ms after setting the baud rate to
  //     respond to commands again (it restarts).
  // So, this 500 ms delay should deal with this.
  delay(500);
}

void Sabertooth::setDeadband(byte value) const
{
  SabertoothPacket::setDeadband(value).send(*this);
}

void Sabertooth::setRamping(byte value) const
{
  SabertoothPacket::setRamping(value).send(*this);
}

void Sabertooth::setTimeout(int milliseconds) const
{
  SabertoothPacket::setTimeout(milliseconds).send(*this);
}

SabertoothPacket::SabertoothPacket(byte command, byte value)
  : _command(command), _value(value)
{
  
}

byte SabertoothPacket::checksum(byte address) const
{
  return (address + command() + value()) & B01111111;
}

void SabertoothPacket::getBytes(byte address, byte bytes[4]) const
{
  bytes[0] = address;
  bytes[1] = command();
  bytes[2] = value();
  bytes[3] = checksum(address);
}

void SabertoothPacket::send(byte address) const
{
  send(address, Serial);
}

void SabertoothPacket::send(byte address, Stream& port) const
{
  byte bytes[4];
  getBytes(address, bytes);
  port.write(bytes, 4);
}

void SabertoothPacket::send(const Sabertooth& sabertooth) const
{
  send(sabertooth.address(), sabertooth.port());
}

SabertoothPacket SabertoothPacket::motor(byte motor, int power)
{
  return throttleType((motor == 2 ? 4 : 0) + (power < 0 ? 1 : 0), power);
}

SabertoothPacket SabertoothPacket::drive(int power)
{
  return throttleType(power < 0 ? 9 : 8, power);
}

SabertoothPacket SabertoothPacket::turn(int power)
{
  return throttleType(power < 0 ? 11 : 10, power);
}

SabertoothPacket SabertoothPacket::setMinVoltage(byte value)
{
  return SabertoothPacket(2, (byte)min(value, 120));
}

SabertoothPacket SabertoothPacket::setMaxVoltage(byte value)
{
  return SabertoothPacket(3, (byte)min(value, 127));
}

SabertoothPacket SabertoothPacket::setBaudRate(long baudRate)
{
  byte value;
  switch (baudRate)
  {
  case 2400:           value = 1; break;
  case 9600: default: value = 2; break;
  case 19200:          value = 3; break;
  case 38400:          value = 4; break;
  case 115200:         value = 5; break;
  }
  return SabertoothPacket(15, value);
}

SabertoothPacket SabertoothPacket::setDeadband(byte value)
{
  return SabertoothPacket(17, (byte)min(value, 127));
}

SabertoothPacket SabertoothPacket::setRamping(byte value)
{
  return SabertoothPacket(16, (byte)constrain(value, 0, 80));
}

SabertoothPacket SabertoothPacket::setTimeout(int milliseconds)
{
  return SabertoothPacket(14, (byte)((constrain(milliseconds, 0, 12700) + 99) / 100));
}

SabertoothPacket SabertoothPacket::throttleType(byte command, int power)
{
  power = constrain(power, -127, 127);
  return SabertoothPacket(command, (byte)abs(power));
}
