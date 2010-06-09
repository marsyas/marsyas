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

#include "AimPZFC.h"

using namespace std;
using namespace Marsyas;

AimPZFC::AimPZFC(string name):MarSystem("AimPZFC",name)
{
  is_initialized = false;
  initialized_israte = 0.0;
  initialized_inobservations = 0;
  initialized_mindamp = 0.0;
  initialized_maxdamp = 0.0;
  initialized_cf_max = 0.0;
  initialized_cf_min = 0.0;
  
  is_reset = false;
  reseted_inobservations = 0;
  reseted_agc_factor = 0;

  addControls();
}


AimPZFC::~AimPZFC()
{
  ctrl_pole_damping_ = getctrl("mrs_real/pole_damping");
  ctrl_zero_damping_ = getctrl("mrs_real/zero_damping");
  ctrl_zero_factor_ = getctrl("mrs_real/zero_factor");
  ctrl_step_factor_ = getctrl("mrs_real/step_factor");
  ctrl_bandwidth_over_cf_ = getctrl("mrs_real/bandwidth_over_cf");
  ctrl_min_bandwidth_hz_ = getctrl("mrs_real/min_bandwidth_hz");
  ctrl_agc_factor_ = getctrl("mrs_real/agc_factor");
  ctrl_cf_max_ = getctrl("mrs_real/cf_max");
  ctrl_cf_min_ = getctrl("mrs_real/cf_min");
  ctrl_mindamp_ = getctrl("mrs_real/mindamp");
  ctrl_maxdamp_ = getctrl("mrs_real/maxdamp");
  ctrl_do_agc_step_ = getctrl("mrs_bool/do_agc_step");
  ctrl_use_fit_ = getctrl("mrs_bool/do_use_fit");
}


MarSystem*
AimPZFC::clone() const
{
  return new AimPZFC(*this);
}

void 
AimPZFC::addControls()
{
  addControl("mrs_real/pole_damping", 0.12f , ctrl_pole_damping_);
  addControl("mrs_real/zero_damping", 0.2f , ctrl_zero_damping_);
  addControl("mrs_real/zero_factor", 1.4f , ctrl_zero_factor_);
  addControl("mrs_real/step_factor", 1.0f/3.0f , ctrl_step_factor_);
  addControl("mrs_real/bandwidth_over_cf", 0.11f , ctrl_bandwidth_over_cf_);
  addControl("mrs_real/min_bandwidth_hz", 27.0f , ctrl_min_bandwidth_hz_);
  addControl("mrs_real/agc_factor", 12.0f, ctrl_agc_factor_);
  addControl("mrs_real/cf_max", 6000.0f, ctrl_cf_max_);
  addControl("mrs_real/cf_min", 100.0f, ctrl_cf_min_);
  addControl("mrs_real/mindamp", 0.18f , ctrl_mindamp_);
  addControl("mrs_real/maxdamp", 0.4f , ctrl_maxdamp_);
  addControl("mrs_bool/do_agc_step", true , ctrl_do_agc_step_);
  addControl("mrs_bool/use_fit", false , ctrl_use_fit_);
}

