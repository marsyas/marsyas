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

/** 
    \class MidiOutput
    \brief Control of MahaDeviBot Commands via Midi Controls.
*/

#include "MidiOutput.h"

using namespace std;
using namespace Marsyas;

MidiOutput::MidiOutput(string name):MarSystem("MidiOutput",name)
{  
#ifdef MRSMIDI
  midiout = NULL;
#endif 
  addControls();
}


MidiOutput::MidiOutput(const MidiOutput& a):MarSystem(a)
{
#ifdef MRSMIDI
  midiout = NULL;
#endif 
  ctrl_byte1_ = getctrl("mrs_natural/byte1");
  ctrl_byte2_ = getctrl("mrs_natural/byte2");
  ctrl_byte3_ = getctrl("mrs_natural/byte3");
  ctrl_sendMessage_ = getctrl("mrs_bool/sendMessage");
}

MidiOutput::~MidiOutput()
{
#ifdef MRSMIDI
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
  setctrlState("mrs_bool/sendMessage", true);
}



void
MidiOutput::myUpdate()
{
  MRSDIAG("MidiOutput.cpp - MidiOutput:myUpdate");
  
  MarSystem::myUpdate();
  

#ifdef MRSMIDI

  if (midiout == NULL) 
    {
      try { 
	midiout = new RtMidiOut();
      } 
      catch (RtError &error) { 
	error.printMessage();
	return;
      }
      
      try { 
	midiout->openPort(0);
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
  

  
  mrs_bool sendMessage = ctrl_sendMessage_->to<mrs_bool>();
  
  if (sendMessage) 
    {
      message[0] = ctrl_byte1_->to<mrs_natural>();
      message[1] = ctrl_byte2_->to<mrs_natural>();
      message[2] = ctrl_byte3_->to<mrs_natural>();
      midiout->sendMessage( &message );
      updctrl(ctrl_sendMessage_, false);
    }

      
#endif
  
  
}

void 
MidiOutput::myProcess(realvec& in, realvec& out)
{
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) =  in(o,t);
      }
}







	
