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

#include "Flux.h"
#include "../common_source.h"

using namespace std;
using std::abs;

using namespace Marsyas;

Flux::Flux(mrs_string name):MarSystem("Flux",name)
{
  addControls();

  diff_ = 0.0;
  flux_ = 0.0;
  max_ = 0.0;
  addToStabilizingDelay_ = 1;
}

Flux::Flux(const Flux& a) : MarSystem(a)
{
  ctrl_reset_ = getctrl("mrs_bool/reset");
  ctrl_mode_ = getctrl("mrs_string/mode");
  addToStabilizingDelay_ = 1;
}

Flux::~Flux()
{
}

MarSystem*
Flux::clone() const
{
  return new Flux(*this);
}

void
Flux::addControls()
{
  addctrl("mrs_string/mode", "marsyas", ctrl_mode_);
  // set this true initially to clear prevWindow_
  addctrl("mrs_bool/reset", true, ctrl_reset_);
  setctrlState("mrs_bool/reset", true);
}

void
Flux::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Flux.cpp - Flux:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  if (ctrl_mode_->to<mrs_string>() == "multichannel") {
    ctrl_onObservations_->setValue(inObservations_, NOUPDATE);
    // need this to match
    prevWindow_.create(ctrl_onObservations_->to<mrs_natural>(),
                       ctrl_onSamples_->to<mrs_natural>());
  } else {
    ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
    prevWindow_.create(ctrl_inObservations_->to<mrs_natural>(),
                       ctrl_inSamples_->to<mrs_natural>());
  }
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("Flux_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  reset_ = ctrl_reset_->to<mrs_bool>();

}

void
Flux::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_string mode = ctrl_mode_->to<mrs_string>();

  if (reset_) {
    prevWindow_.setval(0.0);
    // don't set this control to be false!
    // this is almost always linked to by another
    // control, so let that one do the false'ing!
    reset_ = false;
  }

  for (t = 0; t < inSamples_; t++)
  {
    if(mode == "marsyas")
    {
      flux_ = 0.0;
      diff_ = 0.0;
      max_ = 0.0;
      for(o = 1; o < inObservations_; ++o)
      {
        logtmp_ = log(in(o,t) + MINREAL);
        diff_ = pow((mrs_real)logtmp_  - prevWindow_(o,t), (mrs_real)2.0);
        if(diff_ > max_)
          max_ = diff_;
        flux_ += diff_;

        prevWindow_(o,t) = logtmp_;
      }

      if(max_ != 0.0)
        flux_ /= (max_ * inObservations_);
      else
        flux_ = 0.0;

      out(0,t) = flux_;
    }
    else if (mode == "Laroche2003")
    {
      flux_ = 0.0;

      for (o=1; o < inObservations_; o++)
      {
        mrs_real tmp;
        tmp = in(o,t) - prevWindow_(o,t);
        if (tmp >= 0)
          // flux_ += in(o,t);
          flux_ += tmp;
        prevWindow_(o,t) = in(o,t);
      }

      out(0,t) = flux_;
    }
    else if (mode == "multichannel")
    {
#if 0
      cout<<"in"<<endl;
      for(o = 0 ; o < inObservations_; o++) {
        cout<<in(o,t)<<" ";
      }
      cout<<endl;
#endif
      for(o = 0 ; o < inObservations_; o++) {
        mrs_real tmp = in(o,t) - prevWindow_(o,t);
        prevWindow_(o,t) = in(o,t);
        if (tmp < 0) {
          tmp = 0;
        }
        out(o, t) = tmp;
      }
#if 0
      cout<<"out"<<endl;
      for(o = 0 ; o < inObservations_; o++) {
        cout<<out(o,t)<<" ";
      }
      cout<<endl;
#endif
    }

    else if(mode=="DixonDAFX06")
    {
      flux_ = 0.0;
      //diff_ = 0.0;
      //max_ = 0.0;

      for(o = 1 ; o < inObservations_; o++)
      {

        //Simon's version
        mrs_real tmp = in(o,t)  - prevWindow_(o,t);

        diff_ = (tmp+abs(tmp))/2;

        //lmartins version
        //diff_ = in(o,t)*in(o,t) - prevWindow_(o,t)*prevWindow_(o,t);
        //diff_ = (diff_+abs(diff_))/2.0;

        //if(diff_ > max_)
        //	max_ = diff_;

        flux_ += diff_;
        prevWindow_(o,t) = in(o,t);

        //from Juan's Matlab
        //			mrs_real tmp = pow(in(o,t), 2.0)  - pow(prevWindow_(o,t), 2.0);
        //			mrs_real tmp2 = (tmp+abs(tmp))/2;
        //			diff_ = sqrt(tmp2);
        //			if(diff_ > max_)
        //				max_ = diff_;
        //			flux_ += diff_;
        //			prevWindow_(o,t) = in(o,t);
      }

      //Normalizing with max_ was a bad idea
      // 		if(max_ != 0.0)
      // 			flux_ /= (max_ * inObservations_);
      // 		else
      // 			flux_ = 0.0;
      //
      out(0,t) = flux_;

    }
  }

  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
  //MATLAB_PUT(out, "Flux_out");
  //MATLAB_EVAL("FluxTS = [FluxTS, Flux_out];");
  //MATLAB_EVAL("plot(FluxTS)");
}