void
AimPZFC::myUpdate(MarControlPtr sender)
{

  (void) sender;
  MRSDIAG("AimPZFC.cpp - AimPZFC:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(channel_count_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimPZFC_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  //
  // Does the MarSystem need initialization?
  //
  if (initialized_israte != ctrl_israte_->to<mrs_real>() ||
      initialized_inobservations != ctrl_inObservations_->to<mrs_natural>() ||
      initialized_mindamp != ctrl_mindamp_->to<mrs_real>() ||
      initialized_maxdamp != ctrl_maxdamp_->to<mrs_real>() ||
      initialized_cf_max != ctrl_cf_max_->to<mrs_real>() ||
      initialized_cf_min != ctrl_cf_min_->to<mrs_real>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_israte = ctrl_israte_->to<mrs_real>();
    initialized_inobservations = ctrl_inObservations_->to<mrs_natural>();
    initialized_mindamp = ctrl_mindamp_->to<mrs_real>();
    initialized_maxdamp = ctrl_maxdamp_->to<mrs_real>();
    initialized_cf_max = ctrl_cf_max_->to<mrs_real>();
    initialized_cf_min = ctrl_cf_min_->to<mrs_real>();
  }

  //
  // Does the MarSystem need a reset?
  //
  if (reseted_inobservations != ctrl_inObservations_->to<mrs_natural>() ||
      reseted_agc_factor != ctrl_agc_factor_->to<mrs_real>()) {
    is_reset = false;
  }

  if (!is_reset) {
    ResetInternal();
    is_reset = true;
    reseted_inobservations = ctrl_inObservations_->to<mrs_natural>();
    reseted_agc_factor != ctrl_agc_factor_->to<mrs_real>();
  }

}

bool
AimPZFC::InitializeInternal() {
  channel_count_ = 0;
  SetPZBankCoeffs();
  return true;
}

void
AimPZFC::ResetInternal() {
  // These buffers may be actively modified by the algorithm
  agc_state_.clear();
  agc_state_.resize(channel_count_);
  for (int i = 0; i < channel_count_; ++i) {
    agc_state_[i].clear();
    agc_state_[i].resize(agc_stage_count_, 0.0f);
  }

  state_1_.clear();
  state_1_.resize(channel_count_, 0.0f);

  state_2_.clear();
  state_2_.resize(channel_count_, 0.0f);

  previous_out_.clear();
  previous_out_.resize(channel_count_, 0.0f);

  pole_damps_mod_.clear();
  pole_damps_mod_.resize(channel_count_, 0.0f);

  inputs_.clear();
  inputs_.resize(channel_count_, 0.0f);

  // Init AGC
  AGCDampStep();
  // pole_damps_mod_ and agc_state_ are now be initialized

  // Modify the pole dampings and AGC state slightly from their values in
  // silence in case the input is abuptly loud.
  for (int i = 0; i < channel_count_; ++i) {
    pole_damps_mod_[i] += 0.05f;
    for (int j = 0; j < agc_stage_count_; ++j)
      agc_state_[i][j] += 0.05f;
  }

  last_input_ = 0.0f;
}

bool
AimPZFC::SetPZBankCoeffs() {
  /*! \todo Re-implement the alternative parameter settings
   */
  if (ctrl_use_fit_->to<mrs_bool>()) {
    if (!SetPZBankCoeffsERBFitted())
      return false;
  } else {
    if (!SetPZBankCoeffsOrig())
      return false;
  }

  float mindamp = ctrl_mindamp_->to<mrs_real>();
  float maxdamp = ctrl_maxdamp_->to<mrs_real>();

  rmin_.resize(channel_count_);
  rmax_.resize(channel_count_);
  xmin_.resize(channel_count_);
  xmax_.resize(channel_count_);

  for (int c = 0; c < channel_count_; ++c) {
    // Calculate maximum and minimum damping options
    rmin_[c] = exp(-mindamp * pole_frequencies_[c]);
    rmax_[c] = exp(-maxdamp * pole_frequencies_[c]);

    xmin_[c] = rmin_[c] * cos(pole_frequencies_[c]
                                      * pow((1-pow(mindamp, 2)), 0.5f));
    xmax_[c] = rmax_[c] * cos(pole_frequencies_[c]
                                      * pow((1-pow(maxdamp, 2)), 0.5f));

  }

  // Set up AGC parameters
  agc_stage_count_ = 4;
  agc_epsilons_.resize(agc_stage_count_);
  agc_epsilons_[0] = 0.0064f;
  agc_epsilons_[1] = 0.0016f;
  agc_epsilons_[2] = 0.0004f;
  agc_epsilons_[3] = 0.0001f;

  agc_gains_.resize(agc_stage_count_);
  agc_gains_[0] = 1.0f;
  agc_gains_[1] = 1.4f;
  agc_gains_[2] = 2.0f;
  agc_gains_[3] = 2.8f;

  float mean_agc_gain = 0.0f;
  for (int c = 0; c < agc_stage_count_; ++c)
    mean_agc_gain += agc_gains_[c];
  mean_agc_gain /= static_cast<float>(agc_stage_count_);

  for (int c = 0; c < agc_stage_count_; ++c)
    agc_gains_[c] /= mean_agc_gain;

  return true;
}

bool 
AimPZFC::SetPZBankCoeffsOrig() {
  // This function sets the following variables:
  // channel_count_
  // pole_dampings_
  // pole_frequencies_
  // za0_, za1_, za2
  // output_

  double sample_rate = getctrl("mrs_real/israte")->to<mrs_real>();
  double cf_max = getctrl("mrs_real/cf_max")->to<mrs_real>();
  double cf_min = getctrl("mrs_real/cf_min")->to<mrs_real>();
  double bandwidth_over_cf = getctrl("mrs_real/bandwidth_over_cf")->to<mrs_real>();
  double min_bandwidth_hz = getctrl("mrs_real/min_bandwidth_hz")->to<mrs_real>();
  double step_factor = getctrl("mrs_real/step_factor")->to<mrs_real>();
  double pole_damping = getctrl("mrs_real/pole_damping")->to<mrs_real>();
  double zero_factor = getctrl("mrs_real/zero_factor")->to<mrs_real>();
  double zero_damping = getctrl("mrs_real/zero_damping")->to<mrs_real>();

  // TODO(tomwalters): There's significant code-duplication between this function
  // and SetPZBankCoeffsERBFitted, and SetPZBankCoeffs

  // Normalised maximum pole frequency
  float pole_frequency = cf_max / sample_rate * (2.0f * M_PI);
  channel_count_ = 0;
  while ((pole_frequency / (2.0f * M_PI)) * sample_rate > cf_min) {
    float bw = bandwidth_over_cf * pole_frequency + 2 * M_PI * min_bandwidth_hz / sample_rate;
    pole_frequency -= step_factor * bw;
    channel_count_++;
  }

  // sness - Probably should just set this in myUpdate, but want to
  // make sure it gets updated correctly.
  ctrl_onObservations_->setValue(channel_count_, NOUPDATE);

  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, pole_damping);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0f);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0f);
  za1_.clear();
  za1_.resize(channel_count_, 0.0f);
  za2_.clear();
  za2_.resize(channel_count_, 0.0f);

  // The output signal bank
  // output_.Initialize(channel_count_, buffer_length_, sample_rate);

  // Reset the pole frequency to maximum
  pole_frequency = cf_max / sample_rate * (2.0f * PI);
  // cout << "pole_frequency=" << pole_frequency << endl;

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    // output_.set_centre_frequency(i, frequency);

    float zero_frequency = Minimum(M_PI, zero_factor * pole_frequency);

    // Impulse-invariance mapping
    float z_plane_theta = zero_frequency * sqrt(1.0f - pow(zero_damping, 2));
    float z_plane_rho = exp(-zero_damping * zero_frequency);

    // Direct-form coefficients from z-plane rho and theta
    float a1 = -2.0f * z_plane_rho * cos(z_plane_theta);
    float a2 = z_plane_rho * z_plane_rho;

    // Normalised to unity gain at DC
    float a_sum = 1.0f + a1 + a2;
    za0_[i] = 1.0f / a_sum;
    za1_[i] = a1 / a_sum;
    za2_[i] = a2 / a_sum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
    float bw = bandwidth_over_cf * pole_frequency + 2 * M_PI * min_bandwidth_hz / sample_rate;
    pole_frequency -= step_factor * bw;
  }
  return true;
}


