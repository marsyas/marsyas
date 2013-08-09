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
    \class AutoCorrelationFFT
    \brief AutoCorrelation System based on FFT


*/



#include "AutoCorrelationFFT.h"

using std::ostringstream;
using namespace Marsyas;


AutoCorrelationFFT::AutoCorrelationFFT(mrs_string name):MarSystem("AutoCorrelationFFT", name)
{
  //Add any specific controls needed by AutoCorrelationFFT
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  //addControls();
}

AutoCorrelationFFT::AutoCorrelationFFT(const AutoCorrelationFFT& orig) : MarSystem(orig)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work

  //ctrl_offStart_ = getctrl("mrs_real/offStart");
}

AutoCorrelationFFT::~AutoCorrelationFFT()
{
}

MarSystem*
AutoCorrelationFFT::clone() const
{
  return new AutoCorrelationFFT(*this);
}

void
AutoCorrelationFFT::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();

  setControl("mrs_natural/onSamples",  (mrs_natural)(ctrl_inSamples_->to<mrs_natural>()/2));
  setControl("mrs_natural/onObservations",  (mrs_natural)(ctrl_inObservations_->to<mrs_natural>()));

  // Add Names of The Observations to the observation names.
  //inObsNames+="";

  // Add prefix to the observation names.
  //ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "AutoCorrelationFFT_"), NOUPDATE);
}


void
AutoCorrelationFFT::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural o=0; o<inObservations_; o++)
  {

    mrs_natural i;
    temp_.create(inSamples_);
    for (i=0; i < inSamples_; ++i)
      temp_(i) = in(o,i);
    mrs_real *temp = temp_.getData();
    fft_.rfft(temp, inSamples_/2, FFT_FORWARD);
    //temp[0] = 0.0;
    //temp[1] = 0.0;
    out(o,0) = 0.0;				// DC


    // compute magnitude of freqs
    for (i=1; i<onSamples_; ++i)
    {
      out(o,i) = sqrt(temp[2*i]*temp[2*i] + temp[2*i+1]*temp[2*i+1]);
      //cout << "outvec[" << 2*i << "]: " << outvec(2*i) << endl;
    }
  }


}

