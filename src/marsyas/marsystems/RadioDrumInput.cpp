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


#include "RadioDrumInput.h"
#include "../common_source.h"


#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif

#include <cstddef>

using namespace std;
using namespace Marsyas;

RadioDrumInput::RadioDrumInput(mrs_string name):MarSystem("RadioDrumInput",name)
{
  initMidi = false;
#ifdef MARSYAS_MIDIIO
  rdin = NULL;
#endif
  addControls();
}

RadioDrumInput::RadioDrumInput(const RadioDrumInput& a): MarSystem(a)
{
  rx_ = getctrl("mrs_natural/rightstickx");
  ry_ = getctrl("mrs_natural/rightsticky");
  rz_ = getctrl("mrs_natural/rightstickz");
  lx_ = getctrl("mrs_natural/leftstickx");
  ly_ = getctrl("mrs_natural/leftsticky");
  lz_ = getctrl("mrs_natural/leftstickz");
}

RadioDrumInput::~RadioDrumInput()
{
#ifdef MARSYAS_MIDIIO
  delete rdin;
#endif
}

MarSystem* RadioDrumInput::clone() const
{
  return new RadioDrumInput(*this);
}

void RadioDrumInput::addControls()
{
  addctrl("mrs_natural/port", 0);
  addctrl("mrs_bool/initmidi", false);
  setctrlState("mrs_bool/initmidi", true);
  addctrl("mrs_natural/rightstickx", 0, rx_);
  addctrl("mrs_natural/rightsticky", 0, ry_);
  addctrl("mrs_natural/rightstickz", 0, rz_);
  addctrl("mrs_natural/leftstickx", 0, lx_);
  addctrl("mrs_natural/leftsticky", 0, ly_);
  addctrl("mrs_natural/leftstickz", 0, lz_);
}

void RadioDrumInput::myUpdate(MarControlPtr sender)
{
  MRSDIAG("RadioDrumInput.cpp - RadioDrumInput:myUpdate");
  MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO
  rdin = NULL;

  initMidi= getctrl("mrs_bool/initmidi")->to<mrs_bool>();

  if (!initMidi) {
    try {
      rdin = new RtMidiIn();
    }
    catch (RtError &error) {
      error.printMessage();
      return;
    }
    rdin->setCallback(&RadioDrumInput::mycallback, this);
    rdin->ignoreTypes(false, false, false);
    setctrl("mrs_bool/initmidi", false);

    try {
      rdin->openPort(getctrl("mrs_natural/port")->to<mrs_natural>());
    }
    catch (RtError &error)
    {
      error.printMessage();
      return;
    }
    initMidi = !initMidi;
  }
#endif
}

/*
    From the Radio Drum Manual
-----------------------------------------

                                             MIDI COMMAND (THREE BYTES)
                                             BYTE1 BYTE2 BYTE3**
    trigger from stick 1 & whack strength       A0    1    WHACK
                          & X1                  A0   15       X1
                          & Y1                  A0   16       Y1
    trigger from stick 2 & whack strength       A0    2    WHACK
                          & X2                  A0   17       X2
                          & Y2                  A0   18       Y2
    trigger from B15+ button                    A0    3        1
    down trigger from B14- foot switch          A0    3        2
    up trigger from B14- foot switch            A0    3        3
    down trigger from B15- foot switch          A0    3        4
    up trigger from B15- foot switch            A0    3        5
    pot 1 current value                         A0    4     POT1
    pot 2 current value                         A0    5     POT2
    pot 3 current value                         A0    6     POT3
    pot 4 current value                         A0    7     POT4
    stick 1 x current position                  A0    8       X1
    stick 1 y current position                  A0    9       Y1
    stick 1 z current position                  A0   10       Z1
    stick 2 x current position                  A0   11       X2
    stick 2 y current position                  A0   12       Y2
    stick 2 z current position                  A0   13       Z2

All data in BYTE3 is encoded in the standard midi range 0-127

*/
void RadioDrumInput::mycallback(double deltatime, std::vector< unsigned char > * message, void *userData)
{
  // FIXME Unused parameter
  (void) deltatime;
  size_t nBytes = 0;
  nBytes = message->size();

  RadioDrumInput* mythis = (RadioDrumInput*) userData;

  // fix this to make more sense with specific radio drum input
  if (nBytes ==  3)
  {

    // 160 is to detect poly pressure on channel 1 0xA0
    if ( message->at(0) == 160) {
      if(message->at(1) == 8)
        mythis->rightstickx = message->at(2);
      else  if ( message->at(1)==9)
        mythis->rightsticky = message->at(2);
      else  if ( message->at(1)==10)
        mythis->rightstickz = message->at(2);
      else  if ( message->at(1)==11)
        mythis->leftstickx = message->at(2);
      else  if ( message->at(1)==12)
        mythis->leftsticky = message->at(2);
      else  if ( message->at(1)==13)
        mythis->leftstickz = message->at(2);
    }

  }
}

void RadioDrumInput::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // just pass data through
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) =  in(o,t);
    }

  rx_->setValue((mrs_natural)rightstickx, NOUPDATE);
  ry_->setValue((mrs_natural)rightsticky, NOUPDATE);
  rz_->setValue((mrs_natural)rightstickz, NOUPDATE);
  lx_->setValue((mrs_natural)leftstickx, NOUPDATE);
  ly_->setValue((mrs_natural)leftsticky, NOUPDATE);
  lz_->setValue((mrs_natural)leftstickz, NOUPDATE);

}