// bool 
// AimPZFC::SetPZBankCoeffsERB() {
//   // This function sets the following variables:
//   // channel_count_
//   // pole_dampings_
//   // pole_frequencies_
//   // za0_, za1_, za2
//   // output_

//   double sample_rate = getctrl("mrs_real/israte")->to<mrs_real>();
//   double cf_max = getctrl("mrs_real/cf_max")->to<mrs_real>();
//   double cf_min = getctrl("mrs_real/cf_min")->to<mrs_real>();
//   double bandwidth_over_cf = getctrl("mrs_real/bandwidth_over_cf")->to<mrs_real>();
//   double min_bandwidth_hz = getctrl("mrs_real/min_bandwidth_hz")->to<mrs_real>();
//   double step_factor = getctrl("mrs_real/step_factor")->to<mrs_real>();
//   double pole_damping = getctrl("mrs_real/pole_damping")->to<mrs_real>();
//   double zero_factor = getctrl("mrs_real/zero_factor")->to<mrs_real>();
//   double zero_damping = getctrl("mrs_real/zero_damping")->to<mrs_real>();

//   // TODO(tomwalters): There's significant code-duplication between here,
//   // SetPZBankCoeffsERBFitted, and SetPZBankCoeffs

//   // Normalised maximum pole frequency
//   float pole_frequency = cf_max / sample_rate * (2.0f * M_PI);
//   channel_count_ = 0;
//   while ((pole_frequency / (2.0f * M_PI)) * sample_rate > cf_min) {
//     float bw = ERBTools::Freq2ERBw(pole_frequency
//                                   / (2.0f * M_PI) * sample_rate);
//     pole_frequency -= step_factor * (bw * (2.0f * M_PI) / sample_rate);
//     channel_count_++;
//   }

