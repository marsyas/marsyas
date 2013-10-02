/*
 ** Copyright (C) 2010 Stefaan Lippens
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

#include "RunningAutocorrelation.h"
#include "../common_source.h"

// For min().
#include <algorithm>

using namespace std;
using namespace Marsyas;

RunningAutocorrelation::RunningAutocorrelation(mrs_string name) :
  MarSystem("RunningAutocorrelation", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

RunningAutocorrelation::RunningAutocorrelation(const RunningAutocorrelation& a) :
  MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_maxLag_ = getctrl("mrs_natural/maxLag");
  ctrl_normalize_ = getctrl("mrs_bool/normalize");
  ctrl_doNotNormalizeForLag0_ = getctrl("mrs_bool/doNotNormalizeForLag0");
  ctrl_clear_ = getctrl("mrs_bool/clear");
  ctrl_unfoldToObservations_ = getctrl("mrs_bool/unfoldToObservations");
}

RunningAutocorrelation::~RunningAutocorrelation()
{
}

MarSystem*
RunningAutocorrelation::clone() const
{
  return new RunningAutocorrelation(*this);
}

void RunningAutocorrelation::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addctrl("mrs_natural/maxLag", 15, ctrl_maxLag_);
  setctrlState("mrs_natural/maxLag", true);
  addctrl("mrs_bool/normalize", false, ctrl_normalize_);
  setctrlState("mrs_bool/normalize", true);
  addctrl("mrs_bool/doNotNormalizeForLag0", false,
          ctrl_doNotNormalizeForLag0_);
  setctrlState("mrs_bool/doNotNormalizeForLag0", true);
  addctrl("mrs_bool/clear", false, ctrl_clear_);
  setctrlState("mrs_bool/clear", true);
  addctrl("mrs_bool/unfoldToObservations", false, ctrl_unfoldToObservations_);
  setctrlState("mrs_bool/unfoldToObservations", true);
}

/**
 * Helper function to add autocorrelation unfolding prefixes to the observation names.
 */
mrs_string prefixObservationNamesWithAutocorrelationUnfoldingPrefixes_(
  mrs_string inObservationNames, mrs_bool normalize,
  mrs_bool doNotNormalizeForLag0, mrs_natural maxLag)
{
  vector<mrs_string> inObsNames = obsNamesSplit(inObservationNames);
  mrs_string onObsNames("");
  for (vector<mrs_string>::iterator it = inObsNames.begin(); it
       != inObsNames.end(); it++)
  {
    for (int lag = 0; lag <= maxLag; lag++)
    {
      ostringstream oss;
      if (normalize && !(doNotNormalizeForLag0 && lag == 0))
      {
        oss << "Normalized";
      }
      oss << "Autocorr" << lag << "_" << (*it) << ",";
      onObsNames += oss.str();
    }

  }
  return onObsNames;

}

void RunningAutocorrelation::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("RunningAutocorrelation.cpp - RunningAutocorrelation:myUpdate");

  // Update the cache of the control values.
  maxLag_ = ctrl_maxLag_->to<mrs_natural> ();
  if (maxLag_ < 0)
  {
    MRSERR("maxLag should be greater than zero.");
    // Setting it to zero to be sure.
    maxLag_ = 0;
  }
  normalize_ = ctrl_normalize_->to<mrs_bool> ();
  doNotNormalizeForLag0_ = ctrl_doNotNormalizeForLag0_->to<mrs_bool> ();
  unfoldToObservations_ = ctrl_unfoldToObservations_->to<mrs_bool> ();

  // Configure output flow.
  mrs_natural onObservations = unfoldToObservations_ ? inObservations_
                               * (maxLag_ + 1) : inObservations_;
  mrs_natural onSamples = unfoldToObservations_ ? 1 : maxLag_ + 1;
  ctrl_onSamples_->setValue(onSamples, NOUPDATE);
  ctrl_onObservations_->setValue(onObservations, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  // Handle the observation names.
  mrs_string onObsNames = ctrl_inObsNames_->to<mrs_string> ();
  if (unfoldToObservations_)
  {
    onObsNames
    = prefixObservationNamesWithAutocorrelationUnfoldingPrefixes_(
        onObsNames, normalize_, doNotNormalizeForLag0_, maxLag_);
  }
  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

  // Allocate and initialize the buffers and counters.
  this->acBuffer_.stretch(inObservations_, maxLag_ + 1);
  this->acBuffer_.setval(0.0);
  this->memory_.stretch(inObservations_, maxLag_);
  this->memory_.setval(0.0);

  // We just cleared the internal buffers, so we should reset the clear control.
  ctrl_clear_->setValue(false, NOUPDATE);
}

void RunningAutocorrelation::myProcess(realvec& in, realvec& out)
{
  /// Iterate over the observations and samples and do the processing.
  for (mrs_natural i = 0; i < inObservations_; ++i)
  {
    // Calculate the autocorrelation terms
    for (mrs_natural lag = 0; lag <= maxLag_; lag++)
    {
      // For the first part, we need the memory.
      for (mrs_natural n = 0; n < min(lag, inSamples_); n++)
      {
        acBuffer_(i, lag) += in(i, n) * memory_(i, maxLag_ - lag + n);
      }
      // For the second part, we have enough with the input slice.
      for (mrs_natural n = lag; n < inSamples_; n++)
      {
        acBuffer_(i, lag) += in(i, n) * in(i, n - lag);
      }
    }

    // Store result in output vector.
    mrs_natural u = (mrs_natural) unfoldToObservations_;
    mrs_natural o_base = unfoldToObservations_ ? i * (maxLag_ + 1) : i;
    for (mrs_natural lag = 0; lag <= maxLag_; lag++)
    {
      out(o_base + u * lag, (1 - u) * lag) = acBuffer_(i, lag);

    }
    // Do the (optional) normalization.
    if (normalize_ && acBuffer_(i, 0) > 0.0)
    {
      mrs_natural lag = doNotNormalizeForLag0_ ? 1 : 0;
      for (; lag <= maxLag_; lag++)
      {
        out(o_base + u * lag, (1 - u) * lag) /= acBuffer_(i, 0);
      }
    }

    // Remember the last samples for next time.
    // First, shift samples in memory (if needed).
    for (mrs_natural m = 0; m < maxLag_ - inSamples_; m++)
    {
      memory_(i, m) = memory_(i, m + inSamples_);
    }
    // Second, copy over samples from input.
    for (mrs_natural m = 1; m <= min(maxLag_, inSamples_); m++)
    {
      memory_(i, maxLag_ - m) = in(i, inSamples_ - m);
    }
  }
}
