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
    \class Envelope
    \brief Multiply input realvec by Value of Envelope

   Simple MarSystem example. Just multiply the values of the input realvec
with current value of the ADSR envelope and put them in the output vector.

*/



#include "ADSR.h"

using namespace Marsyas;
using namespace std;


ADSR::ADSR(string name):MarSystem("ADSR", name)
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
  //addctrl("mrs_real/aRate", 0.001);//attack rate 
  addctrl("mrs_real/aTime", 0.2);//attack time 
  addctrl("mrs_real/aTarget", 1.0);//attack target 
  //addctrl("mrs_real/dRate", 0.001);//decay rate 
  addctrl("mrs_real/dTime", 0.1);//decay time
  addctrl("mrs_real/susLevel", 0.85);//sustain level 
  //addctrl("mrs_real/rRate", 0.001);//release rate
  addctrl("mrs_real/rTime", 0.2);//release time
  addctrl("natural/state", 1);
  addctrl("mrs_real/nton", 0.0);
  addctrl("mrs_real/ntoff", 0.0);
  //setctrlState("mrs_real/aRate", true);
  setctrlState("mrs_real/aTime", true);
  setctrlState("mrs_real/aTarget", true);
  //setctrlState("mrs_real/dRate", true);
  setctrlState("mrs_real/dTime", true);
  setctrlState("mrs_real/susLevel", true);
  //setctrlState("mrs_real/rRate", true);
  setctrlState("mrs_real/rTime", true);
  setctrlState("mrs_real/nton", true);
  setctrlState("mrs_real/ntoff", true);
} 


void
ADSR::update()
{
  MRSDIAG("ADSR.cpp - ADSR:update");
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")); 
  setctrl("string/onObsNames", getctrl("string/inObsNames"));

  //aRate_ = getctrl("mrs_real/aRate")->toReal();
  aTime_ = getctrl("mrs_real/aTime")->toReal();
  aTarget_ = getctrl("mrs_real/aTarget")->toReal();
  //dRate_ = getctrl("mrs_real/dRate")->toReal();
  dTime_ = getctrl("mrs_real/dTime")->toReal();
  susLevel_ = getctrl("mrs_real/susLevel")->toReal();
  //rRate_ = getctrl("mrs_real/rRate")->toReal();
  rTime_ = getctrl("mrs_real/rTime")->toReal();

  sampleRate_= getctrl("mrs_real/israte")->toReal();

  aRate_ = 1.0 / (aTime_ * sampleRate_);
  dRate_ = 1.0 / (dTime_ * sampleRate_);
  rRate_ = 1.0 / (rTime_ * sampleRate_);
  //cout << "attack decay release rates:"<< aRate_ <<" "<< dRate_<<" "<< rRate_<< endl;
 
  //cout << "attack decay release times:"<< aTime_ <<" "<< dTime_<<" "<< rTime_<< endl;
  noteon_ = getctrl("mrs_real/nton")->toReal();
  noteoff_ = getctrl("mrs_real/ntoff")->toReal();

  if(noteon_){
    
    this->updctrl("mrs_real/nton",0.0);
    value_=0.0;
    target_ = aTarget_;
    state_ = 1;  
  }

  if(noteoff_){
   
    this->updctrl("mrs_real/ntoff",0.0);
    cout << "noteof ADSR" << endl;
    target_ = 0.0;
    state_ = 4;
  }

}


void 
ADSR::myProcess(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {

	switch (state_) {

	case 1://attack
	  value_ += aRate_;
	  if (value_ >= target_) {
	    value_ = target_;
	    rate_ = dRate_;
	    target_ = susLevel_;
	    state_ = 2;
	  }
	  break;

	case 2://decay
	  value_ -= dRate_;
	  if (value_ <= susLevel_) {
	    value_ = susLevel_;
	    rate_ = 0.0;
	    state_ = 3;
	  }
	  break;

	case 4://release
	  value_ -= rRate_;
	  if (value_ <= 0.0)       {
	    value_ = 0.0;
	    state_ = 5;//done
	  }
	}//switch

	out(o,t) =  value_* in(o,t);
	//cout <<"output=" << out(o,t)<< endl;
	//cout <<"val=" << value_<< endl;

      }//for


  

  
}//process

