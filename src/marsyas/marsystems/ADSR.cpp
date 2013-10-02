/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "ADSR.h"
#include "../common_source.h"


using namespace Marsyas;


ADSR::ADSR(mrs_string name):MarSystem("ADSR", name)
{
  addControls();
}

ADSR::~ADSR()
{
}

MarSystem*
ADSR::clone() const
{
  return new ADSR(*this);
}

void
ADSR::addControls()
{
  //addctrl("mrs_real/aRate",  0.001); //attack rate
  addctrl("mrs_real/aTime",    0.2);   //attack time
  addctrl("mrs_real/aTarget",  1.0);   //attack target
  //addctrl("mrs_real/dRate",  0.001); //decay rate
  addctrl("mrs_real/dTime",    0.1);   //decay time
  addctrl("mrs_real/susLevel", 0.85);  //sustain level
  //addctrl("mrs_real/rRate",  0.001); //release rate
  addctrl("mrs_real/rTime",    0.2);   //release time

  // Lee's Adjustments
  addctrl("mrs_real/eValue", 0.0 );     // envelope value
  addctrl("mrs_bool/bypass", false);

  addctrl("mrs_natural/state", 1);

  addctrl("mrs_real/nton",  0.0);
  addctrl("mrs_real/ntoff", 0.0);

  addctrl("mrs_bool/noteon",  false);
  addctrl("mrs_bool/noteoff", false);

  //setctrlState("mrs_real/aRate",  true);
  setctrlState("mrs_real/aTime",    true);
  setctrlState("mrs_real/aTarget",  true);
  //setctrlState("mrs_real/dRate",  true);
  setctrlState("mrs_real/dTime",    true);
  setctrlState("mrs_real/susLevel", true);
  //setctrlState("mrs_real/rRate",  true);
  setctrlState("mrs_real/rTime",    true);
  setctrlState("mrs_real/nton",     true);
  setctrlState("mrs_real/ntoff",    true);
  setctrlState("mrs_real/eValue",   true);
  setctrlState("mrs_bool/bypass",   true);
  setctrlState("mrs_bool/noteon",   true);
  setctrlState("mrs_bool/noteoff",  true);
}

void
ADSR::myUpdate(MarControlPtr sender)
{
  MRSDIAG("ADSR.cpp - ADSR:myUpdate");

  //setctrl("natural/onSamples", getctrl("natural/inSamples"));
  //setctrl("natural/onObservations", getctrl("natural/inObservations"));
  //setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  //setctrl("string/onObsNames", getctrl("string/inObsNames"));
  MarSystem::myUpdate(sender);

  //aRate_ = getctrl("mrs_real/aRate")->to<mrs_real>();
  aTime_ = getctrl("mrs_real/aTime")->to<mrs_real>();
  aTarget_ = getctrl("mrs_real/aTarget")->to<mrs_real>();
  //dRate_ = getctrl("mrs_real/dRate")->to<mrs_real>();
  dTime_ = getctrl("mrs_real/dTime")->to<mrs_real>();
  susLevel_ = getctrl("mrs_real/susLevel")->to<mrs_real>();
  //rRate_ = getctrl("mrs_real/rRate")->to<mrs_real>();
  rTime_ = getctrl("mrs_real/rTime")->to<mrs_real>();

  sampleRate_= getctrl("mrs_real/israte")->to<mrs_real>();

  aRate_ = 1.0 / (aTime_ * sampleRate_);
  dRate_ = 1.0 / (dTime_ * sampleRate_);
  rRate_ = 1.0 / (rTime_ * sampleRate_);
  nton_ = getctrl("mrs_real/nton")->to<mrs_real>();
  ntoff_ = getctrl("mrs_real/ntoff")->to<mrs_real>();

  noteon_ = getctrl("mrs_bool/noteon")->to<mrs_bool>();
  noteoff_ = getctrl("mrs_bool/noteoff")->to<mrs_bool>();

  bypass_ = getctrl("mrs_bool/bypass")->to<mrs_bool>();


  if(noteon_ || nton_)
  {
    this->setctrl("mrs_real/nton",0.0);
    this->setctrl("mrs_bool/noteon", false);
    value_=0.0;
    target_ = aTarget_;
    state_ = 1;
  }

  if(noteoff_ || ntoff_)
  {
    this->setctrl("mrs_real/ntoff",0.0);
    this->setctrl("mrs_bool/noteoff", false);
    target_ = 0.0;
    state_ = 4;
  }
}

void
ADSR::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      switch (state_)
      {
      case 1://attack
        value_ += aRate_;
        if (value_ >= target_)
        {
          value_ = target_;
          rate_ = dRate_;
          target_ = susLevel_;
          state_ = 2;
        }
        break;
      case 2://decay
        value_ -= dRate_;
        if (value_ <= susLevel_)
        {
          value_ = susLevel_;
          rate_ = 0.0;
          state_ = 3;
        }
        break;
      case 4://release
        value_ -= rRate_;
        if (value_ <= 0.0)
        {
          value_ = 0.0;
          state_ = 5;//done
        }
      }//switch

      if ( !bypass_ )
      {
        out(o,t) =  value_* in(o,t);
      }
      else
      {
        out(o,t) =  value_;
      }
    }//for
  }//for

  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
  //MATLAB_PUT(out, "ADSR_out");
  //MATLAB_EVAL("onsetAudio = [onsetAudio, ADSR_out];");
  //MATLAB_EVAL("toy_with_onsets");

}
