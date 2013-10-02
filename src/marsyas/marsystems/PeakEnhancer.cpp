/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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
    \class PeakEnhancer
    \brief Enhance peaks that are harmonic

    Enhance harmonic peaks resulting from autocorrelation periodicity
detection by applying half wave rectification and then subtracting the
upsampled fvec from the original fvec.
*/


#include "PeakEnhancer.h"
#include "HalfWaveRectifier.h"
#include "Filter.h"
#include <marsyas/system/MarSystem.h>

using std::ostringstream;
using namespace Marsyas;


PeakEnhancer::PeakEnhancer(mrs_string name):MarSystem("PeakEnhancer", name)
{
  //Add any specific controls needed by PeakEnhancer
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
  rect_=NULL;
  lowpass_=NULL;
}

PeakEnhancer::PeakEnhancer(const PeakEnhancer& orig) : MarSystem(orig)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work

  ctrl_itnum_ = getctrl("mrs_natural/itnum");
  if (orig.rect_!=NULL) rect_=orig.rect_->clone();
  else rect_=NULL;
  if (orig.lowpass_!=NULL) lowpass_=orig.lowpass_->clone();
  else lowpass_=NULL;
}

PeakEnhancer::~PeakEnhancer()
{
  if (rect_!=NULL) delete rect_;
  if (lowpass_!=NULL) delete lowpass_;
}

MarSystem*
PeakEnhancer::clone() const
{
  return new PeakEnhancer(*this);
}

void
PeakEnhancer::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/itnum", (mrs_natural)1, ctrl_itnum_);
}

void
PeakEnhancer::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();

  setControl("mrs_natural/onSamples",  (mrs_natural)(ctrl_inSamples_->to<mrs_natural>()));
  setControl("mrs_natural/onObservations",  (mrs_natural)(ctrl_inObservations_->to<mrs_natural>()));

  // Add Names of The Observations to the observation names.
  //inObsNames+="";

  // Add prefix to the observation names.
  //ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "PeakEnhancer_"), NOUPDATE);

  mrs_realvec num_low;
  num_low.create(3);
  mrs_realvec denom_low;
  denom_low.create(3);
  num_low(0)=0.1207f; num_low(1)=0.2415f; num_low(2)=0.1207f;
  denom_low(0)=1.0f; denom_low(1)=-0.8058f; denom_low(2)=0.2888f;

  if (rect_==NULL ) rect_=new HalfWaveRectifier("hwr");
  if (lowpass_==NULL) lowpass_=new Filter("lpf");

  lowpass_->setctrl("mrs_realvec/ncoeffs", num_low);
  lowpass_->setctrl("mrs_realvec/dcoeffs", denom_low);

  rect_->setControl("mrs_natural/inSamples",ctrl_inSamples_);
  rect_->setControl("mrs_natural/inObservations",ctrl_inObservations_);

  rect_->setControl("mrs_natural/onSamples",ctrl_onSamples_);
  rect_->setControl("mrs_natural/onObservations",ctrl_onObservations_);

  lowpass_->setControl("mrs_natural/inSamples",ctrl_inSamples_);
  lowpass_->setControl("mrs_natural/inObservations",ctrl_inObservations_);

  lowpass_->setControl("mrs_natural/onSamples",ctrl_onSamples_);
  lowpass_->setControl("mrs_natural/onObservations",ctrl_onObservations_);


}


void
PeakEnhancer::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural o=0; o<inObservations_; o++)
  {

    mrs_realvec tempVec;
    tempVec.create(inSamples_);
    mrs_realvec& temp=tempVec;
    mrs_realvec temp2Vec;
    temp2Vec.create(inSamples_);
    mrs_realvec& temp2=temp2Vec;
    mrs_realvec temp3Vec;
    temp3Vec.create(inSamples_);
    mrs_realvec& temp3=temp3Vec;

    //half wave rectify
    rect_->process(in, temp);

    temp2.setval(0.0);

    //timestretching via resampling using upsampling and lowpass filteing (discarding (ctrl_itnum_-1)/ctrl_itnum_ of the data )
    for (mrs_natural i=0; i<inSamples_/ctrl_itnum_->to<mrs_natural>(); ++i)
      temp2(ctrl_itnum_->to<mrs_natural>()*i)=temp(i);

    lowpass_->process(temp2, temp3);

    for(mrs_natural i=0; i<inSamples_; ++i)
    {
      out(i)=in(o,i)-temp3(i);
    }
  }


}