//   // Now the number of channels is known, various buffers for the filterbank
//   // coefficients can be initialised
//   pole_dampings_.clear();
//   pole_dampings_.resize(channel_count_, pole_damping);
//   pole_frequencies_.clear();
//   pole_frequencies_.resize(channel_count_, 0.0f);

//   // Direct-form coefficients
//   za0_.clear();
//   za0_.resize(channel_count_, 0.0f);
//   za1_.clear();
//   za1_.resize(channel_count_, 0.0f);
//   za2_.clear();
//   za2_.resize(channel_count_, 0.0f);

//   // The output signal bank
//   // output_.Initialize(channel_count_, buffer_length_, sample_rate);

//   // Reset the pole frequency to maximum
//   pole_frequency = cf_max / sample_rate * (2.0f * M_PI);

//   for (int i = channel_count_ - 1; i > -1; --i) {
//     // Store the normalised pole frequncy
//     pole_frequencies_[i] = pole_frequency;

//     // Calculate the real pole frequency from the normalised pole frequency
//     float frequency = pole_frequency / (2.0f * M_PI) * sample_rate;

//     // Store the real pole frequency as the 'centre frequency' of the filterbank
//     // channel
//     // output_.set_centre_frequency(i, frequency);

//     float zero_frequency = Minimum(M_PI, zero_factor * pole_frequency);

//     // Impulse-invariance mapping
//     float z_plane_theta = zero_frequency * sqrt(1.0f - pow(zero_damping, 2));
//     float z_plane_rho = exp(-zero_damping * zero_frequency);

//     // Direct-form coefficients from z-plane rho and theta
//     float a1 = -2.0f * z_plane_rho * cos(z_plane_theta);
//     float a2 = z_plane_rho * z_plane_rho;

//     // Normalised to unity gain at DC
//     float a_sum = 1.0f + a1 + a2;
//     za0_[i] = 1.0f / a_sum;
//     za1_[i] = a1 / a_sum;
//     za2_[i] = a2 / a_sum;

//     float bw = ERBTools::Freq2ERBw(pole_frequency
//                                   / (2.0f * M_PI) * sample_rate);
//     pole_frequency -= step_factor * (bw * (2.0f * M_PI) / sample_rate);
//   }
//   return true;
// }

