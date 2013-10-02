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

#include "AimGammatone.h"
#include "../common_source.h"
#include "../ERBTools.h"

using std::ostringstream;
using std::complex;
using std::abs;
using std::cout;
using std::endl;

using namespace Marsyas;

AimGammatone::AimGammatone(mrs_string name):MarSystem("AimGammatone",name)
{
  is_initialized = false;
  initialized_num_channels = 0;
  initialized_min_frequency = 0.0;
  initialized_max_frequency = 0.0;
  initialized_israte = 0.0;

  is_reset = false;
  reset_num_channels = 0;

  addControls();
}


AimGammatone::AimGammatone(const AimGammatone& a): MarSystem(a)
{
  is_initialized = false;
  initialized_num_channels = 0;
  initialized_min_frequency = 0.0;
  initialized_max_frequency = 0.0;
  initialized_israte = 0.0;

  is_reset = false;
  reset_num_channels = 0;

  ctrl_num_channels_= getctrl("mrs_natural/num_channels");
  ctrl_min_frequency_ = getctrl("mrs_real/min_frequency");
  ctrl_max_frequency_ = getctrl("mrs_real/max_frequency");
}

AimGammatone::~AimGammatone()
{
}


MarSystem*
AimGammatone::clone() const
{
  return new AimGammatone(*this);
}

void
AimGammatone::addControls()
{
  addControl("mrs_natural/num_channels", 200 , ctrl_num_channels_);
  addControl("mrs_real/min_frequency", 86.0 , ctrl_min_frequency_);
  addControl("mrs_real/max_frequency", 16000.0 , ctrl_max_frequency_);
}

