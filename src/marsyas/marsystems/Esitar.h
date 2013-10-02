/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_ESITAR_H
#define MARSYAS_ESITAR_H

#include <marsyas/system/MarSystem.h>

class RtMidiIn;


namespace Marsyas
{
/**
    \class Esitar
    \ingroup MIDI
    \brief Update midi inpput using controls. Just through for dataflow.
*/


class marsyas_EXPORT Esitar: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  static void mycallback(double deltatime, std::vector< unsigned char> * message, void *userData);


  RtMidiIn* midiin;
  int byte3, byte2, type;

public:
  Esitar(std::string name);
  ~Esitar();
  MarSystem* clone() const;

  int rval;

  // Sitar Sensors
  int thumb;
  int fret;

  // Accelerometer Variables
  int headx;
  int heady;
  int headz;

  // Potentiometers
  int pot1;
  int pot2;

  // Buttons
  int switch1;
  int switch2;
  int switch3;
  int switch4;
  int switch5;
  int switch6;
  int switch7;
  int switch8;
  int switch9;
  int switch10;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