bool 
AimPZFC::SetPZBankCoeffsERBFitted() {
  //float parameter_values[3 * 7] = {
    //// Filed, Nfit = 524, 11-3 parameters, PZFC, cwt 0, fit time 9915 sec
    //1.14827,   0.00000,   0.00000,  // % SumSqrErr=  10125.41
    //0.53571,  -0.70128,   0.63246,  // % RMSErr   =   2.81586
    //0.76779,   0.00000,   0.00000,  // % MeanErr  =   0.00000
    //// Inf   0.00000   0.00000 % RMSCost  = NaN
    //0.00000,   0.00000,   0.00000,
    //6.00000,   0.00000,   0.00000,
    //1.08869,  -0.09470,   0.07844,
    //10.56432,   2.52732,   1.86895
    //// -3.45865  -1.31457   3.91779 % Kv
  //};

  float parameter_values[3 * 7] = {
  // Fit 515 from Dick
  // Final, Nfit = 515, 9-3 parameters, PZFC, cwt 0
     1.72861,   0.00000,   0.00000,  // SumSqrErr =  13622.24
     0.56657,  -0.93911,   0.89163,  // RMSErr    =  3.26610
     0.39469,   0.00000,   0.00000,  // MeanErr   =  0.00000
  // Inf,       0.00000,   0.00000,  // RMSCost   =  NaN - would set coefc to infinity, but this isn't passed on
     0.00000,   0.00000,   0.00000,
     2.00000,   0.00000,   0.00000,  //
     1.27393,   0.00000,   0.00000,
    11.46247,  5.46894,   0.11800
  // -4.15525,  1.54874,   2.99858   // Kv
  };

  double sample_rate = getctrl("mrs_real/israte")->to<mrs_real>();
  double cf_max = getctrl("mrs_real/cf_max")->to<mrs_real>();
  double cf_min = getctrl("mrs_real/cf_min")->to<mrs_real>();

  // Precalculate the number of channels required - this method is ugly but it
  // was the quickest way of converting from MATLAB as the step factor between
  // channels can vary quadratically with pole frequency...

  // Normalised maximum pole frequency
  float pole_frequency = cf_max / sample_rate * (2.0f * M_PI);

  channel_count_ = 0;
  while ((pole_frequency / (2.0f * M_PI)) * sample_rate > cf_min) {
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate;
    float f_dep = ERBTools::Freq2ERB(frequency)
                  / ERBTools::Freq2ERB(1000.0f) - 1.0f;
    float bw = ERBTools::Freq2ERBw(pole_frequency
                                  / (2.0f * M_PI) * sample_rate);
    float step_factor = 1.0f
      / (parameter_values[4*3] + parameter_values[4 * 3 + 1]
      * f_dep + parameter_values[4 * 3 + 2] * f_dep * f_dep);  // 1/n2
    pole_frequency -= step_factor * (bw * (2.0f * M_PI) / sample_rate);
    channel_count_++;
  }

  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, 0.0f);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0f);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0f);
  za1_.clear();
  za1_.resize(channel_count_, 0.0f);
  za2_.clear();
  za2_.resize(channel_count_, 0.0f);

  // The output signal bank
  // output_.Initialize(channel_count_, buffer_length_, sample_rate);

  // Reset the pole frequency to maximum
  pole_frequency = cf_max / sample_rate * (2.0f * M_PI);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    // output_.set_centre_frequency(i, frequency);

    // From PZFC_Small_Signal_Params.m { From PZFC_Params.m {
    float DpndF = ERBTools::Freq2ERB(frequency)
                  / ERBTools::Freq2ERB(1000.0f) - 1.0f;

    float p[8];  // Parameters (short name for ease of reading)

    // Use parameter_values to recover the parameter values for this frequency
    for (int param = 0; param < 7; ++param)
      p[param] = parameter_values[param * 3]
                 + parameter_values[param * 3 + 1] * DpndF
                 + parameter_values[param * 3 + 2] * DpndF * DpndF;

    // Calculate the final parameter
    p[7] = p[1] * pow(10.0f, (p[2] / (p[1] * p[4])) * (p[6] - 60.0f) / 20.0f);
    if (p[7] < 0.2f)
      p[7] = 0.2f;

    // Nominal bandwidth at this frequency
    float fERBw = ERBTools::Freq2ERBw(frequency);

    // Pole bandwidth
    float fPBW = ((p[7] * fERBw * (2 * M_PI) / sample_rate) / 2)
                 * pow(p[4], 0.5f);

    // Pole damping
    float pole_damping = fPBW / sqrt(pow(pole_frequency, 2) + pow(fPBW, 2));

    // Store the pole damping
    pole_dampings_[i] = pole_damping;

    // Zero bandwidth
    float fZBW = ((p[0] * p[5] * fERBw * (2 * M_PI) / sample_rate) / 2)
                 * pow(p[4], 0.5f);

    // Zero frequency
    float zero_frequency = p[5] * pole_frequency;

    if (zero_frequency > M_PI) {
      MRSWARN("Warning: Zero frequency is above the Nyquist frequency.");
      MRSWARN("Continuing anyway but results may not be accurate.");
    }
      // LOG_ERROR(_T("Warning: Zero frequency is above the Nyquist frequency "
      //              "in ModulePZFC(), continuing anyway but results may not "
      //              "be accurate."));

    // Zero damping
    float fZDamp = fZBW / sqrt(pow(zero_frequency, 2) + pow(fZBW, 2));

    // Impulse-invariance mapping
    float fZTheta = zero_frequency * sqrt(1.0f - pow(fZDamp, 2));
    float fZRho = exp(-fZDamp * zero_frequency);

    // Direct-form coefficients
    float fA1 = -2.0f * fZRho * cos(fZTheta);
    float fA2 = fZRho * fZRho;

    // Normalised to unity gain at DC
    float fASum = 1.0f + fA1 + fA2;
    za0_[i] = 1.0f / fASum;
    za1_[i] = fA1 / fASum;
    za2_[i] = fA2 / fASum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
    pole_frequency -= ((1.0f / p[4])
                       * (fERBw * (2.0f * M_PI) / sample_rate));
  }
