/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "StereoSpectrumFeatures.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

StereoSpectrumFeatures::StereoSpectrumFeatures(mrs_string name):MarSystem("StereoSpectrumFeatures", name)
{
  m0_ = 0.0;
  m1_ = 0.0;
}

StereoSpectrumFeatures::~StereoSpectrumFeatures()
{
}

MarSystem*
StereoSpectrumFeatures::clone() const
{
  return new StereoSpectrumFeatures(*this);
}

void
StereoSpectrumFeatures::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  MRSDIAG("StereoSpectrumFeatures.cpp - StereoSpectrumFeatures:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);

  // CHANGE THIS TO THE RIGHT NUMBER OF FEATURES
  ctrl_onObservations_->setValue((mrs_natural)4, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  oss << "StereoSpectrumFeatures_Ptotal,";
  oss << "StereoSpectrumFeatures_Plow,";
  oss << "StereoSpectrumFeatures_Pmedium,";
  oss << "StereoSpectrumFeatures_Phigh,";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  audioBW_ = ctrl_israte_->to<mrs_real>() * ctrl_inObservations_->to<mrs_natural>();

  // FIXME These variables are defined but unused.
  // mrs_real lowBounday = 250.0; // Hz
  // mrs_real highBounday = 2800.0; // Hz

  low_ = (mrs_natural)(250.0 / ctrl_israte_->to<mrs_real>());
  high_ = (mrs_natural)(2800.0 / ctrl_israte_->to<mrs_real>());
}

void
StereoSpectrumFeatures::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (t = 0; t < inSamples_; t++)
  {
    m0_ = 0.0;
    for (o=0; o < inObservations_; o++)
    {
      m0_ += (in(o,t) * in(o,t));
    }
    if (m0_ != 0.0)
      out(0,t) =  sqrt(m0_ / inObservations_);
    else
      out(0,t) = 0.0;
  }

  // low band panning RMS 0Hz-250Hz
  for (t = 0; t < inSamples_; t++)
  {
    m0_ = 0.0;
    for (o=0; o < low_; o++)
    {
      m0_ += (in(o,t) * in(o,t));
    }
    if (m0_ != 0.0)
      out(1,t) =  sqrt(m0_ / low_);
    else
      out(1,t) = 0.0;
  }

  // middle band panning RMS 250Hz-2800Hz
  for (t = 0; t < inSamples_; t++)
  {
    m0_ = 0.0;
    for (o=low_; o < high_; o++)
    {
      m0_ += (in(o,t) * in(o,t));
    }
    if (m0_ != 0.0)
      out(2,t) =  sqrt(m0_ / (high_-low_));
    else
      out(2,t) = 0.0;
  }

  for (t = 0; t < inSamples_; t++)
  {
    m0_ = 0.0;
    for (o=high_; o < inObservations_; o++)
    {
      m0_ += (in(o,t) * in(o,t));
    }
    if (m0_ != 0.0)
      out(3,t) =  sqrt(m0_ / (inObservations_-high_));
    else
      out(3,t) = 0.0;
  }

}
