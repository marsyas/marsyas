/*
** Copyright (C) 1998-2017 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include "Histogram.h"

using std::ostringstream;
using namespace Marsyas;

Histogram::Histogram(mrs_string name) : MarSystem("Histogram", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

Histogram::Histogram(const Histogram& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_gain_ = getctrl("mrs_real/gain");
  ctrl_normalize_ = getctrl("mrs_bool/normalize");
  ctrl_reset_ = getctrl("mrs_bool/reset");
  ctrl_histoSize_ = getctrl("mrs_natural/histoSize");
}


Histogram::~Histogram()
{
}

MarSystem*
Histogram::clone() const
{
  return new Histogram(*this);
}

void
Histogram::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addctrl("mrs_real/gain", 1.0, ctrl_gain_);
  addctrl("mrs_bool/normalize", false, ctrl_normalize_);
  addctrl("mrs_bool/reset", false, ctrl_reset_);
  addctrl("mrs_natural/histoSize", 100, ctrl_histoSize_);
}

void
Histogram::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  
  MRSDIAG("Histogram.cpp - Histogram:myUpdate");
  ctrl_onSamples_->setValue(ctrl_histoSize_->to<mrs_natural>(),  NOUPDATE);
  // ctrl_onSamples_->setValue(ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>(), NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>());
  
}

void
Histogram::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  float amplitude;
  mrs_natural bin;
  float increment;
  
  const mrs_real& gainValue = ctrl_gain_->to<mrs_real>();
  const mrs_bool& normalize = ctrl_normalize_->to<mrs_bool>();
  const mrs_bool& reset = ctrl_reset_->to<mrs_bool>();
  
  increment = 1.0 / onSamples_;
  /// Iterate over the observations and samples and do the processing.

  if (reset)
    {
      out.setval(0.0);
      ctrl_reset_->setValue(false);
    }

  // do not accumulate 
  out.setval(0.0);
  
  for (o = 0; o < inObservations_; o++)
  {
    // for (t = 0; t < histoSize; t++)
    for (t = 0; t < inSamples_; t++)
    {
      amplitude = in(0,t);
      amplitude += 1.0; // bring to 0-2.0 range
      amplitude *= 0.5; // bring to 0-1.0 range
      amplitude *= onSamples_; // scale to histogram range 
      bin = (mrs_natural)amplitude;
      out(o, bin) = out(0,bin) + increment;
    }
  }

  if (normalize)
    {

      /* for (o = 0; o < inObservations_; o++)      
	for (t = 0; t < onSamples_/2; t++)
	  {
	    if (out(o, t+onSamples_/2) != 0.0)
		out(o, t) = out(o,t) / out(o, t+onSamples_/2);
	    else
	      out(o, t) = 0.0;
	  }

      for (o = 0; o < inObservations_; o++)      
	for (t = onSamples_/2; t < onSamples_; t++)
	  {
	    out(o, t) = 0.0;
	  }
      */ 

      // out.setval(0.0);
      out .normMaxMin();
      ctrl_normalize_->setValue(false);
    }
}
