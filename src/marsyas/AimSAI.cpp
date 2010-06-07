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

#include "AimSAI.h"

using namespace std;
using namespace Marsyas;

AimSAI::AimSAI(string name):MarSystem("AimSAI",name)
{
  is_initialized = false;
  initialized_israte = 0;
  initialized_inobservations = 0;
  initialized_insamples = 0;
  initialized_frame_period_ms = 0.0;
  initialized_min_delay_ms = 0.0;
  initialized_max_delay_ms = 0.0;
  initialized_buffer_memory_decay = 0.0;
  initialized_max_concurrent_strobes = 0;
  initialized_strobe_weight_alpha = 0.0;

  is_reset = false;
  reseted_israte = 0;
  reseted_inobservations = 0;
  reseted_frame_period_ms = 0;

  is_centre_frequencies_calculated = false;
  centre_frequencies_inobservations = 0;
  centre_frequencies_max_frequency = 0.0;
  centre_frequencies_min_frequency = 0.0;

  addControls();

  is_centre_frequencies_calculated = false;

  // Set default values
  min_strobe_delay_idx_ = 0;
  max_strobe_delay_idx_ = 0;
  sai_decay_factor_ = 0.0f;
  fire_counter_ = 0;
}

AimSAI::~AimSAI()
{
}

MarSystem*
AimSAI::clone() const
{
  return new AimSAI(*this);
}

void 
AimSAI::addControls()
{
  addControl("mrs_real/min_delay_ms_", 0.0f , ctrl_min_delay_ms_);
  addControl("mrs_real/max_delay_ms_", 35.0f , ctrl_max_delay_ms_);
  addControl("mrs_real/strobe_weight_alpha_", 0.5f , ctrl_strobe_weight_alpha_);
  addControl("mrs_real/buffer_memory_decay_;", 0.03f , ctrl_buffer_memory_decay_);
  // addControl("mrs_real/frame_period_ms_ ", 20.0f , ctrl_frame_period_ms_);
  addControl("mrs_real/frame_period_ms_ ", 11.61f , ctrl_frame_period_ms_);
  addControl("mrs_natural/max_concurrent_strobes_;", 50 , ctrl_max_concurrent_strobes_);
  addControl("mrs_real/min_frequency", 86.0f , ctrl_min_frequency_);
  addControl("mrs_real/max_frequency", 16000.0f , ctrl_max_frequency_);
}

void
AimSAI::myUpdate(MarControlPtr sender)
{
  int temp_frame_period_samples = floor(ctrl_israte_->to<mrs_real>() * ctrl_frame_period_ms_->to<mrs_real>()
                                / 1000.0f);

  MRSDIAG("AimSAI.cpp - AimSAI:myUpdate");

  (void) sender;
  // ctrl_onSamples_->setValue(sai_buffer_length, NOUPDATE);
  ctrl_onSamples_->setValue(temp_frame_period_samples, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>()/2, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimSAI_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);


  //
  // Does the MarSystem need initialization?
  //
  if (initialized_israte != ctrl_israte_->to<mrs_real>() ||
      initialized_inobservations != ctrl_inObservations_->to<mrs_natural>() ||
      initialized_insamples != ctrl_inSamples_->to<mrs_natural>() ||
      initialized_frame_period_ms != ctrl_frame_period_ms_->to<mrs_real>() ||
      initialized_min_delay_ms != ctrl_min_delay_ms_->to<mrs_real>() ||
      initialized_max_delay_ms != ctrl_max_delay_ms_->to<mrs_real>() ||
      initialized_buffer_memory_decay != ctrl_buffer_memory_decay_->to<mrs_real>() ||
      initialized_max_concurrent_strobes != ctrl_max_concurrent_strobes_->to<mrs_real>() ||
      initialized_strobe_weight_alpha != ctrl_strobe_weight_alpha_->to<mrs_real>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_israte = ctrl_israte_->to<mrs_real>();
    initialized_inobservations = ctrl_inObservations_->to<mrs_natural>();
    initialized_insamples = ctrl_inSamples_->to<mrs_natural>();
    initialized_frame_period_ms = ctrl_frame_period_ms_->to<mrs_real>();
    initialized_min_delay_ms = ctrl_min_delay_ms_->to<mrs_real>();
    initialized_max_delay_ms = ctrl_max_delay_ms_->to<mrs_real>();
    initialized_buffer_memory_decay = ctrl_buffer_memory_decay_->to<mrs_real>();
    initialized_max_concurrent_strobes = ctrl_max_concurrent_strobes_->to<mrs_natural>();
    initialized_strobe_weight_alpha = ctrl_strobe_weight_alpha_->to<mrs_real>();
  }

  //
  // Does the MarSystem need a reset?
  //
  if (reseted_israte != ctrl_israte_->to<mrs_real>() || 
      reseted_inobservations != ctrl_inObservations_->to<mrs_natural>() || 
      reseted_frame_period_ms != ctrl_frame_period_ms_->to<mrs_real>()) {
    is_reset = false;
  }

  if (!is_reset) {
    ResetInternal();
    is_reset = true;
    reseted_israte = ctrl_israte_->to<mrs_real>();
    reseted_inobservations = ctrl_inObservations_->to<mrs_natural>();
    reseted_frame_period_ms = ctrl_frame_period_ms_->to<mrs_real>();
  }


  //
  // Does the MarSystem need to recalculate the centre_frequencies?
  //
  if (centre_frequencies_inobservations != ctrl_inObservations_->to<mrs_natural>() || 
      centre_frequencies_max_frequency != ctrl_max_frequency_->to<mrs_natural>() || 
      centre_frequencies_min_frequency != ctrl_min_frequency_->to<mrs_natural>()) {
    is_centre_frequencies_calculated = false;
  }

  //
  // Have the centre frequencies been calculated
  //
  if (!is_centre_frequencies_calculated) {
    CalculateCentreFrequencies();
    is_centre_frequencies_calculated = true;
  }

}