return true;
}

void
AimPZFC::AGCDampStep() {
  if (detect_.size() == 0) {
    // If  detect_ is not initialised, it means that the AGC is not set up.
    // Set up now.
    /*! \todo Make a separate InitAGC function which does this.
     */
    detect_.clear();
    float detect_zero = DetectFun(0.0f);
    detect_.resize(channel_count_, detect_zero);

    for (int c = 0; c < channel_count_; c++)
      for (int st = 0; st < agc_stage_count_; st++)
        agc_state_[c][st] = (1.2f * detect_[c] * agc_gains_[st]);
  }

  float fAGCEpsLeft = 0.3f;
  float fAGCEpsRight = 0.3f;

  for (int c = channel_count_ - 1; c > -1; --c) {
    for (int st = 0; st < agc_stage_count_; ++st) {
      // This bounds checking is ugly and wasteful, and in an inner loop.
      // If this algorithm is slow, this is why!
      /*! \todo Proper non-ugly bounds checking in AGCDampStep()
       */
      float fPrevAGCState;
      float fCurrAGCState;
      float fNextAGCState;

      if (c < channel_count_ - 1)
        fPrevAGCState = agc_state_[c + 1][st];
      else
        fPrevAGCState = agc_state_[c][st];

      fCurrAGCState = agc_state_[c][st];

      if (c > 0)
        fNextAGCState = agc_state_[c - 1][st];
      else
        fNextAGCState = agc_state_[c][st];

      // Spatial smoothing
      /*! \todo Something odd is going on here
       *  I think this line is not quite right.
       */
      float agc_avg = fAGCEpsLeft * fPrevAGCState
                      + (1.0f - fAGCEpsLeft - fAGCEpsRight) * fCurrAGCState
                      + fAGCEpsRight * fNextAGCState;
      // Temporal smoothing
      agc_state_[c][st] = agc_avg * (1.0f - agc_epsilons_[st])
                          + agc_epsilons_[st] * detect_[c] * agc_gains_[st];
    }
  }

  // float offset = 1.0f - agc_factor_ * DetectFun(0.0f);
  float offset = 1.0f - ctrl_agc_factor_->to<mrs_real>() * DetectFun(0.0f);

  for (int i = 0; i < channel_count_; ++i) {
    float fAGCStateMean = 0.0f;
    for (int j = 0; j < agc_stage_count_; ++j)
      fAGCStateMean += agc_state_[i][j];

    fAGCStateMean /= static_cast<float>(agc_stage_count_);

    pole_damps_mod_[i] = pole_dampings_[i] *
                         (offset + ctrl_agc_factor_->to<mrs_real>() * fAGCStateMean);
  }
}

