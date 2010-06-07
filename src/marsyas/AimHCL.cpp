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

#include "AimHCL.h"

using namespace std;
using namespace Marsyas;

AimHCL::AimHCL(string name):MarSystem("AimHCL",name)
{
  is_initialized = false;
  initialized_lowpass_cutoff = 0;

  is_reset = false;
  reseted_inobservations = 0;
  reseted_lowpass_order = 0;

  addControls();
}


AimHCL::~AimHCL()
{
}


MarSystem*
AimHCL::clone() const
{
  return new AimHCL(*this);
}

void 
AimHCL::addControls()
{
  addControl("mrs_bool/do_lowpass", true , ctrl_do_lowpass_);
  addControl("mrs_bool/do_log", false , ctrl_do_log_);
  addControl("mrs_real/lowpass_cutoff", 1200.0 , ctrl_lowpass_cutoff_);
  addControl("mrs_natural/lowpass_order", 2 , ctrl_lowpass_order_);
}

void
AimHCL::myUpdate(MarControlPtr sender)
{
  (void) sender;
  MRSDIAG("AimHCL.cpp - AimHCL:myUpdate");
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>(), NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
  ctrl_onObsNames_->setValue("AimHCL_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

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
      reseted_lowpass_order != ctrl_lowpass_order_->to<mrs_real>()) {
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
AimHCL::InitializeInternal() {
  time_constant_ = 1.0f / (2.0f * PI * ctrl_lowpass_cutoff_->to<mrs_real>());
  return true;
}

void 
AimHCL::ResetInternal() {
  xn_ = 0.0f;
  yn_ = 0.0f;
  yns_.clear();
  yns_.resize(ctrl_inObservations_->to<mrs_natural>());
  for (int c = 0; c < ctrl_inObservations_->to<mrs_natural>(); ++c) {
    yns_[c].resize(ctrl_lowpass_order_->to<mrs_natural>(), 0.0f);
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
AimHCL::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  for (o = 0; o < ctrl_inObservations_->to<mrs_natural>(); o++) {
    for (t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {
      // out(o,t) = in(o,t);
      if (in(o,t) < 0.0f) {
        out(o, t) = 0.0f;
      } else {
        float s = in(o,t);
        if (ctrl_do_log_->to<mrs_bool>()) {
          s *= pow(2.0f, 15);
          if (s < 1.0f) s = 1.0f;
          s = 20.0f * log10(s);
        }
        out(o, t) = s;
      }
    }
    if (ctrl_do_lowpass_->to<mrs_bool>()) {
      float b = exp(-1.0f / (ctrl_israte_->to<mrs_natural>() * time_constant_));
      float gain = 1.0f / (1.0f - b);
      for (int j = 0; j < ctrl_lowpass_order_->to<mrs_natural>(); j++) {
        for (int k = 0; k < ctrl_onSamples_->to<mrs_natural>(); ++k) {
          xn_ = out(o,k);
          yn_ = xn_ + b * yns_[o][j];
          yns_[o][j] = yn_;
          out(o, k) = yn_ / gain;
        }
      }
    }
  }
}
