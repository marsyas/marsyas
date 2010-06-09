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

#include "AimSSI.h"

using namespace std;
using namespace Marsyas;

AimSSI::AimSSI(string name):MarSystem("AimSSI",name)
{
  is_centre_frequencies_calculated = false;
  addControls();
}


AimSSI::~AimSSI()
{
}

bool 
AimSSI::InitializeInternal() {
  return true;
}

void 
AimSSI::ResetInternal() {
}

// Calculates log2 of number.
double
AimSSI::Log2(double n) {
  // log(n)/log(2) is log2.  
  return log( n ) / log( 2 );
}


MarSystem*
AimSSI::clone() const
{
  return new AimSSI(*this);
}

void 
AimSSI::addControls()
{
  addControl("mrs_bool/do_pitch_cutoff", false , ctrl_do_pitch_cutoff_);
  addControl("mrs_bool/weight_by_cutoff", false , ctrl_weight_by_cutoff_);
  addControl("mrs_bool/weight_by_scaling", false , ctrl_weight_by_scaling_);
  addControl("mrs_bool/log_cycles_axis", true , ctrl_log_cycles_axis_);
  addControl("mrs_real/pitch_search_start_ms", 2.0f , ctrl_pitch_search_start_ms_);
  addControl("mrs_real/ssi_width_cycles", 10.0f , ctrl_ssi_width_cycles_);
  addControl("mrs_real/pivot_cf", 1000.0f , ctrl_pivot_cf_);

  // From AimGammatone
  addControl("mrs_real/min_frequency", 86.0f , ctrl_min_frequency_);
  addControl("mrs_real/max_frequency", 16000.0f , ctrl_max_frequency_);

}

