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

#include "Resample.h"

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

Resample::Resample(mrs_string name):MarSystem("Resample", name)
{
  //Add any specific controls needed by Resample
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)

  interpolator_=new ResampleLinear("resa");
  addControls();
}

Resample::Resample(const Resample& orig) : MarSystem(orig)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work




  //private:
  //  //Add specific controls needed by this MarSystem.
  //    void addControls();
  //    MarControlPtr ctrl_option_;
  //    MarControlPtr ctrl_samplingRateAdjustmentMode_;
  //    MarControlPtr ctrl_offStart_;
  //    MarControlPtr ctrl_offEnd_;
  //    MarControlPtr ctrl_newSamplingRate_;
  //    MarControlPtr ctrl_resamplingMode_;
  //    MarSystem* interpolator_;
  //    //mrs_string resaModeOld_;
  //    void myUpdate(MarControlPtr sender);


  //public:
  //  Resample(std::string name);
  //    Resample(const Resample& a);
  //  ~Resample();
  //  MarSystem* clone() const;
  //  //MarSystem* Resample::getInterpolator() const;

  //  void myProcess(realvec& in, realvec& out);



  ctrl_offStart_ = getctrl("mrs_real/offStart");
  ctrl_offEnd_ = getctrl("mrs_real/offEnd");
  ctrl_option_ = getctrl("mrs_bool/option");
  ctrl_resamplingMode_ = getctrl("mrs_string/resamplingMode");
  ctrl_samplingRateAdjustmentMode_ = getctrl("mrs_bool/samplingRateAdjustmentMode");
  ctrl_newSamplingRate_ = getctrl("mrs_real/newSamplingRate");


  //resaModeOld_=orig.resaModeOld_;


  //interpolator_=orig.getInterpolator();
  interpolator_=orig.interpolator_->clone();
}

Resample::~Resample()
{

  delete interpolator_;
}

MarSystem*
Resample::clone() const
{
  return new Resample(*this);
}


void
Resample::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/offStart", 0.0, ctrl_offStart_);
  addctrl("mrs_real/offEnd", 0.0, ctrl_offEnd_);
  addctrl("mrs_bool/samplingRateAdjustmentMode", (mrs_bool)true , ctrl_samplingRateAdjustmentMode_);
  addctrl("mrs_string/resamplingMode", "linear" , ctrl_resamplingMode_);
  addctrl("mrs_bool/option", (mrs_bool)false , ctrl_option_);
  addctrl("mrs_real/newSamplingRate", 22050.0 , ctrl_newSamplingRate_);
  
  setctrlState("mrs_bool/samplingRateAdjustmentMode",(mrs_bool)true);
  setctrlState("mrs_real/newSamplingRate",(mrs_bool)true);
  setctrlState("mrs_string/resamplingMode",(mrs_bool)true);
}

void
Resample::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);


  mrs_real alpha = ctrl_newSamplingRate_->to<mrs_real>()/ctrl_israte_->to<mrs_real>();

  ctrl_onSamples_->setValue((mrs_natural) (alpha * ctrl_inSamples_->to<mrs_natural>()), NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>());
  if (!(ctrl_samplingRateAdjustmentMode_->to<mrs_bool>()))
  {
    alpha=1.0;
  }

  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>()*alpha);

  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  // Add prefix to the observation names. -> will be done by submarsystems
  //ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Resample_"), NOUPDATE);


  mrs_string resaMode=ctrl_resamplingMode_->to<mrs_string>();
  //if (resaMode!=resaModeOld_)
  //{
  //    resaModeOld_=resaMode;


  delete interpolator_;
  interpolator_ = NULL;
  if (resaMode==(mrs_string)"sincip")
    {
      interpolator_= new ResampleSinc("resa");
      interpolator_->updControl("mrs_real/offStart", ctrl_offStart_->to<mrs_real>());
      interpolator_->updControl("mrs_real/offEnd", ctrl_offEnd_->to<mrs_real>());

      interpolator_->updControl("mrs_bool/windowedMode", ctrl_option_->to<mrs_bool>());
    }
  else if (resaMode==(mrs_string)       "bezier")
    {
      interpolator_= new ResampleBezier("resa");
      interpolator_->updControl("mrs_real/offStart", ctrl_offStart_->to<mrs_real>());
      interpolator_->updControl("mrs_real/offEnd", ctrl_offEnd_->to<mrs_real>());

      interpolator_->updControl("mrs_bool/tangentMode", ctrl_option_);
    }
  else if (resaMode==(mrs_string)       "near")
    {
      interpolator_= new ResampleNearestNeighbour("resa");
    }
  else
    {
      interpolator_= new ResampleLinear("resa");
    }
  
  if (interpolator_ != NULL)
    {
      interpolator_->updControl("mrs_bool/samplingRateAdjustmentMode", ctrl_samplingRateAdjustmentMode_->to<mrs_bool>());
      interpolator_->updControl("mrs_real/stretch", ctrl_newSamplingRate_->to<mrs_real>()/ctrl_israte_->to<mrs_real>());
      interpolator_->updControl("mrs_natural/inSamples", inSamples_);
      interpolator_->updControl("mrs_natural/inObservations", inObservations_);
    }
}


void
Resample::myProcess(realvec& in, realvec& out)
{
  interpolator_->process(in,out);
}
