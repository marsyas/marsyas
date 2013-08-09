/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "Color.h"

using namespace Marsyas;

Color::Color()
{
  red_ = 0.5;
  green_ = 0.5;
  blue_ = 0.5;
}

Color::~Color()
{
}

Color::Color(float r, float g, float b)
{
  red_ = r;
  green_ = g;
  blue_ = b;
}


void
Color::setRGB(float r, float g, float b)
{
  red_ = r;
  green_ = g;
  blue_ = b;
}


void
Color::setR()
{
  red_ = 1.0;
  green_ = 0.0;
  blue_ = 0.0;
}


void
Color::setG()
{
  red_ = 0.0;
  green_ = 1.0;
  blue_ = 0.0;
}

void
Color::setB()
{
  red_ = 0.0;
  green_ = 0.0;
  blue_ = 1.0;
}


float
Color::getR()
{
  return red_;
}


float
Color::getG()
{
  return green_;
}

float
Color::getB()
{
  return blue_;
}
