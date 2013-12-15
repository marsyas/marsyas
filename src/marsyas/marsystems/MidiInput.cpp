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


#include "MidiInput.h"
#include "../common_source.h"

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif

#include <cstddef>

using std::ostringstream;
using std::size_t;
using namespace Marsyas;

MidiInput::MidiInput(mrs_string name):MarSystem("MidiInput",name)
{
  initMidi = false;
#ifdef MARSYAS_MIDIIO
  midiin = NULL;
#endif
  addControls();
}

MidiInput::MidiInput(const MidiInput& a): MarSystem(a)
{
  ctrl_byte1_ = getctrl("mrs_natural/byte1");
  ctrl_byte2_ = getctrl("mrs_natural/byte2");
  ctrl_byte3_ = getctrl("mrs_natural/byte3");
}

MidiInput::~MidiInput()
{
#ifdef MARSYAS_MIDIIO
  delete midiin;
#endif
}

MarSystem* MidiInput::clone() const
{
  return new MidiInput(*this);
}

void MidiInput::addControls()
{
  addctrl("mrs_natural/port", 0);
  addctrl("mrs_bool/virtualPort", false);
  addctrl("mrs_bool/initmidi", false);
  setctrlState("mrs_bool/initmidi", true);
  addctrl("mrs_natural/byte1", 0, ctrl_byte1_);
  addctrl("mrs_natural/byte2", 0, ctrl_byte2_);
  addctrl("mrs_natural/byte3", 0, ctrl_byte3_);
}

void MidiInput::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MidiInput.cpp - MidiInput:myUpdate");
  MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO
  midiin = NULL;

  initMidi= getctrl("mrs_bool/initmidi")->to<mrs_bool>();
  initMidi = !initMidi;

  virtualPort = getctrl("mrs_bool/virtualPort")->to<mrs_bool>();

  if (!initMidi) {
    try {
      midiin = new RtMidiIn();
    }
    catch (RtError &error) {
      error.printMessage();
      return;
    }
    midiin->setCallback(&MidiInput::mycallback, this);
    midiin->ignoreTypes(false, false, false);
    setctrl("mrs_bool/initmidi", false);

    if (virtualPort)
    {
      try {
        midiin->openVirtualPort("MarsyasInput");
      }
      catch (RtError &error)
      {
        error.printMessage();
        return;
      }
    }
    else
    {
      try {
        midiin->openPort(getctrl("mrs_natural/port")->to<mrs_natural>());
      }
      catch (RtError &error)
      {
        error.printMessage();
        return;
      }
    }
  }

#endif
}

void MidiInput::mycallback(double deltatime, std::vector< unsigned char > * message, void *userData)
{
  // FIXME Unused parameter
  (void) deltatime;
  std::size_t nBytes = 0;
  nBytes = message->size();

  MidiInput* mythis = (MidiInput*) userData;
  std::vector<int> v(3);

  if (nBytes > 0)
  {
    if (nBytes > 2)
    {
	  v[0] = message->at(0);
	  v[1] = message->at(1);
	  v[2] = message->at(2);
	  mythis->msgQueue.push(v);
    }
  }
}

void MidiInput::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  std::vector<int> *v;
  // just pass data through
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) =  in(o,t);
    }
  }
  if (!msgQueue.empty()) {
	v = new std::vector<int>(msgQueue.front());
	msgQueue.pop();
	ctrl_byte1_->setValue((mrs_natural)v->at(0), NOUPDATE);
	ctrl_byte2_->setValue((mrs_natural)v->at(1), NOUPDATE);
	ctrl_byte3_->setValue((mrs_natural)v->at(2), NOUPDATE);
	delete v;
  }
}
