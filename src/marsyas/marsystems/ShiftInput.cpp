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

#include "ShiftInput.h"

using namespace std;
using namespace Marsyas;

ShiftInput::ShiftInput(mrs_string name): MarSystem("ShiftInput", name)
{
  winSize_ = 0;
  hopSize_ = 0;
  addControls();
}

ShiftInput::~ShiftInput()
{
}

ShiftInput::ShiftInput(const ShiftInput& a): MarSystem(a)
{
  winSize_ = 0;
  hopSize_ = 0;

  ctrl_reset_ = getctrl("mrs_bool/reset");
  ctrl_winSize_ = getctrl("mrs_natural/winSize");

  ctrl_clean_ = getctrl("mrs_bool/clean");
  ctrl_lowCleanLimit_ = getctrl("mrs_real/lowCleanLimit");
  ctrl_highCleanLimit_ = getctrl("mrs_real/highCleanLimit");
}

MarSystem*
ShiftInput::clone() const
{
  return new ShiftInput(*this);
}

void
ShiftInput::addControls()
{
  // The control for the window size.
  addControl("mrs_natural/winSize", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES, ctrl_winSize_);
  setControlState("mrs_natural/winSize", true);

  // The control to reset the internal buffer, set to true so the buffer
  // itself is properly initialized later.
  addControl("mrs_bool/reset", true, ctrl_reset_);

  // Clean control for partially cleaning the buffer at any point of the analysis
  // The low and high clean limits (as a portion of the buffer size) are given by
  // lowCleanLimit and highCleanLimit controls
  addControl("mrs_bool/clean", false, ctrl_clean_);
  addControl("mrs_real/lowCleanLimit", 0.0, ctrl_lowCleanLimit_);
  addControl("mrs_real/highCleanLimit", 1.0, ctrl_highCleanLimit_);
}

void
ShiftInput::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  // Cache the window and hop size for use in myProcess().
  winSize_ = ctrl_winSize_->to<mrs_natural>();
  hopSize_ = ctrl_inSamples_->to<mrs_natural>();

  // If the hop size is smaller than the window size, we need buffering to
  // handle the overlap between successive windows.
  // When the hop size is larger than the window size, we don't have overlap
  // and don't need buffering.
  if (hopSize_ < winSize_)
  {
    outSavedData_.stretch(ctrl_inObservations_->to<mrs_natural>(), winSize_ - hopSize_);
    if (hopSize_ == 0) {
      addToStabilizingDelay_ = 0;
    } else {
      mrs_natural full = (mrs_natural) (((mrs_real) winSize_ / hopSize_));
      addToStabilizingDelay_ = ((mrs_natural) ceil(((mrs_real) winSize_ / hopSize_)-full)) + full - 1;
      //cout<<winSize_ <<"\t"<<hopSize_ <<"\t"<< (mrs_real) winSize_ / hopSize_<<"\t"<<full<<"\t"<<addToStabilizingDelay_<<endl;
    }
  } else {
    addToStabilizingDelay_ = 0;
  }

  // Update the output stream format.
  ctrl_onSamples_->setValue(ctrl_winSize_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // Update the onObsNames string.
  ostringstream prefix_oss;
  prefix_oss << "HopSize" << hopSize_ << "_WinSize" << winSize_ << "_";
  mrs_string onObsNames = obsNamesAddPrefix(ctrl_inObsNames_->to<mrs_string>(), prefix_oss.str());
  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);
}

void
ShiftInput::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;


  for (o = 0; o<inObservations_; ++o)
  {
    if (hopSize_ < winSize_)
    {
      // When the hop size is smaller than the window size, we have to work
      // with a buffer.

      // Check if we must clear the audio buffer (due to a reset call).
      // TODO: shouldn't this be done in myUpdate?
      if (ctrl_reset_->to<mrs_bool>())
      {
        outSavedData_.setval(0.0);
        ctrl_reset_->setValue(false);
      }

      // Check if requested a call for a partial clean of the buffer
      // TODO: shouldn't this be done in myUpdate?
      if(ctrl_clean_->to<mrs_bool>())
      {
        // round up with ceil()
        mrs_natural lowCleanLimit = (mrs_natural) ceil(winSize_
                                    * getctrl("mrs_real/lowCleanLimit")->to<mrs_real>());
        mrs_natural highCleanLimit = (mrs_natural) (lowCleanLimit + (ceil( winSize_
                                     * getctrl("mrs_real/highCleanLimit")->to<mrs_real>()
                                                                         ) - lowCleanLimit));

        for (t = lowCleanLimit; t < highCleanLimit; t++)
          outSavedData_(o, t) = 0.0;

        ctrl_clean_->setValue(false);
      }

      // TODO: this can be done more efficiently with a circular buffer
      // instead of shifting the buffer data all the time.

      // First part of output: copy the buffered data.
      for (t = 0; t < winSize_ - hopSize_; t++)
      {
        out(o, t) = outSavedData_(o, t);
      }
      // Second part of output: copy the new data from the input.
      for (t = winSize_ - hopSize_; t < winSize_; t++)
      {
        out(o, t) = in(o, t - (winSize_ - hopSize_));
      }
      // Store part of the output to the buffer for the next time.
      for (t = 0; t < winSize_ - hopSize_; t++)
      {
        outSavedData_(o, t) = out(o, t + hopSize_);
      }
    }
    else
    {
      // When the hop size is larger than the window size, we just need to
      // copy the first onSamples_ samples and discard the rest.
      for (t = 0; t < onSamples_; ++t)
      {
        out(o, t) = in(o, t);
      }
    }
  }

  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
  // MATLAB_PUT(in, "ShiftInput_in");
  // MATLAB_PUT(out, "ShiftInput_out");
  // MATLAB_EVAL("size(ShiftInput_in)");

// 	MATLAB_EVAL("plot(ShiftInput_in)");
//	getchar();
//  MATLAB_EVAL("plot(ShiftInput_out)");
//	getchar();

}
