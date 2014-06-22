/*
Arduino Library for SyRen Simplified Serial
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

#include "SyRenSimplified.h"

SyRenSimplified::SyRenSimplified()
  : _port(Serial)
{
  
}

SyRenSimplified::SyRenSimplified(Stream& port)
  : _port(port)
{
  
}

void SyRenSimplified::motor(int power)
{
  byte command;
  power = constrain(power, -127, 127);
  command = (power < 0 ? 127 : 128) + power;
  _port.write(command);
}

void SyRenSimplified::motor(byte motor_, int power)
{
  if (motor_ == 1) { motor(power); }
}

void SyRenSimplified::stop()
{
  motor(0);
}
