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

#include "PitchDiff.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

PitchDiff::PitchDiff(mrs_string name) : MarSystem("PitchDiff", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

PitchDiff::PitchDiff(const PitchDiff& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_expectedPitch_ = getctrl("mrs_real/expectedPitch");
  ctrl_ignoreOctaves_ = getctrl("mrs_bool/ignoreOctaves");
  ctrl_absoluteValue_ = getctrl("mrs_bool/absoluteValue");
}


PitchDiff::~PitchDiff()
{
}

MarSystem*
PitchDiff::clone() const
{
  return new PitchDiff(*this);
}

void
PitchDiff::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addctrl("mrs_real/expectedPitch", 440.0, ctrl_expectedPitch_);
  addctrl("mrs_bool/ignoreOctaves", false, ctrl_ignoreOctaves_);
  addctrl("mrs_bool/absoluteValue", false, ctrl_absoluteValue_);
  setctrlState("mrs_real/expectedPitch", true);
}

void
PitchDiff::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PitchDiff.cpp - PitchDiff:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "PitchDiff_"),
                             NOUPDATE);

  expectedMidiPitch_ = hertz2pitch(ctrl_expectedPitch_->to<mrs_real>());
}

void
PitchDiff::myProcess(realvec& in, realvec& out)
{
  /// Iterate over the observations and samples and do the processing.
  for (mrs_natural o = 0; o < inObservations_; o++)
  {
    // we should only have one sample input

    // convert to midi
    mrs_real in_midi = hertz2pitch(in(o,0));
    // difference in midi
    mrs_real diff = in_midi - expectedMidiPitch_;
    if (ctrl_ignoreOctaves_->isTrue()) {
      diff = fmod(diff, 12.0);
      // seeing -1.5 is much more intuitive than
      // seeing 9.5.
      if (diff > 6) {
        diff -= 12.0;
      }
      if (diff < -6) {
        diff += 12.0;
      }
    }
    if (ctrl_absoluteValue_->isTrue()) {
      diff = fabs(diff);
    }
    out(o, 0) = diff;
  }
}
