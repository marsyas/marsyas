/*
 ** Copyright (C) 2009 Stefaan Lippens <soxofaan@gmail.com>
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

#include "RunningStatistics.h"
#include "../common_source.h"

#include <string>
#include <sstream>

using namespace std;
using namespace Marsyas;

RunningStatistics::RunningStatistics(mrs_string name) :
  MarSystem("RunningStatistics", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

RunningStatistics::RunningStatistics(const RunningStatistics& a) :
  MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_enable_mean_ = getctrl("mrs_bool/enableMean");
  ctrl_enable_stddev_ = getctrl("mrs_bool/enableStddev");
  ctrl_enable_skewness_ = getctrl("mrs_bool/enableSkewness");
  ctrl_clear_ = getctrl("mrs_bool/clear");
  ctrl_clearPerTick_ = getctrl("mrs_bool/clearPerTick");

}

RunningStatistics::~RunningStatistics()
{
}

MarSystem*
RunningStatistics::clone() const
{
  return new RunningStatistics(*this);
}

void RunningStatistics::addControls()
{
  addctrl("mrs_bool/enableMean", true, ctrl_enable_mean_);
  ctrl_enable_mean_->setState(true);
  addctrl("mrs_bool/enableStddev", true, ctrl_enable_stddev_);
  ctrl_enable_stddev_->setState(true);
  addctrl("mrs_bool/enableSkewness", false, ctrl_enable_skewness_);
  ctrl_enable_skewness_->setState(true);
  addctrl("mrs_bool/clear", false, ctrl_clear_);
  addctrl("mrs_bool/clearPerTick", false, ctrl_clearPerTick_);

}

void RunningStatistics::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("RunningStatistics.cpp - RunningStatistics:myUpdate");

  // Update caches of control values.
  enable_mean_ = ctrl_enable_mean_->to<mrs_bool> ();
  enable_stddev_ = ctrl_enable_stddev_->to<mrs_bool> ();
  enable_skewsness_ = ctrl_enable_skewness_->to<mrs_bool> ();

  mrs_natural fanout = (mrs_natural) enable_mean_
                       + (mrs_natural) enable_stddev_ + (mrs_natural) enable_skewsness_;

  // Set the output slice format and rate based on the
  // input slice format and rate.
  ctrl_onSamples_->setValue((mrs_natural) 1, NOUPDATE);
  onObservations_ = fanout * ctrl_inObservations_->to<mrs_natural> ();
  ctrl_onObservations_->setValue(onObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // Update the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string> ();
  mrs_string onObsNames("");
  if (enable_mean_)
  {
    onObsNames += obsNamesAddPrefix(inObsNames, "RunningMean_");
  }
  if (enable_stddev_)
  {
    onObsNames += obsNamesAddPrefix(inObsNames, "RunningStddev_");
  }
  if (enable_skewsness_)
  {
    onObsNames += obsNamesAddPrefix(inObsNames, "RunningSkewness_");
  }

  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

  // Allocate and initialize the buffers and counters.
  sumxBuffer_.stretch(onObservations_, 1);
  sumx2Buffer_.stretch(onObservations_, 1);
  sumx3Buffer_.stretch(onObservations_, 1);
  // Initialize the buffers.
  clear();
}

void RunningStatistics::clear(void) {
  sumxBuffer_.setval(0.0);
  sumx2Buffer_.setval(0.0);
  sumx3Buffer_.setval(0.0);
  samplecounter_ = 0;
  // We just cleared the internal buffers, so we should reset the clear control.
  ctrl_clear_->setValue(false, NOUPDATE);
}

void RunningStatistics::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  if (ctrl_clear_->to<mrs_bool>() || ctrl_clearPerTick_->to<mrs_bool>()) {
    clear();
  }

  // Update the sample counter.
  samplecounter_ += inSamples_;
  // Temporary variables for mean, stddev, ...
  mrs_real mean, var, stddev, skewness;
  // Temporary helper variable.
  mrs_real tmp;

  // Precalculate offsets where to put stddev and skewness in output.
  mrs_natural o_offset_stdev = (mrs_natural) enable_mean_ * inObservations_;
  mrs_natural o_offset_skewness = o_offset_stdev
                                  + (mrs_natural) enable_stddev_ * inObservations_;

  for (o = 0; o < inObservations_; o++)
  {
    // Gather the sums.
    for (t = 0; t < inSamples_; t++)
    {
      sumxBuffer_(o, 0) += in(o, t);
      sumx2Buffer_(o, 0) += (tmp = in(o, t) * in(o, t));
      sumx3Buffer_(o, 0) += (tmp * in(o, t));
    }
    // Calculate  the statistics.
    mean = sumxBuffer_(o, 0) / samplecounter_;
    var = sumx2Buffer_(o, 0) / samplecounter_ - mean * mean;
    stddev = sqrt(var);
    skewness = (sumx3Buffer_(o, 0) / samplecounter_ - 3 * mean * var - mean
                * mean * mean);
    skewness = var > 0.0 ? skewness / (var * stddev) : 0.0;

    // Store in output slice.
    if (enable_mean_)
    {
      out(o, 0) = mean;
    }
    if (enable_stddev_)
    {
      out(o + o_offset_stdev, 0) = stddev;
    }
    if (enable_skewsness_)
    {
      out(o + o_offset_skewness, 0) = skewness;
    }

  }
}
