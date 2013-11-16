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

#include "../common_source.h"
#include "Esitar.h"

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif


using std::ostringstream;
using namespace Marsyas;

Esitar::Esitar(mrs_string name):MarSystem("Esitar",name)
{
  //type_ = "Esitar";
  //name_ = name;

#ifdef MARSYAS_MIDIIO
  midiin = NULL;
#endif
}


Esitar::~Esitar()
{
#ifdef MARSYAS_MIDIIO
  delete midiin;
#endif
}


MarSystem*
Esitar::clone() const
{
  return new Esitar(*this);
}

void
Esitar::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Esitar.cpp - Esitar:myUpdate");

// 	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
//   setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
  MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO
  try {
    midiin = new RtMidiIn();
  }
  catch (RtError &error) {
    error.printMessage();
    return;
  }

  try {
    midiin->openPort(0);
  }
  catch (RtError &error)
  {
    error.printMessage();
    return;
  }
  midiin->setCallback(&Esitar::mycallback, this);
  midiin->ignoreTypes(false, false, false);
#endif
}


void
Esitar::mycallback(double deltatime, std::vector< unsigned char > * message, void *userData)
{
  // FIXME Unused parameter
  (void) deltatime;
  size_t nBytes = 0;
  nBytes = message->size();

  Esitar* mythis = (Esitar*) userData;


  if (nBytes > 0)
  {
    if (nBytes > 2)
    {
      mythis->byte3 = message->at(2);
      mythis->byte2 = message->at(1);
      mythis->type = message->at(0);
    }

    // thumb
    if ((mythis->type == 176)&&(mythis->byte2 == 1))
    {
      mythis->thumb = mythis->byte3;
    }

    // fret
    if ((mythis->type == 176)&&(mythis->byte2 == 2))
    {
      mythis->fret = mythis->byte3;
    }

    // headx
    if ((mythis->type == 176)&&(mythis->byte2 == 6))
    {
      mythis->headx = mythis->byte3;
    }

    // heady
    if ((mythis->type == 176)&&(mythis->byte2 == 8))
    {
      mythis->heady = mythis->byte3;
    }

    // headz
    if ((mythis->type == 176)&&(mythis->byte2 == 10))
    {
      mythis->headz = mythis->byte3;
    }

    // pot1
    if ((mythis->type == 176)&&(mythis->byte2 == 7))
    {
      mythis->pot1 = mythis->byte3;
    }

    // pot2
    if ((mythis->type == 176)&&(mythis->byte2 == 9))
    {
      mythis->pot2 = mythis->byte3;
    }

    // switch1
    if ((mythis->type == 144)&&(mythis->byte2 == 1))
    {
      mythis->switch1 = mythis->byte3;
    }

    // switch2
    if ((mythis->type == 144)&&(mythis->byte2 == 2))
    {
      mythis->switch2 = mythis->byte3;
    }

    // switch3
    if ((mythis->type == 144)&&(mythis->byte2 == 3))
    {
      mythis->switch3 = mythis->byte3;
    }

    // switch4
    if ((mythis->type == 144)&&(mythis->byte2 == 4))
    {
      mythis->switch4 = mythis->byte3;
    }

    // switch5
    if ((mythis->type == 144)&&(mythis->byte2 == 5))
    {
      mythis->switch5 = mythis->byte3;
    }

    // switch6
    if ((mythis->type == 144)&&(mythis->byte2 == 6))
    {
      mythis->switch6 = mythis->byte3;
    }

    // switch7
    if ((mythis->type == 144)&&(mythis->byte2 == 7))
    {
      mythis->switch7 = mythis->byte3;
    }

    // switch8
    if ((mythis->type == 144)&&(mythis->byte2 == 8))
    {
      mythis->switch8 = mythis->byte3;
    }

    // switch9
    if ((mythis->type == 144)&&(mythis->byte2 == 9))
    {
      mythis->switch9 = mythis->byte3;
    }

    // switch10
    if ((mythis->type == 144)&&(mythis->byte2 == 10))
    {
      mythis->switch10 = mythis->byte3;
    }

  }
}

void
Esitar::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  //checkFlow(in,out);

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) =  in(o,t);
    }
}