void
AimGammatone::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  MRSDIAG("AimGammatone.cpp - AimGammatone:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
  ctrl_onObsNames_->setValue("AimGammatone_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  // Need to have double the amount of channels, the first set of
  // channels are for the signals the second set of channels are for
  // the centre frequencies
  ctrl_onObservations_->setValue(ctrl_num_channels_->to<mrs_natural>() , NOUPDATE);

  //
  // Does the MarSystem need initialization?
  //
  if (initialized_num_channels != ctrl_num_channels_->to<mrs_natural>() ||
      initialized_min_frequency != ctrl_min_frequency_->to<mrs_real>() ||
      initialized_max_frequency != ctrl_max_frequency_->to<mrs_real>() ||
      initialized_israte != ctrl_israte_->to<mrs_real>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_num_channels = ctrl_num_channels_->to<mrs_natural>();
    initialized_min_frequency = ctrl_min_frequency_->to<mrs_real>();
    initialized_max_frequency = ctrl_max_frequency_->to<mrs_real>();
    initialized_israte = ctrl_israte_->to<mrs_real>();
  }

  //
  // Does the MarSystem need a reset?
  //
  if (reset_num_channels != ctrl_num_channels_->to<mrs_natural>()) {
    is_reset = false;
  }

  if (!is_reset) {
    ResetInternal();
    is_reset = true;
    reset_num_channels = ctrl_num_channels_->to<mrs_natural>();
  }

}

bool
AimGammatone::InitializeInternal() {
  mrs_natural num_channels = ctrl_num_channels_->to<mrs_natural>();
  double min_frequency = ctrl_min_frequency_->to<mrs_real>();
  double max_frequency = ctrl_max_frequency_->to<mrs_real>();

  // Calculate number of channels, and centre frequencies
  double erb_max = ERBTools::Freq2ERB(max_frequency);
  double erb_min = ERBTools::Freq2ERB(min_frequency);
  double delta_erb = (erb_max - erb_min) / (num_channels - 1);

  centre_frequencies_.resize(num_channels);
  double erb_current = erb_min;

  for (int i = 0; i < num_channels; ++i) {
    centre_frequencies_[i] = ERBTools::ERB2Freq(erb_current);
    erb_current += delta_erb;
  }

  a_.resize(num_channels);
  b1_.resize(num_channels);
  b2_.resize(num_channels);
  b3_.resize(num_channels);
  b4_.resize(num_channels);
  state_1_.resize(num_channels);
  state_2_.resize(num_channels);
  state_3_.resize(num_channels);
  state_4_.resize(num_channels);

  for (int ch = 0; ch < num_channels; ++ch) {
    double cf = centre_frequencies_[ch];
    double erb = ERBTools::Freq2ERBw(cf);

    // Sample interval
    double dt = 1.0 / ctrl_israte_->to<mrs_real>();

    // Bandwidth parameter
    double b = 1.019 * 2.0 * PI * erb;

    // The following expressions are derived in Apple TR #35, "An
    // Efficient Implementation of the Patterson-Holdsworth Cochlear
    // Filter Bank" and used in Malcolm Slaney's auditory toolbox, where he
    // defines this alternaltive four stage cascade of second-order filters.

    // Calculate the gain:
    double cpt = cf * PI * dt;
    complex<double> exponent(0.0, 2.0 * cpt);
    complex<double> ec = exp(2.0 * exponent);
    complex<double> two_cf_pi_t(2.0 * cpt, 0.0);
    complex<double> two_pow(pow(2.0, (3.0 / 2.0)), 0.0);
    complex<double> p1 = -2.0 * ec * dt;
    complex<double> p2 = 2.0 * exp(-(b * dt) + exponent) * dt;
    complex<double> b_dt(b * dt, 0.0);

    double gain = abs(
                    (p1 + p2 * (cos(two_cf_pi_t) - sqrt(3.0 - two_pow) * sin(two_cf_pi_t)))
                    * (p1 + p2 * (cos(two_cf_pi_t) + sqrt(3.0 - two_pow) * sin(two_cf_pi_t)))
                    * (p1 + p2 * (cos(two_cf_pi_t) - sqrt(3.0 + two_pow) * sin(two_cf_pi_t)))
                    * (p1 + p2 * (cos(two_cf_pi_t) + sqrt(3.0 + two_pow) * sin(two_cf_pi_t)))
                    / pow((-2.0 / exp(2.0 * b_dt) - 2.0 * ec + 2.0 * (1.0 + ec)
                           / exp(b_dt)), 4));

    // The filter coefficients themselves:
    const int coeff_count = 3;
    a_[ch].resize(coeff_count, 0.0);
    b1_[ch].resize(coeff_count, 0.0);
    b2_[ch].resize(coeff_count, 0.0);
    b3_[ch].resize(coeff_count, 0.0);
    b4_[ch].resize(coeff_count, 0.0);
    state_1_[ch].resize(coeff_count, 0.0);
    state_2_[ch].resize(coeff_count, 0.0);
    state_3_[ch].resize(coeff_count, 0.0);
    state_4_[ch].resize(coeff_count, 0.0);

    double B0 = dt;
    double B2 = 0.0;

    double B11 = -(2.0 * dt * cos(2.0 * cf * PI * dt) / exp(b * dt)
                   + 2.0 * sqrt(3 + pow(2.0, 1.5)) * dt
                   * sin(2.0 * cf * PI * dt) / exp(b * dt)) / 2.0;
    double B12 = -(2.0 * dt * cos(2.0 * cf * PI * dt) / exp(b * dt)
                   - 2.0 * sqrt(3 + pow(2.0, 1.5)) * dt
                   * sin(2.0 * cf * PI * dt) / exp(b * dt)) / 2.0;
    double B13 = -(2.0 * dt * cos(2.0 * cf * PI * dt) / exp(b * dt)
                   + 2.0 * sqrt(3 - pow(2.0, 1.5)) * dt
                   * sin(2.0 * cf * PI * dt) / exp(b * dt)) / 2.0;
    double B14 = -(2.0 * dt * cos(2.0 * cf * PI * dt) / exp(b * dt)
                   - 2.0 * sqrt(3 - pow(2.0, 1.5)) * dt
                   * sin(2.0 * cf * PI * dt) / exp(b * dt)) / 2.0;

    a_[ch][0] = 1.0;
    a_[ch][1] = -2.0 * cos(2.0 * cf * PI * dt) / exp(b * dt);
    a_[ch][2] = exp(-2.0 * b * dt);
    b1_[ch][0] = B0 / gain;
    b1_[ch][1] = B11 / gain;
    b1_[ch][2] = B2 / gain;
    b2_[ch][0] = B0;
    b2_[ch][1] = B12;
    b2_[ch][2] = B2;
    b3_[ch][0] = B0;
    b3_[ch][1] = B13;
    b3_[ch][2] = B2;
    b4_[ch][0] = B0;
    b4_[ch][1] = B14;
    b4_[ch][2] = B2;
  }
  return true;
}

void
AimGammatone::ResetInternal() {
  mrs_natural num_channels = ctrl_num_channels_->to<mrs_natural>();

  state_1_.resize(num_channels);
  state_2_.resize(num_channels);
  state_3_.resize(num_channels);
  state_4_.resize(num_channels);
  for (int i = 0; i < num_channels; ++i) {
    state_1_[i].resize(3, 0.0);
    state_2_[i].resize(3, 0.0);
    state_3_[i].resize(3, 0.0);
    state_4_[i].resize(3, 0.0);
  }
}

void
AimGammatone::myProcess(realvec& in, realvec& out)
{
  int audio_channel = 0;

  std::vector<std::vector<double> >::iterator b1 = b1_.begin();
  std::vector<std::vector<double> >::iterator b2 = b2_.begin();
  std::vector<std::vector<double> >::iterator b3 = b3_.begin();
  std::vector<std::vector<double> >::iterator b4 = b4_.begin();
  std::vector<std::vector<double> >::iterator a = a_.begin();
  std::vector<std::vector<double> >::iterator s1 = state_1_.begin();
  std::vector<std::vector<double> >::iterator s2 = state_2_.begin();
  std::vector<std::vector<double> >::iterator s3 = state_3_.begin();
  std::vector<std::vector<double> >::iterator s4 = state_4_.begin();

  // Temporary storage between filter stages
  std::vector<double> outbuff(ctrl_inSamples_->to<mrs_natural>());
  mrs_natural _channel_count = ctrl_num_channels_->to<mrs_natural>();
  mrs_natural _inSamples = ctrl_inSamples_->to<mrs_natural>();

  for (int ch = 0; ch < _channel_count;
       ++ch, ++b1, ++b2, ++b3, ++b4, ++a, ++s1, ++s2, ++s3, ++s4) {
    for (int i = 0; i < _inSamples; ++i) {
      // Direct-form-II IIR filter
      double inputsample = in(audio_channel, i);
      outbuff[i] = (*b1)[0] * inputsample + (*s1)[0];
      for (unsigned int stage = 1; stage < s1->size(); ++stage)
        (*s1)[stage - 1] = (*b1)[stage] * inputsample
                           - (*a)[stage] * outbuff[i] + (*s1)[stage];
    }
    for (int i = 0; i < _inSamples; ++i) {
      // Direct-form-II IIR filter
      double inputsample = outbuff[i];
      outbuff[i] = (*b2)[0] * inputsample + (*s2)[0];
      for (unsigned int stage = 1; stage < s2->size(); ++stage)
        (*s2)[stage - 1] = (*b2)[stage] * inputsample
                           - (*a)[stage] * outbuff[i] + (*s2)[stage];
    }
    for (int i = 0; i < _inSamples; ++i) {
      // Direct-form-II IIR filter
      double inputsample = outbuff[i];
      outbuff[i] = (*b3)[0] * inputsample + (*s3)[0];
      for (unsigned int stage = 1; stage < s3->size(); ++stage)
        (*s3)[stage - 1] = (*b3)[stage] * inputsample
                           - (*a)[stage] * outbuff[i] + (*s3)[stage];
    }
    for (int i = 0; i < _inSamples; ++i) {
      // Direct-form-II IIR filter
      double inputsample = outbuff[i];
      outbuff[i] = (*b4)[0] * inputsample + (*s4)[0];
      for (unsigned int stage = 1; stage < s4->size(); ++stage)
        (*s4)[stage - 1] = (*b4)[stage] * inputsample
                           - (*a)[stage] * outbuff[i] + (*s4)[stage];
      out(ch, i) = outbuff[i];
    }
  }

  // Copy over the centre frequencies to the second half of the observations
  /* for (t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {
    for (o = 0; o < _channel_count; o++) {
      out(o + _channel_count, t) = centre_frequencies_[o];
    }
  }
  */

}
