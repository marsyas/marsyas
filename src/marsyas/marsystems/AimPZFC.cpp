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

#include "AimPZFC.h"
#include "../common_source.h"
#include "../ERBTools.h"

using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

AimPZFC::AimPZFC(mrs_string name):MarSystem("AimPZFC",name)
{
  is_initialized = false;
  initialized_israte = 0.0;
  initialized_inobservations = 0;
  initialized_mindamp = 0.0;
  initialized_maxdamp = 0.0;
  initialized_cf_max = 0.0;
  initialized_cf_min = 0.0;
  channel_count_ = 1;

  is_reset = false;
  reseted_inobservations = 0;
  reseted_agc_factor = 0;

  addControls();
}

AimPZFC::AimPZFC(const AimPZFC& a): MarSystem(a)
{
  is_initialized = false;
  initialized_israte = 0.0;
  initialized_inobservations = 0;
  initialized_mindamp = 0.0;
  initialized_maxdamp = 0.0;
  initialized_cf_max = 0.0;
  initialized_cf_min = 0.0;


  channel_count_ = 1;

  is_reset = false;
  reseted_inobservations = 0;
  reseted_agc_factor = 0;

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
  ctrl_use_fit_ = getctrl("mrs_bool/use_fit");

}


AimPZFC::~AimPZFC()
{
}


MarSystem*
AimPZFC::clone() const
{
  return new AimPZFC(*this);
}

void
AimPZFC::addControls()
{
  addControl("mrs_real/pole_damping", 0.12 , ctrl_pole_damping_);
  addControl("mrs_real/zero_damping", 0.2 , ctrl_zero_damping_);
  addControl("mrs_real/zero_factor", 1.4 , ctrl_zero_factor_);
  addControl("mrs_real/step_factor", 1.0/3.0 , ctrl_step_factor_);
  addControl("mrs_real/bandwidth_over_cf", 0.11 , ctrl_bandwidth_over_cf_);
  addControl("mrs_real/min_bandwidth_hz", 27.0 , ctrl_min_bandwidth_hz_);
  addControl("mrs_real/agc_factor", 12.0, ctrl_agc_factor_);
  addControl("mrs_real/cf_max", 6000.0, ctrl_cf_max_);
  addControl("mrs_real/cf_min", 100.0, ctrl_cf_min_);
  addControl("mrs_real/mindamp", 0.18 , ctrl_mindamp_);
  addControl("mrs_real/maxdamp", 0.4 , ctrl_maxdamp_);
  addControl("mrs_bool/do_agc_step", true , ctrl_do_agc_step_);
  addControl("mrs_bool/use_fit", false , ctrl_use_fit_);
}

