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

#include "ResampleSinc.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

/** Marsystem for resampling the audio Samplingrate
*
*
*
*
*
*
*/

ResampleSinc::ResampleSinc(mrs_string name):MarSystem("ResampleSinc", name)
{
  //Add any specific controls needed by ResampleSinc
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

ResampleSinc::ResampleSinc(const ResampleSinc& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work


//Controls:
  //mrs_bool ctrl_windowedMode	-	false:	apply sinc interpolation directly
  //								true:	(default) use values of Kaiser Window and interpolate linearly between them if necessary
  //mrs_bool ctrl_samplingRateAdjustmentMode - adjust new resulting SamplingRate for following Marsystems
  //mrs_real stretch - desired stretch ratio (number of output samples = input number of samples*stretch)
  //mrs_real offStart - (default:0) offset from the start (towards the end) of the Samples (if only a part of the samples should be used to interpolate)
  //mrs_real offEnd - (default:0) offset from the end (towards the start) of the Samples (if only a part of the samples should be used to interpolate)


  ctrl_offStart_ = getctrl("mrs_real/offStart");
  ctrl_offEnd_ = getctrl("mrs_real/offEnd");
  ctrl_windowedMode_ = getctrl("mrs_bool/windowedMode");
  ctrl_samplingRateAdjustmentMode_ = getctrl("mrs_bool/samplingRateAdjustmentMode");
  ctrl_stretch_ = getctrl("mrs_real/stretch");

}

ResampleSinc::~ResampleSinc()
{

}

MarSystem*
ResampleSinc::clone() const
{
  return new ResampleSinc(*this);
}

void
ResampleSinc::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/offStart", 0.0, ctrl_offStart_);
  addctrl("mrs_real/offEnd", 0.0, ctrl_offEnd_);
  addctrl("mrs_bool/samplingRateAdjustmentMode", (mrs_bool)true , ctrl_samplingRateAdjustmentMode_);
  addctrl("mrs_bool/windowedMode", (mrs_bool)false , ctrl_windowedMode_);
  addctrl("mrs_real/stretch", 1.0 , ctrl_stretch_);
  setctrlState("mrs_real/stretch", true);
  setctrlState("mrs_bool/samplingRateAdjustmentMode",(mrs_bool)true);

}

void
ResampleSinc::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);


  mrs_real alpha = ctrl_stretch_->to<mrs_real>();

  mrs_natural onSamples = (mrs_natural) (alpha * ctrl_inSamples_->to<mrs_natural>());

  ctrl_onSamples_->setValue(onSamples, NOUPDATE);

  arrx_.allocate(onSamples);

  //cout << "updating: " << ctrl_inSamples_->to<mrs_natural>() << endl;
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>());
  if (!(ctrl_samplingRateAdjustmentMode_->to<mrs_bool>()))
  {
    alpha=1.0;
  }

  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>()*alpha);
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  // Add prefix to the observation names.
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "ResampleSinc_"), NOUPDATE);

}

mrs_real
ResampleSinc::sinc(mrs_real t)
{
  mrs_real ret=0.0;
  if (t==0)
    ret=1.0;
  else
    ret=sin(t*PI)/(t*PI);
  return ret;
}

mrs_real
ResampleSinc::window(mrs_real position)
{
  mrs_realvec y;
  y.create(11);

//    Kaiser Window beta=2, length 11
  y(0)=1;
  y(1)=1.393725584134;
  y(2)=1.749980639738;
  y(3)=2.033757714070;
  y(4)=2.21650903826;
  y(5)=2.279585302336;

//    Kaiser Window beta=1, length 13
//    y(0)=1;
//    y(1)=1.077860145694;
//    y(2)=1.143786491151;
//    y(3)=1.196474329913;
//    y(4)=1.234877004454;
//    y(5)=1.258229528804;
//    y(6)=1.266065877752;

//    Kaiser Window beta=2, length 13
//    y(0)=1.0;
//    y(1)=1.3297043679778717;
//    y(2)=1.6376481570588971;
//    y(3)=1.9029098945382872;
//    y(4)=2.1070523256224973;
//    y(5)=2.2356659845728475;
//    y(6)=2.2795853023360673;


  mrs_real divisor=2.279585302336;

  for (mrs_natural i=0; i<y.getSize(); ++i)

    if (i<=y.getSize()/2)
      y(i)=y(i)/divisor;
    else
      y(i)=y(y.getSize()-i-1);

  if ((position <-5) | (position > 5))
    return 0;

  mrs_natural index=0;
  while (index+1<5+position)
  {
    index=index+1;
  }
  mrs_real difference=5+position-index;
  return (1-difference)*y(index)+difference*y(index+1);
}


void
ResampleSinc::myProcess(realvec& in, realvec& out)
{
  mrs_bool windowedMode = ctrl_windowedMode_->to<mrs_bool>();
  mrs_real offStart=ctrl_offStart_->to<mrs_real>();
  mrs_real offEnd=ctrl_offEnd_->to<mrs_real>();
  mrs_real ratio=(inSamples_-1-offStart-offEnd)/(mrs_real)(onSamples_-1);

  for(mrs_natural i=0; i<onSamples_; ++i)
    arrx_(i)=offStart+i*ratio;

  mrs_natural winlength = 5; //maximum windowlength is also enforced by the window function

  for (mrs_natural o=0; o<inObservations_; o++)
  {
    for (int i=0; i<onSamples_; ++i)
    {
      mrs_real sample = 0;
      for (mrs_natural ansinks=0; ansinks<inSamples_; ansinks++)
      {
        mrs_real sincIndex = arrx_(i)-ansinks;
        if (abs(sincIndex)<winlength)
        {
          if (windowedMode)
          {
            sample += in(o,ansinks)*sinc(sincIndex)*window(sincIndex);
          }
          else
          {
            sample += in(o,ansinks)*sinc(sincIndex);
          }
        }
      }
      out(o,i) = sample;
    }
  }
}
