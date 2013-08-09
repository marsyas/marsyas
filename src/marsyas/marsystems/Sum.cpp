/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Sum.h"

using namespace std;
using namespace Marsyas;

Sum::Sum(mrs_string name):MarSystem("Sum",name)
{
  addControls();
}

Sum::Sum(const Sum& a): MarSystem(a)
{
  ctrl_weight_ = getctrl("mrs_real/weight");
  ctrl_stereo_ = getctrl("mrs_bool/stereo");
  ctrl_mode_ = getctrl("mrs_string/mode");
  setctrlState("mrs_string/mode", true);
}



Sum::~Sum()
{
}

void
Sum::addControls()
{
  addctrl("mrs_real/weight", 1.0, ctrl_weight_);
  addctrl("mrs_bool/stereo", false, ctrl_stereo_);
  addctrl("mrs_string/mode", "orig", ctrl_mode_);
}

MarSystem*
Sum::clone() const
{
  return new Sum(*this);
}

void
Sum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  // // Start with the default MarSystem setup with equal input/output
  // //stream format ...
  // MarSystem::myUpdate(sender);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);



  // get name of first observation and use for sum
  mrs_string inObsName = stringSplit(ctrl_inObsNames_->to<mrs_string>(), ",")[0];
  ctrl_onObsNames_->setValue("Sum_" + inObsName+",", NOUPDATE);


  // sness - Do what the MarSystem did before we refactored it, just in case
  // other people are depending on the old behaviour.  In the future, probably
  // refactor this code out

  if(ctrl_mode_->to<mrs_string>() == "orig") {
    /// ... but set the number of output observations correctly.
    mrs_bool stereo = ctrl_stereo_->to<mrs_bool>();
    if (!stereo) {
      ctrl_onObservations_->setValue(1, NOUPDATE);
    } else {
      ctrl_onObservations_->setValue(2, NOUPDATE);
    }
  } else {

    // sness - New Sum code
    if (ctrl_mode_->to<mrs_string>() == "sum_observations") {
      ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
      ctrl_onSamples_->setValue(1, NOUPDATE);
      ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);

    } else if (ctrl_mode_->to<mrs_string>() == "sum_samples") {
      ctrl_onObservations_->setValue(1, NOUPDATE);
      ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
    } else if (ctrl_mode_->to<mrs_string>() == "sum_whole") {
      ctrl_onObservations_->setValue(1, NOUPDATE);
      ctrl_onSamples_->setValue(1, NOUPDATE);
    }
  }



}


void
Sum::myProcess(realvec& in, realvec& out)
{
  mrs_real weightValue = ctrl_weight_->to<mrs_real>();
  mrs_bool stereo = ctrl_stereo_->to<mrs_bool>();
  mrs_natural t,o,c;

  // sness - Do what the MarSystem did before we refactored it, just in case
  // other people are depending on the old behaviour.  In the future, probably
  // refactor this code out
  if (ctrl_mode_->to<mrs_string>() == "orig")
  {
    // Sum the observation channels per sample.
    if (!stereo)
    {
      for (t = 0; t < inSamples_; t++)
      {
        out(0, t) = 0;
        for (o = 0; o < inObservations_; o++)
        {
          out(0, t) += (weightValue * in(o, t));
        }
      }
    }
    else 			// stereo
    {
      for (t = 0; t < inSamples_; t++)
        for (c=0; c < 2; ++c)
        {
          out(c, t) = 0;
          for (o = c; o < inObservations_; o+=2)
          {
            out(c, t) += (weightValue * in(o, t));
          }
        }
    }
  } else {
    // sness - New Sum code
    for (o = 0; o < onObservations_; o++) {
      for (t = 0; t < onSamples_; t++) {
        out(o,t) = 0.0;
      }
    }
    // AL: why not replace this with out.setval for readability?

    if (ctrl_mode_->to<mrs_string>() == "sum_observations") {
      for (o = 0; o < inObservations_; o++) {
        for (t = 0; t < inSamples_; t++) {
          out(o,0) += in(o,t);
        }
      }
    } else if (ctrl_mode_->to<mrs_string>() == "sum_samples")
    {
      for (o = 0; o < inObservations_; o++) {
        for (t = 0; t < inSamples_; t++) {
          out(0,t) += in(o,t);
        }
      }
    } else if (ctrl_mode_->to<mrs_string>() == "sum_whole") {
      for (o = 0; o < inObservations_; o++) {
        for (t = 0; t < inSamples_; t++) {
          out(0,0) += in(o,t);
        }
      }
    }
  }
}