void
AimPZFC::myUpdate(MarControlPtr sender)
{

  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AimPZFC.cpp - AimPZFC:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimPZFC_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  // Need to have double the amount of channels, the first set of
  // channels are for the signals the second set of channels are for
  // the centre frequencies
  ctrl_onObservations_->setValue(channel_count_ * 2, NOUPDATE);

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
    reseted_agc_factor = (mrs_natural) ctrl_agc_factor_->to<mrs_real>();
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
  // cout << "AimPZFC::ResetInternal" << endl;

  // These buffers may be actively modified by the algorithm
  agc_state_.clear();
  agc_state_.resize(channel_count_);
  for (int i = 0; i < channel_count_; ++i) {
    agc_state_[i].clear();
    agc_state_[i].resize(agc_stage_count_, 0.0);
  }

  state_1_.clear();
  state_1_.resize(channel_count_, 0.0);

  state_2_.clear();
  state_2_.resize(channel_count_, 0.0);

  previous_out_.clear();
  previous_out_.resize(channel_count_, 0.0);

  pole_damps_mod_.clear();
  pole_damps_mod_.resize(channel_count_, 0.0);

  inputs_.clear();
  inputs_.resize(channel_count_, 0.0);

  // Init AGC
  offset_ = 1.0 - ctrl_agc_factor_->to<mrs_real>() * DetectFun(0.0);
  agc_factor_ = ctrl_agc_factor_->to<mrs_real>();
  AGCDampStep();
  // pole_damps_mod_ and agc_state_ are now be initialized

  // Modify the pole dampings and AGC state slightly from their values in
  // silence in case the input is abuptly loud.
  for (int i = 0; i < channel_count_; ++i) {
    pole_damps_mod_[i] += 0.05;
    for (int j = 0; j < agc_stage_count_; ++j)
      agc_state_[i][j] += 0.05;
  }

  last_input_ = 0.0;
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

  double mindamp = ctrl_mindamp_->to<mrs_real>();
  double maxdamp = ctrl_maxdamp_->to<mrs_real>();

  rmin_.resize(channel_count_);
  rmax_.resize(channel_count_);
  xmin_.resize(channel_count_);
  xmax_.resize(channel_count_);

  for (int c = 0; c < channel_count_; ++c) {
    // Calculate maximum and minimum damping options
    rmin_[c] = exp(-mindamp * pole_frequencies_[c]);
    rmax_[c] = exp(-maxdamp * pole_frequencies_[c]);

    xmin_[c] = rmin_[c] * cos(pole_frequencies_[c]
                              * pow((1-pow(mindamp, 2)), 0.5));
    xmax_[c] = rmax_[c] * cos(pole_frequencies_[c]
                              * pow((1-pow(maxdamp, 2)), 0.5));

  }

  // Set up AGC parameters
  agc_stage_count_ = 4;
  agc_epsilons_.resize(agc_stage_count_);
  agc_epsilons_[0] = 0.0064;
  agc_epsilons_[1] = 0.0016;
  agc_epsilons_[2] = 0.0004;
  agc_epsilons_[3] = 0.0001;

  agc_gains_.resize(agc_stage_count_);
  agc_gains_[0] = 1.0;
  agc_gains_[1] = 1.4;
  agc_gains_[2] = 2.0;
  agc_gains_[3] = 2.8;

  double mean_agc_gain = 0.0;
  for (int c = 0; c < agc_stage_count_; ++c)
    mean_agc_gain += agc_gains_[c];
  mean_agc_gain /= static_cast<double>(agc_stage_count_);

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
  double pole_frequency = cf_max / sample_rate * (2.0 * PI);
  channel_count_ = 0;
  while ((pole_frequency / (2.0 * PI)) * sample_rate > cf_min) {
    double bw = bandwidth_over_cf * pole_frequency + 2 * PI * min_bandwidth_hz / sample_rate;
    pole_frequency -= step_factor * bw;
    channel_count_++;
  }



  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, pole_damping);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0);
  za1_.clear();
  za1_.resize(channel_count_, 0.0);
  za2_.clear();
  za2_.resize(channel_count_, 0.0);

  // The output signal bank
  // output_.Initialize(channel_count_, buffer_length_, sample_rate);

  // cout.precision(20);

  // cout << "********** 1/3=" << 1.0/3.0 << endl;

  // Reset the pole frequency to maximum
  pole_frequency = cf_max / sample_rate * (2.0 * PI);
  // cout << "cf_max=" << cf_max << endl;
  // cout << "sample_rate=" << sample_rate << endl;
  // cout << "pole_frequency=" << pole_frequency << endl;

  centre_frequencies_.clear();
  centre_frequencies_.resize(channel_count_);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // cout << "i=" << i << endl;

    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    double frequency = pole_frequency / (2.0 * PI) * sample_rate;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    centre_frequencies_[i] = frequency;
    // output_.set_centre_frequency(i, frequency);

    double zero_frequency = Minimum(PI, zero_factor * pole_frequency);
    // cout << "\tzero_frequency=" << zero_frequency << endl;

    // Impulse-invariance mapping
    double z_plane_theta = zero_frequency * sqrt(1.0 - pow(zero_damping, 2));
    double z_plane_rho = exp(-zero_damping * zero_frequency);
    // cout << "\tz_plane_theta=" << z_plane_theta << endl;
    // cout << "\tz_plane_rho=" << z_plane_rho << endl;

    // Direct-form coefficients from z-plane rho and theta
    double a1 = -2.0 * z_plane_rho * cos(z_plane_theta);
    double a2 = z_plane_rho * z_plane_rho;

    // Normalised to unity gain at DC
    double a_sum = 1.0 + a1 + a2;
    // cout << "\ta1=" << a1 << endl;
    // cout << "\ta2=" << a2 << endl;
    // cout << "\ta_sum=" << a_sum << endl;

    za0_[i] = 1.0 / a_sum;
    za1_[i] = a1 / a_sum;
    za2_[i] = a2 / a_sum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
    double bw = bandwidth_over_cf * pole_frequency + 2 * PI * min_bandwidth_hz / sample_rate;
    // cout << "\tmin_bandwidth_hz_=" << min_bandwidth_hz << endl;
    // cout << "\tbw=" << bw << endl;
    // cout << "\tstep_factor=" << step_factor << endl;
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
//   double pole_frequency = cf_max / sample_rate * (2.0 * PI);
//   channel_count_ = 0;
//   while ((pole_frequency / (2.0 * PI)) * sample_rate > cf_min) {
//     double bw = ERBTools::Freq2ERBw(pole_frequency
//                                   / (2.0 * PI) * sample_rate);
//     pole_frequency -= step_factor * (bw * (2.0 * PI) / sample_rate);
//     channel_count_++;
//   }