void
AimSSI::myUpdate(MarControlPtr sender)
{

  (void) sender;
  MRSDIAG("AimSSI.cpp - AimSSI:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimSSI_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  // sness - Used to be in InitializeInternal in AIM-C.  Would have
  // been really wasteful to keep track of all these variables that
  // need to be initialized.

  // sness - Hacking this for now
  ssi_width_samples_ = 512;
  // ssi_width_samples_ =  ctrl_israte_->to<mrs_real>() * ctrl_ssi_width_cycles_->to<mrs_real>() / ctrl_pivot_cf_->to<mrs_real>();

  if (ssi_width_samples_ > ctrl_inSamples_->to<mrs_natural>()) {
    ssi_width_samples_ = ctrl_inSamples_->to<mrs_natural>();
    float cycles = ssi_width_samples_ * ctrl_pivot_cf_->to<mrs_real>() / ctrl_israte_->to<mrs_real>();
    MRSWARN("Requested SSI width is too long for the input buffer");
    // MRSWARN("Requested SSI width of " + ctrl_ssi_width_cycles_->to<mrs_real>() + " cycles is too long for the " +
    //         "input buffer length of " + ctrl_inObservations_->to<mrs_natural>() + " samples. The SSI will be " +
    //             "truncated at " + ssi_width_samples_ + " samples wide. This corresponds to a width " +
    //             "of " + cycles + " cycles.")
    ctrl_ssi_width_cycles_ = cycles;
  }

  if (!is_centre_frequencies_calculated) {
    CalculateCentreFrequencies();
    is_centre_frequencies_calculated = true;
  }

}

// sness - Because we can't pass the centre_frequencies from one
// MarSystem to the next, we need to recalculate them here.
void 
AimSSI::CalculateCentreFrequencies() {
  int num_channels = ctrl_inObservations_->to<mrs_natural>();
  float erb_max = ERBTools::Freq2ERB(ctrl_max_frequency_->to<mrs_real>());
  float erb_min = ERBTools::Freq2ERB(ctrl_min_frequency_->to<mrs_real>());
  float delta_erb = (erb_max - erb_min) / (num_channels - 1);

  centre_frequencies_.resize(num_channels);
  float erb_current = erb_min;

  for (int i = 0; i < num_channels; ++i) {
    centre_frequencies_[i] = ERBTools::ERB2Freq(erb_current);
    erb_current += delta_erb;
  }
}

int 
AimSSI::ExtractPitchIndex(realvec& in) const {
  // Generate temporal profile of the SAI
  std::vector<float> sai_temporal_profile(ctrl_inSamples_->to<mrs_natural>(), 0.0f);
  for (int i = 0; i < ctrl_inSamples_->to<mrs_natural>(); ++i) {
    float val = 0.0f;
    for (int ch = 0; ch < ctrl_inObservations_->to<mrs_natural>(); ++ch) {
      val += in(ch, i);
    }
    sai_temporal_profile[i] = val;
  }

  // Find pitch value
  int start_sample = floor(ctrl_pitch_search_start_ms_->to<mrs_real>() * ctrl_israte_->to<mrs_real>() / 1000.0f);
  int max_idx = 0;
  float max_val = 0.0f;
  for (int i = start_sample; i < ctrl_inSamples_->to<mrs_natural>(); ++i) {
    if (sai_temporal_profile[i] > max_val) {
      max_idx = i;
      max_val = sai_temporal_profile[i];
    }
  }
  return max_idx;
}


void
AimSSI::myProcess(realvec& in, realvec& out)
{
  int pitch_index = ctrl_inSamples_->to<mrs_natural>() - 1;
  if (ctrl_do_pitch_cutoff_->to<mrs_bool>()) {
    pitch_index = ExtractPitchIndex(in);
  }
  
  for (o = 0; o < ctrl_inObservations_->to<mrs_natural>(); o++) {
    float centre_frequency = centre_frequencies_[o];
    // Copy the buffer from input to output, addressing by h-value
    for (t = 0; t < ssi_width_samples_; t++) {
      float h;
      float cycle_samples = ctrl_israte_->to<mrs_real>() / centre_frequency;
      if (ctrl_log_cycles_axis_->to<mrs_bool>()) {
        float gamma_min = -1.0f;
        float gamma_max = Log2(ctrl_ssi_width_cycles_->to<mrs_real>());
        float gamma = gamma_min + (gamma_max - gamma_min)
                                   * static_cast<float>(t)
                                   / static_cast<float>(ssi_width_samples_);
        h = pow(2.0f, gamma);
      } else {
        h = static_cast<float>(t) * ctrl_ssi_width_cycles_->to<mrs_real>()
            / static_cast<float>(ssi_width_samples_);
      }

      // The index into the input array is a floating-point number, which is
      // split into a whole part and a fractional part. The whole part and
      // fractional part are found, and are used to linearly interpolate
      // between input samples to yield an output sample.
      double whole_part;
      float frac_part = modf(h * cycle_samples, &whole_part);
      int sample = floor(whole_part);

      float weight = 1.0f;

      int cutoff_index = ctrl_inSamples_->to<mrs_natural>() - 1;
      if (ctrl_do_pitch_cutoff_->to<mrs_bool>()) {
        if (pitch_index < cutoff_index) {
          if (ctrl_weight_by_cutoff_->to<mrs_bool>()) {
            weight *= static_cast<float>(ctrl_inSamples_->to<mrs_natural>())
                      / static_cast<float>(pitch_index);
          }
          cutoff_index = pitch_index;
        }
      }

      if (ctrl_weight_by_scaling_->to<mrs_bool>()) {
        if (centre_frequency > ctrl_pivot_cf_->to<mrs_real>()) {
          weight *= (centre_frequency / ctrl_pivot_cf_->to<mrs_real>());
        }
      }

      float val;
      if (sample < cutoff_index) {
        float curr_sample = in(o, sample);
        float next_sample = in(o, sample + 1);
        val = weight * (curr_sample
                        + frac_part * (next_sample - curr_sample));
      } else {
        val = 0.0f;
      }
      out(o, t) = val;
    }
  }

}
