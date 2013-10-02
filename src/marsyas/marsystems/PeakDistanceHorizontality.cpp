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

#include "../common_source.h"
#include "PeakDistanceHorizontality.h"

using std::ostringstream;
using std::abs;

using namespace Marsyas;

//#define MTLB_DBG_LOG


static const mrs_real kIdentityThresh	= 1e-6;
static const mrs_real kSteepness		= .8;
static const mrs_real kCutoff			= 1.;	// in Bark

PeakDistanceHorizontality::PeakDistanceHorizontality(mrs_string name) : MarSystem("PeakDistanceHorizontality", name)
{
  /// Add any specific controls needed by this MarSystem.
  // Default controls that all MarSystems should have (like "inSamples"
  // and "onObservations"), are already added by MarSystem::addControl(),
  // which is already called by the constructor MarSystem::MarSystem(name).
  // If no specific controls are needed by a MarSystem there is no need to
  // implement and call this addControl() method (see for e.g. Rms.cpp)
  addControls();
}

PeakDistanceHorizontality::PeakDistanceHorizontality(const PeakDistanceHorizontality& a) : MarSystem(a)
{
  ctrl_horizvert_	= getctrl("mrs_realvec/inpIsHorizontal");
  ctrl_rangeX_	= getctrl("mrs_real/rangeX");
  ctrl_rangeY_	= getctrl("mrs_real/rangeY");
}


PeakDistanceHorizontality::~PeakDistanceHorizontality()
{
}

MarSystem*
PeakDistanceHorizontality::clone() const
{
  // Every MarSystem should do this.
  return new PeakDistanceHorizontality(*this);
}

void
PeakDistanceHorizontality::addControls()
{
  mrs_realvec tmp(1);
  tmp(0)	= 0;

  addctrl("mrs_bool/bypass", false);
  addctrl("mrs_realvec/weights", tmp);
  addctrl("mrs_natural/numInputs", 0);
  addctrl("mrs_realvec/inpIsHorizontal", tmp, ctrl_horizvert_);
  addctrl("mrs_real/rangeX", 0., ctrl_rangeX_);
  addctrl("mrs_real/rangeY", 0., ctrl_rangeY_);
}

void
PeakDistanceHorizontality::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PeakDistanceHorizontality.cpp - PeakDistanceHorizontality:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  weights_.stretch(inSamples_*getctrl ("mrs_natural/numInputs")->to<mrs_natural>(), inSamples_);

  sigSteepness_	= kSteepness;
  sigCutOff_		= kCutoff;
}

void
PeakDistanceHorizontality::myProcess(realvec& in, realvec& out)
{
  mrs_natural i;
  const mrs_natural	numInputs	= getctrl ("mrs_natural/numInputs")->to<mrs_natural>();
  const mrs_realvec	isHoriz		= ctrl_horizvert_->to<mrs_realvec>();
  const mrs_real		range[2]	= {ctrl_rangeX_->to<mrs_real>(), ctrl_rangeY_->to<mrs_real>()};

  out = in;

  MRSASSERT(range[0] > 0 && range[1] > 0);
  if (isHoriz.getSize () != numInputs)
  {
    MRSWARN("PeakDistanceHorizontality: dimension mismatch");
    MRSASSERT(false);
    out.setval(0);
    return;
  }

  if (getctrl("mrs_bool/bypass")->to<mrs_bool>())
  {
    weights_.setval(1.);
    setctrl ("mrs_realvec/weights", weights_);
    return;
  }

  for (i = 0; i < inSamples_; i++)
  {
    for (mrs_natural j = i; j < inSamples_; j++)
    {
      mrs_natural k;
      mrs_real	horizontality	= ComputeHorizontality (	std::abs(in(1,i)-in(1,j))/range[0],
                                std::abs(in(0,i)-in(0,j))/range[1]),
                                norm			= 0;

      for (k = 0; k < numInputs; k++)
      {
        mrs_real weight = horizontality;

        if (abs(isHoriz(k) - 2) < kIdentityThresh)
          weight	= .5;			// input is both horizontal and vertical
        else if (abs(isHoriz(k)) < kIdentityThresh)
          weight	= 1.-weight;	// input is vertical

        norm							+= weight;
        weights_(k*inSamples_ + i, j)	= weight;
        weights_(k*inSamples_ + j, i)	= weight;	// symmetry
      }
      if (norm != 0)
        norm	= 1./norm;
      for (k = 0; k < numInputs; k++)
      {
        weights_(k*inSamples_ + i, j)	*= norm;
        if (i != j)
          weights_(k*inSamples_ + j, i)	*= norm;	// symmetry
      }
    }
  }
  setctrl ("mrs_realvec/weights", weights_);
#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(weights_, "weights");
  MATLAB_EVAL("figure(1);imagesc(weights),colorbar;");
#endif
#endif
}

mrs_real
PeakDistanceHorizontality::ComputeHorizontality(mrs_real scaledDiffX, mrs_real scaledDiffY)
{

  if (scaledDiffX == 0)
  {
    if (scaledDiffY == 0)
      return .5;
    else
      return 0.;
  }
  else if (scaledDiffY == 0)
    return   1.;

  mrs_real res = scaledDiffX / std::sqrt(scaledDiffX*scaledDiffX + scaledDiffY*scaledDiffY);

  return res*res;
}