//   // Now the number of channels is known, various buffers for the filterbank
//   // coefficients can be initialised
//   pole_dampings_.clear();
//   pole_dampings_.resize(channel_count_, pole_damping);
//   pole_frequencies_.clear();
//   pole_frequencies_.resize(channel_count_, 0.0);

//   // Direct-form coefficients
//   za0_.clear();
//   za0_.resize(channel_count_, 0.0);
//   za1_.clear();
//   za1_.resize(channel_count_, 0.0);
//   za2_.clear();
//   za2_.resize(channel_count_, 0.0);

//   // The output signal bank
//   // output_.Initialize(channel_count_, buffer_length_, sample_rate);

//   // Reset the pole frequency to maximum
//   pole_frequency = cf_max / sample_rate * (2.0 * PI);

//   for (int i = channel_count_ - 1; i > -1; --i) {
//     // Store the normalised pole frequncy
//     pole_frequencies_[i] = pole_frequency;

//     // Calculate the real pole frequency from the normalised pole frequency
//     double frequency = pole_frequency / (2.0 * PI) * sample_rate;

//     // Store the real pole frequency as the 'centre frequency' of the filterbank
//     // channel
//     // output_.set_centre_frequency(i, frequency);

//     double zero_frequency = Minimum(PI, zero_factor * pole_frequency);

//     // Impulse-invariance mapping
//     double z_plane_theta = zero_frequency * sqrt(1.0 - pow(zero_damping, 2));
//     double z_plane_rho = exp(-zero_damping * zero_frequency);

//     // Direct-form coefficients from z-plane rho and theta
//     double a1 = -2.0 * z_plane_rho * cos(z_plane_theta);
//     double a2 = z_plane_rho * z_plane_rho;

//     // Normalised to unity gain at DC
//     double a_sum = 1.0 + a1 + a2;
//     za0_[i] = 1.0 / a_sum;
//     za1_[i] = a1 / a_sum;
//     za2_[i] = a2 / a_sum;

//     double bw = ERBTools::Freq2ERBw(pole_frequency
//                                   / (2.0 * PI) * sample_rate);
//     pole_frequency -= step_factor * (bw * (2.0 * PI) / sample_rate);
//   }
//   return true;
// }