bool 
AimSAI::InitializeInternal() {
  // The SAI output bank must be as long as the SAI's Maximum delay.
  // One sample is added to the SAI buffer length to account for the
  // zero-lag point
  int sai_buffer_length = 1 + floor(ctrl_israte_->to<mrs_real>() * ctrl_max_delay_ms_->to<mrs_real>()
                                    / 1000.0f);
  channel_count_ = ctrl_inObservations_->to<mrs_natural>() / 2;

  // // Make an output SignalBank with the same number of channels and centre
  // // frequencies as the input, but with a different buffer length
  // if (!output_.Initialize(input.channel_count(),
  //                         sai_buffer_length,
  //                         input.sample_rate())) {
  //   LOG_ERROR("Failed to create output buffer in SAI module");
  //   return false;
  // }
  // for (int i = 0; i < inObservations; ++i) {
  //   output_.set_centre_frequency(i, input.centre_frequency(i));
  // }

  // // sai_temp_ will be initialized to zero
  // if (!sai_temp_.Initialize(output_)) {
  //   LOG_ERROR("Failed to create temporary buffer in SAI module");
  //   return false;
  // }
  sai_temp_.create(channel_count_,ctrl_inSamples_->to<mrs_natural>());
  frame_period_samples_ = floor(ctrl_israte_->to<mrs_real>() * ctrl_frame_period_ms_->to<mrs_real>()
                                / 1000.0f);
  min_strobe_delay_idx_ = floor(ctrl_israte_->to<mrs_real>() * ctrl_min_delay_ms_->to<mrs_real>()
                                / 1000.0f);
  max_strobe_delay_idx_ = floor(ctrl_israte_->to<mrs_real>() * ctrl_max_delay_ms_->to<mrs_real>()
                                / 1000.0f);

  // Make sure we don't go past the output buffer's upper bound
  if (max_strobe_delay_idx_ > ctrl_onSamples_->to<mrs_natural>()) {
    max_strobe_delay_idx_ = ctrl_onSamples_->to<mrs_natural>();
  }

  // Define decay factor from time since last sample (see ti2003)
  sai_decay_factor_ = pow(0.5f, 1.0f / (float)(ctrl_buffer_memory_decay_->to<mrs_real>() * (float)ctrl_israte_->to<mrs_real>()));

  // Precompute strobe weights
  strobe_weights_.resize(ctrl_max_concurrent_strobes_->to<mrs_natural>());
  for (int n = 0; n < ctrl_max_concurrent_strobes_->to<mrs_natural>(); ++n) {
    strobe_weights_[n] = pow(1.0f / (n + 1), (float)ctrl_strobe_weight_alpha_->to<mrs_real>());
  }
}

void 
AimSAI::ResetInternal() {
  // Active Strobes
  active_strobes_.clear();
  active_strobes_.resize(channel_count_);
  fire_counter_ = frame_period_samples_ - 1;
}


