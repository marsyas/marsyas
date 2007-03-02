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
    \class DeviBot
    \brief Control of MahaDeviBot Commands via Midi Controls.
*/

#include "DeviBot.h"

using namespace std;
using namespace Marsyas;

DeviBot::DeviBot(string name):MarSystem("DeviBot",name)
{  
#ifdef MARSYAS_MIDIIO
  midiout = NULL;
#endif 
  addControls();
}


DeviBot::DeviBot(const DeviBot& a):MarSystem(a)
{
#ifdef MARSYAS_MIDIIO
  midiout = NULL;
#endif 
  ctrl_arm_ = getctrl("mrs_string/arm");
  ctrl_velocity_ = getctrl("mrs_natural/velocity");
  ctrl_strike_ = getctrl("mrs_bool/strike");
}

DeviBot::~DeviBot()
{
#ifdef MARSYAS_MIDIIO
  delete midiout;
#endif
}


MarSystem* 
DeviBot::clone() const 
{
  return new DeviBot(*this);
}


void 
DeviBot::addControls()
{
  addctrl("mrs_string/arm", "Ga", ctrl_arm_);
  addctrl("mrs_natural/velocity", 64, ctrl_velocity_);
  addctrl("mrs_bool/strike", false, ctrl_strike_);
  setctrlState("mrs_bool/strike", true);
}



void
DeviBot::myUpdate(MarControlPtr sender)
{
  MRSDIAG("DeviBot.cpp - Esitar:myUpdate");
  
MarSystem::myUpdate(sender);

#ifdef MARSYAS_MIDIIO

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
      message.push_back(1);
      message.push_back(24);
      message.push_back(24);
    }
  

  
  mrs_bool strike = ctrl_strike_->to<mrs_bool>();
  
  if (strike) 
    {
      string arm = ctrl_arm_->to<string>();
      mrs_natural velocity = ctrl_velocity_->to<mrs_natural>();      

      if (arm == "Ga") 
	{
	  message[0] = 144;
	  message[1] = 1;
	  message[2] = velocity;      
	}
      else if (arm == "Na") 
	{
	  message[0] = 144;
	  message[1] = 2;
	  message[2] = velocity;      	  
	}
      else 
	{
	  cout << "Unsupported arm" << endl;
	  return;
	}
      midiout->sendMessage( &message );
      
      updctrl(ctrl_strike_, false);
    }
      

  

#endif
  
  
}

void 
DeviBot::myProcess(realvec& in, realvec& out)
{
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) =  in(o,t);
      }
}







	