float
AimPZFC::DetectFun(float fIN) {
  if (fIN < 0.0f)
    fIN = 0.0f;
  float fDetect = Minimum(1.0f, fIN);
  float fA = 0.25f;
  return fA * fIN + (1.0f - fA) * (fDetect - pow(fDetect, 3) / 3.0f);
}

inline float AimPZFC::Minimum(float a, float b) {
  if (a < b)
    return a;
  else
    return b;
}


void
AimPZFC::myProcess(realvec& in, realvec& out)
{
  // cout << "ctrl_inSamples_->to<mrs_natural>()" << ctrl_inSamples_->to<mrs_natural>() << endl;
  for (t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {

    float input_sample = in(0, t);
    // cout << "input_sample=" << in(0, t) << endl;

    // Lowpass filter the input with a zero at PI
    input_sample = 0.5f * input_sample + 0.5f * last_input_;
    last_input_ = in(0, t);

    inputs_[channel_count_ - 1] = input_sample;
    for (int c = 0; c < channel_count_ - 1; ++c) {
      inputs_[c] = previous_out_[c + 1];
      // cout << "inputs_[" << c << "]=" << inputs_[c] << endl;
    }

    // PZBankStep2
    // to save a bunch of divides

    float damp_rate = 1.0f / (getctrl("mrs_real/maxdamp")->to<mrs_real>() - getctrl("mrs_real/mindamp")->to<mrs_real>());
    // cout << "damp_rate=" << damp_rate << endl;

    for (int c = channel_count_ - 1; c > -1; --c) {
      float interp_factor = (pole_damps_mod_[c] - getctrl("mrs_real/mindamp")->to<mrs_real>()) * damp_rate;

      // cout << "xmin_[c]=" << xmin_[c] << endl;
      // cout << "xmax_[c]=" << xmax_[c] << endl;
      // cout << "rmin_[c]=" << rmin_[c] << endl;
      // cout << "rmax_[c]=" << rmax_[c] << endl;

      float x = xmin_[c] + (xmax_[c] - xmin_[c]) * interp_factor;
      float r = rmin_[c] + (rmax_[c] - rmin_[c]) * interp_factor;

      // cout << "x=" << x << endl;
      // cout << "r=" << r << endl;
      // optional improvement to constellation adds a bit to r
      float fd = pole_frequencies_[c] * pole_damps_mod_[c];
      // quadratic for small values, then linear
      r = r + 0.25f * fd * Minimum(0.05f, fd);

      float zb1 = -2.0f * x;
      float zb2 = r * r;


      // canonic poles but with input provided where unity DC gain is
      // assured (mean value of state is always equal to mean value
      // of input)
      float new_state = inputs_[c] - (state_1_[c] - inputs_[c]) * zb1
                                   - (state_2_[c] - inputs_[c]) * zb2;
      // float new_state = inputs_[c] - (state_1_[c] - inputs_[c]) * zb1
      //                   - (state_2_[c] - inputs_[c]) * zb2;

      // cout << "zb1=" << zb1 << endl;
      // cout << "zb2=" << zb2 << endl;
      // cout << "inputs_[c]=" << inputs_[c] << endl;
      // cout << "state_1_[c]=" << state_1_[c] << endl;
      // cout << "state_2_[c]=" << state_2_[c] << endl;
      // cout << "new_state=" << new_state << endl;

      // canonic zeros part as before:
      float output = za0_[c] * new_state + za1_[c] * state_1_[c]
                     + za2_[c] * state_2_[c];

      // cubic compression nonlinearity
      output -= 0.0001f * pow(output, 3);
      // cout << "output=" << output << endl;

      out(c, t) = output;
      detect_[c] = DetectFun(output);
      state_2_[c] = state_1_[c];
      state_1_[c] = new_state;
    }

    if (getctrl("mrs_bool/do_agc_step")->to<mrs_bool>())
      AGCDampStep();

    for (int c = 0; c < channel_count_; ++c)
      previous_out_[c] = out(c,t);
  }

}
