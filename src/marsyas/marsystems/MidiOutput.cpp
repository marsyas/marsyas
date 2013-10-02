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

#include "MidiOutput.h"
#include "../common_source.h"

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif


using std::ostringstream;
using namespace Marsyas;

MidiOutput::MidiOutput(mrs_string name):MarSystem("MidiOutput",name)
{
#ifdef MARSYAS_MIDIIO
  midiout = NULL;
#endif
  addControls();
}


MidiOutput::MidiOutput(const MidiOutput& a):MarSystem(a)
{
#ifdef MARSYAS_MIDIIO
  midiout = NULL;
#endif
  ctrl_byte1_ = getctrl("mrs_natural/byte1");
  ctrl_byte2_ = getctrl("mrs_natural/byte2");
  ctrl_byte3_ = getctrl("mrs_natural/byte3");
  ctrl_sendMessage_ = getctrl("mrs_bool/sendMessage");
}

MidiOutput::~MidiOutput()
{
#ifdef MARSYAS_MIDIIO
  delete midiout;
#endif
}


MarSystem*
MidiOutput::clone() const
{
  return new MidiOutput(*this);
}


void
MidiOutput::addControls()
{
  addctrl("mrs_natural/byte1", 64, ctrl_byte1_);
  addctrl("mrs_natural/byte2", 64, ctrl_byte2_);
  addctrl("mrs_natural/byte3", 64, ctrl_byte3_);
  addctrl("mrs_bool/sendMessage", false, ctrl_sendMessage_);

  addctrl("mrs_natural/port", 0);
  addctrl("mrs_bool/virtualPort", false);
  addctrl("mrs_bool/initMidi", false);

  setctrlState("mrs_bool/sendMessage", true);
  setctrlState("mrs_bool/virtualPort", true);
  setctrlState("mrs_natural/port", true);
  setctrlState("mrs_natural/port", true);
}

void
MidiOutput::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MidiOutput.cpp - MidiOutput:myUpdate");
  MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO

  initMidi = getctrl("mrs_bool/initMidi")->to<mrs_bool>();
  virtualPort = getctrl("mrs_bool/virtualPort")->to<mrs_bool>();

  if (initMidi)
  {
    if (midiout == NULL)
    {
      try {
        midiout = new RtMidiOut();
      }
      catch (RtError &error) {
        error.printMessage();
        return;
      }

      if (virtualPort)
      {
        try
        {
          midiout->openVirtualPort("MarsyasOutput");
        }
        catch(RtError &error)
        {
          error.printMessage();
          return;
        }
        message.push_back(0);
        message.push_back(0);
        message.push_back(0);
      }
      else
      {
        {
          try {
            midiout->openPort( getctrl("mrs_natural/port")->to<mrs_natural>() );
          }
          catch (RtError &error)
          {
            error.printMessage();
            return;
          }
          message.push_back(0);
          message.push_back(0);
          message.push_back(0);
        }
      }
    }
    mrs_bool sendMessage = ctrl_sendMessage_->to<mrs_bool>();
    if (sendMessage)
    {
      message[0] = (unsigned char)ctrl_byte1_->to<mrs_natural>();
      message[1] = (unsigned char)ctrl_byte2_->to<mrs_natural>();
      message[2] = (unsigned char)ctrl_byte3_->to<mrs_natural>();

      midiout->sendMessage( &message );
      setctrl(ctrl_sendMessage_, false);
    }
  }
#endif

}

void MidiOutput::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) =  in(o,t);
    }
}
