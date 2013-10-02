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


#include "Envelope.h"
#include "../common_source.h"


using namespace Marsyas;
using std::ostringstream;


Envelope::Envelope(mrs_string name):MarSystem("Envelope", name)
{
  addControls();
}


Envelope::~Envelope()
{
}


MarSystem*
Envelope::clone() const
{
  return new Envelope(*this);
}

void
Envelope::addControls()
{
  addctrl("mrs_real/target", 0.0);
  //addctrl("mrs_real/rate", 0.001);
  addctrl("mrs_real/time",0.2);
  addctrl("natural/state",0);
  addctrl("mrs_real/nton", 0.0);
  addctrl("mrs_real/ntoff", 0.0);
  setctrlState("mrs_real/target", true);
  //setctrlState("mrs_real/rate", true);
  setctrlState("mrs_real/time", true);
  setctrlState("mrs_real/nton", true);
  setctrlState("mrs_real/ntoff", true);
}


void
Envelope::update()
{
  MRSDIAG("Envelope.cpp - Envelope:update");
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("string/onObsNames", getctrl("string/inObsNames"));

  sampleRate_= getctrl("mrs_real/israte")->to<mrs_real>();
  target_ = getctrl("mrs_real/target")->to<mrs_real>();
  //rate_ = getctrl("mrs_real/rate")->to<mrs_real>();
  time_ = getctrl("mrs_real/time")->to<mrs_real>();

  rate_ = 1.0 / (time_ * sampleRate_);


  noteon_ = getctrl("mrs_real/nton")->to<mrs_real>();
  noteoff_ = getctrl("mrs_real/ntoff")->to<mrs_real>();

  if(noteon_) {
    value_=0.0;
    this->updControl("mrs_real/nton",0.0);
    this->updControl("mrs_real/target",1.0);
    state_ = 1;
  }

  if(noteoff_) {

    this->updControl("mrs_real/ntoff",0.0);
    this->updControl("mrs_real/target",0.0);
    state_ = 1;
  }

}


void
Envelope::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  //checkFlow(in,out);

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {

      if (state_==1) {
        if (target_ > value_) {
          value_ =value_+ rate_;
          if (value_ >= target_) {
            value_ = target_;
            state_ = 0;
          }
        }//target
        else {

          value_ = value_-rate_;
          if (value_ <= target_) {
            value_ = target_;
            state_ = 0;
          }
        }//else
      }//state

      out(o,t) =  value_* in(o,t);

    }//for





}