// We are passed a realvec with the signals and then the strobes for
// the signals, so we have twice the number of observations that we
// would usually have.  Take the strobes part of the realvec and find
// the strobes for each observation.
void
AimSAI::findStrobes(realvec& in) {
  strobes_.clear();
  strobes_.resize(channel_count_);
  for (int o = 0; o < channel_count_; o++) {
    strobes_[o].clear();
    for (int t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {
      if (in(o + channel_count_,t) == 1) {
        strobes_[o].push_back(t);
      }
    }
  }
}

// sness - Because we can't pass the centre_frequencies from one
// MarSystem to the next, we need to recalculate them here.
void 
AimSAI::CalculateCentreFrequencies() {
  // sness - Need to divide by two because there is one strobe channel
  // for every observation channel
  int num_channels = ctrl_inObservations_->to<mrs_natural>() / 2;
  float erb_min = ERBTools::Freq2ERB(ctrl_min_frequency_->to<mrs_real>());
  float erb_max = ERBTools::Freq2ERB(ctrl_max_frequency_->to<mrs_real>());
  float delta_erb = (erb_max - erb_min) / (num_channels - 1);

  centre_frequencies_.resize(num_channels);
  float erb_current = erb_min;

  for (int i = 0; i < num_channels; ++i) {
    centre_frequencies_[i] = ERBTools::ERB2Freq(erb_current);
    erb_current += delta_erb;
  }
}

void
AimSAI::myProcess(realvec& in, realvec& out)
{
  findStrobes(in);

  // Reset the next strobe times
  next_strobes_.clear();
  next_strobes_.resize(channel_count_, 0);

  // Offset the times on the strobes from the previous buffer
  for (int o = 0; o < channel_count_; ++o) {
    active_strobes_[o].ShiftStrobes(ctrl_inSamples_->to<mrs_natural>());
  }

  // Loop over samples to make the SAI
  for (int t = 0; t < ctrl_inSamples_->to<mrs_natural>(); ++t) {
    float decay_factor = pow(sai_decay_factor_, fire_counter_);
    // Loop over channels
    for (int o = 0; o < channel_count_; ++o) {
      // Local convenience variables
      StrobeList &active_strobes = active_strobes_[o];
      int next_strobe_index = next_strobes_[o];
      // Update strobes
      // If we are up to or beyond the next strobe...
      if (next_strobe_index < strobes_[o].size()) {

        if (t == strobes_[o][next_strobe_index]) {
          // A new strobe has arrived.
          // If there are too many strobes active, then get rid of the
          // earliest one
          if (active_strobes.strobe_count() >= ctrl_max_concurrent_strobes_->to<mrs_natural>()) {
            active_strobes.DeleteFirstStrobe();
          }

          // Add the active strobe to the list of current strobes and
          // calculate the strobe weight
          float weight = 1.0f;
          if (active_strobes.strobe_count() > 0) {
            int last_strobe_time = active_strobes.Strobe(
              active_strobes.strobe_count() - 1).time;

            // If the strobe occured within 10 impulse-response
            // cycles of the previous strobe, then lower its weight
            weight = (t - last_strobe_time) / ctrl_israte_->to<mrs_real>()
                     * centre_frequencies_[o] / 10.0f;
            if (weight > 1.0f)
              weight = 1.0f;
          }
          active_strobes.AddStrobe(t, weight);
          next_strobe_index++;

          // Update the strobe weights
          float total_strobe_weight = 0.0f;
          for (int si = 0; si < active_strobes.strobe_count(); ++si) {
            total_strobe_weight += (active_strobes.Strobe(si).weight
              * strobe_weights_[active_strobes.strobe_count() - si - 1]);
          }
          for (int si = 0; si < active_strobes.strobe_count(); ++si) {
            active_strobes.SetWorkingWeight(si,
              (active_strobes.Strobe(si).weight
               * strobe_weights_[active_strobes.strobe_count() - si - 1])
              / total_strobe_weight);
          }
        }
      }

      // Remove inactive strobes
      while (active_strobes.strobe_count() > 0) {
        // Get the relative time of the first strobe, and see if it exceeds
        // the maximum allowed time.
        if ((t - active_strobes.Strobe(0).time) > max_strobe_delay_idx_)
          active_strobes.DeleteFirstStrobe();
        else
          break;
      }

      // Update the SAI buffer with the weighted effect of all the active
      // strobes at the current sample
      for (int si = 0; si < active_strobes.strobe_count(); ++si) {
        // Add the effect of active strobe at correct place in the SAI buffer
        // Calculate 'delay', the time from the strobe event to now
        int delay = t - active_strobes.Strobe(si).time;

        // If the delay is greater than the (user-set)
        // minimum strobe delay, the strobe can be used
        if (delay >= min_strobe_delay_idx_ && delay < max_strobe_delay_idx_) {
          // The value at be added to the SAI
          float sig = in(o, t);

          // Weight the sample correctly
          sig *= active_strobes.Strobe(si).working_weight;

          // Adjust the weight acording to the number of samples until the
          // next output frame
          sig *= decay_factor;

          // Update the temporary SAI buffer
          out(o, delay) = out(o, delay) + sig;
        }
      }

      next_strobes_[o] = next_strobe_index;
    }  // End loop over channels

    fire_counter_--;

    // Check to see if we need to output an SAI frame on this sample
    if (fire_counter_ <= 0) {
      // Decay the SAI by the correct amount and add the current output frame
      float decay = pow(sai_decay_factor_, frame_period_samples_);

      for (int o = 0; o < channel_count_; ++o) {
        for (int t = 0; t < ctrl_inSamples_->to<mrs_natural>(); ++t ) {
          out(o, t) = sai_temp_(o,t) + out(o,t) * decay;
        }
      }

      // Zero the temporary signal
      for (int o = 0; o < sai_temp_.getRows(); ++o) {
        for (int t = 0; t < sai_temp_.getCols(); ++t) {
          sai_temp_(o, t) =  0.0f;
        }
      }

      fire_counter_ = frame_period_samples_ - 1;
    }
  }  // End loop over samples

}
