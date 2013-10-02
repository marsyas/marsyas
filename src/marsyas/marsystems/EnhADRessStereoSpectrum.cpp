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

#include "EnhADRessStereoSpectrum.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

EnhADRessStereoSpectrum::EnhADRessStereoSpectrum(mrs_string name):MarSystem("EnhADRessStereoSpectrum", name)
{
  addControls();
}

EnhADRessStereoSpectrum::EnhADRessStereoSpectrum(const EnhADRessStereoSpectrum& a) : MarSystem(a)
{

}

EnhADRessStereoSpectrum::~EnhADRessStereoSpectrum()
{
}

MarSystem*
EnhADRessStereoSpectrum::clone() const
{
  return new EnhADRessStereoSpectrum(*this);
}

void
EnhADRessStereoSpectrum::addControls()
{

}

void
EnhADRessStereoSpectrum::myUpdate(MarControlPtr sender)
{
  MRSDIAG("EnhADRessStereoSpectrum.cpp - EnhADRessStereoSpectrum:myUpdate");
  (void) sender;  //suppress warning of unused parameter(s)

  N2_ = ctrl_inObservations_->to<mrs_natural>()/3; //i.e. N/2+1

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(N2_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  for (mrs_natural n=0; n < N2_; n++)
    oss << "enhADRess_stereobin_" << n << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
EnhADRessStereoSpectrum::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for(t=0; t < inSamples_; ++t)
  {
    for(o=0; o<N2_; ++o)
    {
      out(o,t) = in(N2_*2+o,t);
    }
  }
  //MATLAB_PUT(out, "out2");
  //MATLAB_EVAL("figure(3);plot(out2)");
}
