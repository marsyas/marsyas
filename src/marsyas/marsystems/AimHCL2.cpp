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

#include "AimHCL2.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

AimHCL2::AimHCL2(mrs_string name):MarSystem("AimHCL2",name)
{
  is_initialized = false;
  initialized_lowpass_cutoff = 0;

  is_reset = false;
  reseted_inobservations = 0;
  reseted_lowpass_order = 0;

  addControls();
}


AimHCL2::~AimHCL2()
{
}


MarSystem*
AimHCL2::clone() const
{
  return new AimHCL2(*this);
}

void
AimHCL2::addControls()
{
  addControl("mrs_bool/do_lowpass", true , ctrl_do_lowpass_);
  addControl("mrs_bool/do_log", false , ctrl_do_log_);
  addControl("mrs_real/lowpass_cutoff", 1200.0 , ctrl_lowpass_cutoff_);
  addControl("mrs_natural/lowpass_order", 2 , ctrl_lowpass_order_);
}

void
AimHCL2::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AimHCL2.cpp - AimHCL2:myUpdate");
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>(), NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
  ctrl_onObsNames_->setValue("AimHCL2_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  //
  // Does the MarSystem need initialization?
  //
  if (initialized_lowpass_cutoff != ctrl_lowpass_cutoff_->to<mrs_real>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_lowpass_cutoff = ctrl_lowpass_cutoff_->to<mrs_real>();
  }

  //
  // Does the MarSystem need a reset?
  //
  if (reseted_inobservations != ctrl_inObservations_->to<mrs_natural>() ||
      reseted_lowpass_order != ctrl_lowpass_order_->to<mrs_natural>()) {
    is_reset = false;
  }

  if (!is_reset) {
    ResetInternal();
    is_reset = true;
    reseted_inobservations = ctrl_inObservations_->to<mrs_natural>();
    reseted_lowpass_order = ctrl_lowpass_order_->to<mrs_natural>();
  }

}


bool
AimHCL2::InitializeInternal() {
  time_constant_ = 1.0 / (2.0 * PI * ctrl_lowpass_cutoff_->to<mrs_real>());
  return true;
}

void
AimHCL2::ResetInternal() {
  xn_ = 0.0;
  yn_ = 0.0;
  yns_.clear();
  yns_.resize(ctrl_inObservations_->to<mrs_natural>());
  mrs_natural _lowpass_order = ctrl_lowpass_order_->to<mrs_natural>();
  for (int c = 0; c < ctrl_inObservations_->to<mrs_natural>(); ++c) {
    yns_[c].resize(_lowpass_order, 0.0);
  }
}

//
// With do_log, the signal is first scaled up so that values <1.0 become
// negligible. This just rescales the sample values to fill the range of a
// 16-bit signed integer, then we lose the bottom bit of resolution. If the
// signal was sampled at 16-bit resolution, there shouldn't be anything to
// speak of there anyway. If it was sampled using a higher resolution, then
// some data will be discarded.
//
void
AimHCL2::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_real _israte = ctrl_israte_->to<mrs_real>();
  mrs_natural _inObservations = ctrl_inObservations_->to<mrs_natural>();
  mrs_natural _inSamples = ctrl_inSamples_->to<mrs_natural>();
  mrs_natural _onSamples = ctrl_onSamples_->to<mrs_natural>();
  mrs_natural _lowpass_order = ctrl_lowpass_order_->to<mrs_natural>();
  mrs_bool _do_lowpass = ctrl_do_lowpass_->to<mrs_bool>();
  mrs_bool _do_log = ctrl_do_log_->to<mrs_bool>();

  mrs_natural _num_channels = _inObservations;
  double b = exp(-1.0 / (_israte * time_constant_));
  double gain = 1.0 / (1.0 - b);

  for (o = 0; o < _num_channels; ++o) {
    for (t = 0; t < _inSamples; ++t) {
      // cout << "in(o,t)=" << in(o,t) << endl;
      if (in(o,t) < 0.0) {
        // cout << "s=zero" << endl;
        out(o, t) = 0.0;
      } else {
        double s = in(o,t);
        if (_do_log) {
          s *= pow(2.0, 15);
          if (s < 1.0) s = 1.0;
          s = 20.0 * log10(s);
        }
        // cout << "s=" << s << endl;
        out(o, t) = s;
      }
    }
    if (_do_lowpass) {

      for (int j = 0; j < _lowpass_order; j++) {
        for (int k = 0; k < _onSamples; ++k) {
          xn_ = out(o,k);
          yn_ = xn_ + b * yns_[o][j];
          yns_[o][j] = yn_;
          // cout << "gain=" << gain << endl;
          // cout << "yn_=" << yn_ << endl;
          out(o, k) = yn_ / gain;
        }
      }
    }
  }



}