bool
AimPZFC::SetPZBankCoeffsERBFitted() {
  // cout << "ModulePZFC::SetPZBankCoeffsERBFitted" << endl;
  // cout << "AimPZFC::SetPZBankCoeffsERBFitted" << endl;
  //double parameter_values[3 * 7] = {
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

  double parameter_values[3 * 7] = {
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
  double pole_frequency = cf_max / sample_rate * (2.0 * PI);

  channel_count_ = 0;
  while ((pole_frequency / (2.0 * PI)) * sample_rate > cf_min) {
    double frequency = pole_frequency / (2.0 * PI) * sample_rate;
    double f_dep = ERBTools::Freq2ERB(frequency)
                   / ERBTools::Freq2ERB(1000.0) - 1.0;
    double bw = ERBTools::Freq2ERBw(pole_frequency
                                    / (2.0 * PI) * sample_rate);
    double step_factor = 1.0
                         / (parameter_values[4*3] + parameter_values[4 * 3 + 1]
                            * f_dep + parameter_values[4 * 3 + 2] * f_dep * f_dep);  // 1/n2
    pole_frequency -= step_factor * (bw * (2.0 * PI) / sample_rate);
    channel_count_++;
  }

  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, 0.0);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0);
  za1_.clear();
  za1_.resize(channel_count_, 0.0);
  za2_.clear();
  za2_.resize(channel_count_, 0.0);

  // The output signal bank
  // output_.Initialize(channel_count_, buffer_length_, sample_rate);

  // Reset the pole frequency to maximum
  pole_frequency = cf_max / sample_rate * (2.0 * PI);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    double frequency = pole_frequency / (2.0 * PI) * sample_rate;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    // output_.set_centre_frequency(i, frequency);

    // From PZFC_Small_Signal_Params.m { From PZFC_Params.m {
    double DpndF = ERBTools::Freq2ERB(frequency)
                   / ERBTools::Freq2ERB(1000.0) - 1.0;

    double p[8];  // Parameters (short name for ease of reading)

    // Use parameter_values to recover the parameter values for this frequency
    for (int param = 0; param < 7; ++param)
      p[param] = parameter_values[param * 3]
                 + parameter_values[param * 3 + 1] * DpndF
                 + parameter_values[param * 3 + 2] * DpndF * DpndF;

    // Calculate the final parameter
    p[7] = p[1] * pow(10.0, (p[2] / (p[1] * p[4])) * (p[6] - 60.0) / 20.0);
    if (p[7] < 0.2)
      p[7] = 0.2;

    // Nominal bandwidth at this frequency
    double fERBw = ERBTools::Freq2ERBw(frequency);

    // Pole bandwidth
    double fPBW = ((p[7] * fERBw * (2 * PI) / sample_rate) / 2)
                  * pow(p[4], 0.5);

    // Pole damping
    double pole_damping = fPBW / sqrt(pow(pole_frequency, 2) + pow(fPBW, 2));

    cout << "pole_damping = " << pole_damping << endl;

    // Store the pole damping
    pole_dampings_[i] = pole_damping;

    // Zero bandwidth
    double fZBW = ((p[0] * p[5] * fERBw * (2 * PI) / sample_rate) / 2)
                  * pow(p[4], 0.5);

    // Zero frequency
    double zero_frequency = p[5] * pole_frequency;

    if (zero_frequency > PI) {
      MRSWARN("Warning: Zero frequency is above the Nyquist frequency.");
      MRSWARN("Continuing anyway but results may not be accurate.");
    }
    // LOG_ERROR(_T("Warning: Zero frequency is above the Nyquist frequency "
    //              "in ModulePZFC(), continuing anyway but results may not "
    //              "be accurate."));

    // Zero damping
    double fZDamp = fZBW / sqrt(pow(zero_frequency, 2) + pow(fZBW, 2));

    // Impulse-invariance mapping
    double fZTheta = zero_frequency * sqrt(1.0 - pow(fZDamp, 2));
    double fZRho = exp(-fZDamp * zero_frequency);

    // Direct-form coefficients
    double fA1 = -2.0 * fZRho * cos(fZTheta);
    double fA2 = fZRho * fZRho;

    // Normalised to unity gain at DC
    double fASum = 1.0 + fA1 + fA2;
    za0_[i] = 1.0 / fASum;
    za1_[i] = fA1 / fASum;
    za2_[i] = fA2 / fASum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
    pole_frequency -= ((1.0 / p[4])
                       * (fERBw * (2.0 * PI) / sample_rate));
  }
  return true;
}

void
AimPZFC::AGCDampStep() {
  // cout << "AimPZFC::AGCDampStep" << endl;
  if (detect_.size() == 0) {
    // If  detect_ is not initialised, it means that the AGC is not set up.
    // Set up now.
    /*! \todo Make a separate InitAGC function which does this.
     */
    detect_.clear();
    double detect_zero = DetectFun(0.0);
    detect_.resize(channel_count_, detect_zero);

    for (int c = 0; c < channel_count_; c++)
      for (int st = 0; st < agc_stage_count_; st++)
        agc_state_[c][st] = (1.2 * detect_[c] * agc_gains_[st]);
  }

  double fAGCEpsLeft = 0.3;
  double fAGCEpsRight = 0.3;

  for (int c = channel_count_ - 1; c > -1; --c) {
    for (int st = 0; st < agc_stage_count_; ++st) {
      // This bounds checking is ugly and wasteful, and in an inner loop.
      // If this algorithm is slow, this is why!
      /*! \todo Proper non-ugly bounds checking in AGCDampStep()
       */
      double fPrevAGCState;
      double fCurrAGCState;
      double fNextAGCState;

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
      double agc_avg = fAGCEpsLeft * fPrevAGCState
                       + (1.0 - fAGCEpsLeft - fAGCEpsRight) * fCurrAGCState
                       + fAGCEpsRight * fNextAGCState;
      // Temporal smoothing
      agc_state_[c][st] = agc_avg * (1.0 - agc_epsilons_[st])
                          + agc_epsilons_[st] * detect_[c] * agc_gains_[st];
    }
  }

  // double offset = 1.0 - agc_factor_ * DetectFun(0.0);


  for (int i = 0; i < channel_count_; ++i) {
    double fAGCStateMean = 0.0;
    for (int j = 0; j < agc_stage_count_; ++j)
      fAGCStateMean += agc_state_[i][j];

    fAGCStateMean /= static_cast<double>(agc_stage_count_);


    pole_damps_mod_[i] = pole_dampings_[i] *
                         (offset_ + agc_factor_ * fAGCStateMean);
  }
}

