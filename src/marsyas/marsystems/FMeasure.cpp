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

#include "FMeasure.h"

using std::ostringstream;
using namespace Marsyas;

FMeasure::FMeasure(mrs_string name):MarSystem("FMeasure", name)
{
  addControls();
  numObsInRef_			= 0;
  numObsInTest_			= 0;
  numTruePos_				= 0;
}

FMeasure::FMeasure(const FMeasure& a) : MarSystem(a)
{
  ctrl_numObsInRef_		= getControl("mrs_natural/numObservationsInReference");
  ctrl_numObsInTest_		= getControl("mrs_natural/numObservationsInTest");
  ctrl_numTruePos_		= getControl("mrs_natural/numTruePositives");
  ctrl_reset_				= getControl("mrs_bool/reset");

  numObsInRef_			= 0;
  numObsInTest_			= 0;
  numTruePos_				= 0;
}

FMeasure::~FMeasure()
{
}

MarSystem*
FMeasure::clone() const
{
  return new FMeasure(*this);
}

void
FMeasure::addControls()
{
  //Add specific controls needed by this MarSystem.
  addControl("mrs_natural/numObservationsInReference", -1, ctrl_numObsInRef_);
  addControl("mrs_natural/numObservationsInTest", -1, ctrl_numObsInTest_);
  addControl("mrs_natural/numTruePositives", -1, ctrl_numTruePos_);
  addControl("mrs_bool/reset", true, ctrl_reset_);
}

void
FMeasure::myUpdate(MarControlPtr sender)
{
  // no need to do anything FMeasure-specific in myUpdate
  MarSystem::myUpdate(sender);

  updControl ("mrs_natural/onSamples", 1);
  updControl ("mrs_natural/onObservations", 3);
}


void
FMeasure::myProcess(realvec& in, realvec& out)
{
  (void) in;

  if (ctrl_reset_->to<mrs_bool>())
  {
    numObsInRef_	= 0;
    numObsInTest_	= 0;
    numTruePos_		= 0;
    updControl ("mrs_bool/reset", false, false);
  }
  numObsInRef_	+= ctrl_numObsInRef_->to<mrs_natural> ();
  numObsInTest_	+= ctrl_numObsInTest_->to<mrs_natural> ();
  numTruePos_		+= ctrl_numTruePos_->to<mrs_natural> ();

  out.setval(0.);

  MRSASSERT(ctrl_numObsInRef_->to<mrs_natural> () > 0 || ctrl_numObsInTest_->to<mrs_natural> () >= 0 || ctrl_numTruePos_->to<mrs_natural> () >= 0);
  MRSASSERT(ctrl_numTruePos_->to<mrs_natural> () <= ctrl_numObsInRef_->to<mrs_natural> () && ctrl_numTruePos_->to<mrs_natural> () <= ctrl_numObsInTest_->to<mrs_natural> ());

  if (numObsInTest_ == 0)
    return;

  out(kPrecision,0)	= numTruePos_ * (1.0/ numObsInTest_),
  out(kRecall,0)		= numTruePos_ * (1.0/ numObsInRef_);

  if (out(kPrecision,0) <= 0 && out(kRecall,0) <= 0)
    return;

  out(kFMeasure,0)	= 2 * out(kPrecision,0) * out(kRecall,0) / (out(kPrecision,0) + out(kRecall,0));
}








