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

#include "../common_source.h"
#include "ADRessStereoSpectrum.h"


using std::ostringstream;
using namespace Marsyas;

ADRessStereoSpectrum::ADRessStereoSpectrum(mrs_string name):MarSystem("ADRessStereoSpectrum", name)
{
  addControls();
}

ADRessStereoSpectrum::ADRessStereoSpectrum(const ADRessStereoSpectrum& a) : MarSystem(a)
{

}

ADRessStereoSpectrum::~ADRessStereoSpectrum()
{
}

MarSystem*
ADRessStereoSpectrum::clone() const
{
  return new ADRessStereoSpectrum(*this);
}

void
ADRessStereoSpectrum::addControls()
{
}

void
ADRessStereoSpectrum::myUpdate(MarControlPtr sender)
{
  MRSDIAG("ADRessStereoSpectrum.cpp - ADRessStereoSpectrum:myUpdate");

  (void) sender;  //suppress warning of unused parameter(s)

  N2_ = ctrl_inObservations_->to<mrs_natural>()/2; //i.e. N/2+1

  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_onObservations_->setValue(N2_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  for (mrs_natural n=0; n < N2_; n++)
    oss << "ADRess_stereobin_" << n << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  beta_ = ctrl_inSamples_->to<mrs_natural>()-2;
}

void
ADRessStereoSpectrum::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for(o=0; o < N2_; ++o)
  {
    //look for the maximum in the azimuth plane
    //NOTE: first column of input is dedicated to bin phases...
    maxVal_ = 0.0;
    maxIndex_ = beta_;
    for(t=0; t<=beta_; ++t)
    {
      if(in(o,t+1) > maxVal_)//AZl
      {
        maxVal_ = in(o,t+1);
        maxIndex_ = t;
      }
      if(in(N2_+o, t+1) > maxVal_)//AZr
      {
        maxVal_ = in(N2_+o,t+1);
        maxIndex_ = beta_*2+1-t;
      }
    }

    //avoid two indexes for center panning
    if(maxIndex_ > beta_)
      maxIndex_--;

    //convert panning index to range [-1,1]
    //and save it to corresponding output
    out(o,0) = (mrs_real)maxIndex_/(beta_*2.0)*2.0 - 1.0;
  }

  // MATLAB_PUT(out, "ADRessBinPan");
  // MATLAB_EVAL("figure(3);plot(ADRessBinPan);");
}