double
AimPZFC::DetectFun(double fIN)
{
  if (fIN < 0.0)
    fIN = 0.0;
  double fDetect = Minimum(1.0, fIN);
  double fA = 0.25;
  return fA * fIN + (1.0 - fA) * (fDetect - pow(fDetect, 3) / 3.0);
}

inline double AimPZFC::Minimum(double a, double b)
{
  if (a < b)
    return a;
  else
    return b;
}


void
AimPZFC::myProcess(realvec& in, realvec& out)
{
  double damp_rate = 1.0 / (ctrl_maxdamp_->to<mrs_real>() - ctrl_mindamp_->to<mrs_real>());
  double min_damp = ctrl_mindamp_->to<mrs_real>();
  double interp_factor;
  bool do_agc = ctrl_do_agc_step_->to<mrs_bool>();

  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    double input_sample = in(0, t);

    // Lowpass filter the input with a zero at PI
    input_sample = 0.5 * input_sample + 0.5 * last_input_;
    last_input_ = in(0, t);

    inputs_[channel_count_ - 1] = input_sample;
    for (int c = 0; c < channel_count_ - 1; ++c)
    {
      inputs_[c] = previous_out_[c + 1];

    }

    // PZBankStep2
    // to save a bunch of divides
    for (int c = channel_count_ - 1; c > -1; --c)
    {
      interp_factor = (pole_damps_mod_[c] -min_damp) * damp_rate;

      double x = xmin_[c] + (xmax_[c] - xmin_[c]) * interp_factor;
      double r = rmin_[c] + (rmax_[c] - rmin_[c]) * interp_factor;

      // optional improvement to constellation adds a bit to r
      double fd = pole_frequencies_[c] * pole_damps_mod_[c];
      // quadratic for small values, then linear
      r = r + 0.25 * fd * Minimum(0.05, fd);

      double zb1 = -2.0 * x;
      double zb2 = r * r;

      // canonic poles but with input provided where unity DC gain is
      // assured (mean value of state is always equal to mean value
      // of input)
      double new_state = inputs_[c] - (state_1_[c] - inputs_[c]) * zb1
                         - (state_2_[c] - inputs_[c]) * zb2;

      // canonic zeros part as before:
      // cout << "za0_[c]=" << za0_[c] << endl;
      // cout << "new_state=" << new_state << endl;
      // cout << "za1_[c]=" << za1_[c] << endl;
      // cout << "state_1_[c]=" << state_1_[c] << endl;
      // cout << "state_2_[c]=" << state_2_[c] << endl;

      double output = za0_[c] * new_state + za1_[c] * state_1_[c]
                      + za2_[c] * state_2_[c];

      // cubic compression nonlinearity
      output -= 0.0001 * pow(output, 3);
      // cout << "output=" << output << endl;

      out(c, t) = output;
      detect_[c] = DetectFun(output);
      state_2_[c] = state_1_[c];
      state_1_[c] = new_state;
    }

    if (do_agc)
    {
      // double offset = 1.0 - ctrl_agc_factor_->to<mrs_real>() * DetectFun(0.0);
      // double agc_factor = ctrl_agc_factor_->to<mrs_real>();
      AGCDampStep();
    }

    for (int c = 0; c < channel_count_; ++c)
      previous_out_[c] = out(c,t);
  }


// Copy over the centre frequencies to the second half of the observations
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    for (mrs_natural o = 0; o < channel_count_; o++)
    {
      out(o + channel_count_, t) = centre_frequencies_[o];
    }
  }

}
