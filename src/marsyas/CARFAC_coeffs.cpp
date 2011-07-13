/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "CARFAC.h"

using std::cout;
using std::endl;
using std::ostream;

namespace Marsyas
{

filter_state_class::filter_state_class()
{
}

filter_state_class::~filter_state_class()
{
}

ostream& operator<<(ostream& o, std::vector<double> a)
{
  int max_x = (a.size() < 5) ? a.size() : 5;
  for (int i=0; i<max_x; i++) {
    o << a[i] << " ";
  }
  return o;
}

ostream& operator<<(ostream& o, std::vector<std::vector<double> > a)
{
  int max_x = (a.size() < 5) ? a.size() : 5;
  int max_y = (a[0].size() < 5) ? a[0].size() : 5;
  for (int i=0; i<max_x; i++) {
    for (int j=0; j<max_y; j++) {
      o << a[i][j] << " ";
    }
    o << endl << "\t\t\t";
  }
  return o;
}

ostream& operator<<(ostream& o, const filter_state_class& l)
{
  o << "\tz1_memory=" << l.z1_memory << endl;
  o << "\tz2_memory=" << l.z2_memory << endl;
  o << "\tzA_memory=" << l.zA_memory << endl;
  o << "\tzB_memory=" << l.zB_memory << endl;
  o << "\tdzB_memory=" << l.dzB_memory << endl;
  o << "\tzY_memory=" << l.zY_memory << endl;
  o << "\tdetect_accum=" << l.detect_accum << endl;

  return o;
}

//////////

AGC_state_class::AGC_state_class()
{
}

AGC_state_class::~AGC_state_class()
{
}

ostream& operator<<(ostream& o, const AGC_state_class& l)
{
  o << "**AGC_state_class" << endl;
  o << "\tsum_AGC=" << l.sum_AGC << endl;

  for (int i = 0; i < 4; i++) {
    o << "\tAGC_memory(" << i << ")=";
    for (int j = 0; j < 5; j++) {
      o << l.AGC_memory[j][i] << " ";
    }
    o << endl;
  }
  return o;
}

//////////

filter_coeffs_class::filter_coeffs_class()
{
}

filter_coeffs_class::~filter_coeffs_class()
{
}

void filter_coeffs_class::init(double v, int n_ch)
{
  velocity_scale = v;

  r_coeffs.assign(n_ch,0);
  a_coeffs.assign(n_ch,0);
  c_coeffs.assign(n_ch,0);
  h_coeffs.assign(n_ch,0);
  g_coeffs.assign(n_ch,0);

}

ostream& operator<<(ostream& o, const filter_coeffs_class& l)
{
  o << "**filter_coeffs_class" << endl;
  o << "\t\tvelocity_scale=" << l.velocity_scale << endl;

  o << "\t\tr_coeffs=" << l.r_coeffs << endl;
  o << "\t\ta_coeffs=" << l.a_coeffs << endl;
  o << "\t\tc_coeffs=" << l.c_coeffs << endl;
  o << "\t\th_coeffs=" << l.h_coeffs << endl;
  o << "\t\tg_coeffs=" << l.g_coeffs << endl;

  return o;
}


//////////

CF_AGC_params_class::CF_AGC_params_class()
{
  n_stages = 4;
  time_constants.push_back(1*0.002);
  time_constants.push_back(4*0.002);
  time_constants.push_back(16*0.002);
  time_constants.push_back(64*0.002);

  AGC_stage_gain = 2;
  decimation = 16;

  AGC1_scales.push_back(1*1);
  AGC1_scales.push_back(2*1);
  AGC1_scales.push_back(3*1);
  AGC1_scales.push_back(4*1);

  AGC2_scales.push_back(1*1.5);
  AGC2_scales.push_back(2*1.5);
  AGC2_scales.push_back(3*1.5);
  AGC2_scales.push_back(4*1.5);

  detect_scale =  0.002;
  AGC_mix_coeff = 0.25;
}


CF_AGC_params_class::~CF_AGC_params_class()
{
}

ostream& operator<<(ostream& o, const CF_AGC_params_class& l)
{
  o << "**CF_AGC_params_class" << endl;
  o << "\t\tn_stages=" << l.n_stages << endl;

  o << "\t\ttime_constants=[";
  for (unsigned int i=0; i<l.time_constants.size(); i++) {
    o << l.time_constants[i] << " ";
  }
  o << "]" << endl;

  o << "\t\tAGC_stage_gain=" << l.AGC_stage_gain << endl;
  o << "\t\tdecimation=" << l.decimation << endl;

  o << "\t\tAGC1_scales=";
  for (unsigned int i=0; i<l.AGC1_scales.size(); i++) {
    o << l.AGC1_scales[i] << " ";
  }
  o << endl;

  o << "\t\tAGC2_scales=";
  for (unsigned int i=0; i<l.AGC2_scales.size(); i++) {
    o << l.AGC2_scales[i] << " ";
  }
  o << endl;

  o << "\t\tdetect_scale=" << l.detect_scale << endl;
  o << "\t\tAGC_mix_coeff=" << l.AGC_mix_coeff << endl;

  return o;
}

//////////

AGC_coeffs_class::AGC_coeffs_class()
{
}

AGC_coeffs_class::~AGC_coeffs_class()
{
}

AGC_coeffs_class& AGC_coeffs_class::operator=(const CF_AGC_params_class& params)
{
  detect_scale = params.detect_scale;
  AGC_stage_gain = params.AGC_stage_gain;
  AGC_mix_coeff = params.AGC_mix_coeff;
  return *this;
}

ostream& operator<<(ostream& o, const AGC_coeffs_class& l)
{
  o << "**AGC_coeffs_class" << endl;
  o << "\t\tdetect_scale=" << l.detect_scale << endl;
  o << "\t\tAGC_stage_gain=" << l.AGC_stage_gain << endl;
  o << "\t\tAGC_mix_coeff=" << l.AGC_mix_coeff << endl;
  o << "\t\tAGC_epsilon=[";
  for (unsigned int i=0; i<l.AGC_epsilon.size(); i++) {
    o << l.AGC_epsilon[i] << " ";
  }
  o << "]" << endl;

  return o;
}

//////////

CF_filter_params_class::CF_filter_params_class()
{
  velocity_scale = 0.002;           // for the cubic nonlinearity
  min_zeta = 0.15;
  first_pole_theta = 0.78 * PI;
  zero_ratio = sqrt(2);
  ERB_per_step = 0.3333;            // assume G&M's ERB formula
  min_pole_Hz = 40;
}

CF_filter_params_class::~CF_filter_params_class()
{
}

// CF_filter_params_class::CF_filter_params_class(const CF_filter_params_class& a)
// {
// }

ostream& operator<<(ostream& o, const CF_filter_params_class& l)
{
  o << "**CF_filter_params_class" << endl;
  o << "\t\tvelocity_scale=" << l.velocity_scale << endl;
  o << "\t\tmin_zeta=" << l.min_zeta << endl;
  o << "\t\tfirst_pole_theta=" << l.first_pole_theta << endl;
  o << "\t\tzero_ratio=" << l.zero_ratio << endl;
  o << "\t\tERB_per_step=" << l.ERB_per_step << endl;
  o << "\t\tmin_pole_Hz=" << l.min_pole_Hz << endl;
  return o;
}

//////////

CF_class::CF_class()
{
  CARFAC_Design();
  CARFAC_DesignFilters(CF_filter_params, fs, pole_freqs);
  CARFAC_DesignAGC(fs);

  // TODO(snessnet) - This should get updated from inObservations
  n_mics = 2;

  CARFAC_Init(n_mics);
}

CF_class::~CF_class()
{
}

//
// Original Docs
// -------------
//
// function CF = CARFAC_Design(fs, CF_filter_params, ...
// CF_AGC_params, ERB_min_BW, ERB_Q)
//
// This function designs the CARFAC (Cascade of Asymmetric Resonators with
// Fast-Acting Compression); that is, it take bundles of parameters and
// computes all the filter coefficients needed to run it.
//
// fs is sample rate (per second)
// CF_filter_params bundles all the PZFC parameters
// CF_AGC_params bundles all the AGC parameters
//
// See other functions for designing and characterizing the CARFAC:
// [naps, CF] = CARFAC_Run(CF, input_waves)
// transfns = CARFAC_Transfer_Functions(CF, to_channels, from_channels)
//
// All args are defaultable; for sample/default args see the code; they
// make 96 channels at default fs = 22050, 114 channels at 44100.
//

// TODO(snessnet) - Have CARFAC_Design take parameters like the original function
void CF_class::CARFAC_Design(double _fs, double _ERB_break_freq, double _ERB_Q)
{
  AGC_coeffs = CF_AGC_params;

  // TODO(snessnet) - We should get this from israte, but this won't
  // be setup until we load the soundfile.  For now require 22050Hz
  // sound files.
  if (_fs == -1) {
    fs = 22050;
  }

  // first figure out how many filter stages (PZFC/CARFAC channels):
  double pole_Hz = CF_filter_params.first_pole_theta * fs / (2 * PI);
  n_ch = 0;
  while (pole_Hz > CF_filter_params.min_pole_Hz) {
    n_ch = n_ch + 1;
    pole_Hz = pole_Hz - CF_filter_params.ERB_per_step * ERB_Hz(pole_Hz, _ERB_break_freq, _ERB_Q);
  }

  pole_freqs.assign(n_ch,0);
  pole_Hz = CF_filter_params.first_pole_theta * fs / (2 * PI);
  for(int ch = 0; ch < n_ch; ch++) {
    pole_freqs[ch] = pole_Hz;
    pole_Hz = pole_Hz - CF_filter_params.ERB_per_step * ERB_Hz(pole_Hz, _ERB_break_freq, _ERB_Q);
  }
}

// Calculate the Equivalent Rectangular Bandwith of a given frequency
double CF_class::ERB_Hz(double CF_Hz, double ERB_break_freq, double ERB_Q)
{
  if (ERB_Q == -1) {
    ERB_Q = 1000/(24.7*4.37); // 9.2645
  }

  if (ERB_break_freq == -1) {
    ERB_break_freq = 1000/4.37; // 228.833
  }

  double ERB = (ERB_break_freq + CF_Hz) / ERB_Q;
  return ERB;
}

// From the input filter_params, sampling frequency and
// pole_frequencies, create all the filter coefficients.
void CF_class::CARFAC_DesignFilters(CF_filter_params_class filter_params, double fs, std::vector<double> pole_freqs)
{
  int n_ch = pole_freqs.size();

  filter_coeffs.init(filter_params.velocity_scale, n_ch);

  // zero_ratio comes in via h.  In book's circuit D, zero_ratio is
  // 1/sqrt(a), and that a is here 1 / (1+f) where h = f*c.
  // solve for f:  1/zero_ratio^2 = 1 / (1+f)
  // zero_ratio^2 = 1+f => f = zero_ratio^2 - 1
  double f = pow(filter_params.zero_ratio,2) - 1;  // nominally 1 for half-octave

  // Make pole positions, s and c coeffs, h and g coeffs, etc., which
  // mostly depend on the pole angle theta.
  std::vector<double> theta(n_ch);
  for (unsigned int i = 0; i < theta.size(); i++) {
    theta[i] = pole_freqs[i] * (2 * PI / fs);
  }

  // Different possible interpretations for min-damping r:
  // r = exp(-theta * CF_filter_params.min_zeta);
  std::vector<double> r(n_ch);
  for (unsigned int i = 0; i < r.size(); i++) {
    r[i] = (1 - (sin(theta[i]) * filter_params.min_zeta)); // higher Q at highest thetas
  }
  filter_coeffs.r_coeffs = r;

  // Undamped coupled-form coefficients:
  for (unsigned int i = 0; i < theta.size(); i++) {
    filter_coeffs.a_coeffs[i] = cos(theta[i]);
    filter_coeffs.c_coeffs[i] = sin(theta[i]);
  }

  // The zeros follow via the h_coeffs
  std::vector<double> h(n_ch);
  for (unsigned int i = 0; i < theta.size(); i++) {
    h[i] = sin(theta[i]) * f;
  }
  filter_coeffs.h_coeffs = h;

  // Aim for unity DC gain at min damping, here; or could try r^2
  std::vector<double> r2 = r;
  for (unsigned int i = 0; i < theta.size(); i++) {
    filter_coeffs.g_coeffs[i] =
        1 / (1 + h[i] * r2[i] * sin(theta[i]) / (1 - 2 * r2[i] * cos(theta[i]) + pow(r2[i], 2)));
  }
}

// Calculate the parameters for the AGC step
void CF_class::CARFAC_DesignAGC(double fs)
{
  std::vector<double> AGC1_scales = CF_AGC_params.AGC1_scales;
  std::vector<double> AGC2_scales = CF_AGC_params.AGC2_scales;

  int n_AGC_stages = CF_AGC_params.n_stages;
  AGC_coeffs.AGC_epsilon.assign(n_AGC_stages, 0);
  AGC_coeffs.AGC1_polez.assign(n_AGC_stages, 0);
  AGC_coeffs.AGC2_polez.assign(n_AGC_stages, 0);
  int decim = CF_AGC_params.decimation;

  for (int stage = 0; stage < n_AGC_stages; stage++) {
    double tau = CF_AGC_params.time_constants[stage];

    // Epsilon is how much new input to take at each update step.
    AGC_coeffs.AGC_epsilon[stage] = 1 - exp(-decim / (tau * fs));

    // And these are the smoothing scales and poles for decimated rate.
    double ntimes = tau * (fs / decim);  // Effective number of times the smoothing is done

    // Divide the spatial variance by effective number of smoothings:
    double t = (pow(AGC1_scales[stage],2)) / ntimes;  // Adjust scale per step for diffusion
    AGC_coeffs.AGC1_polez[stage] = 1 + 1/t - sqrt(pow(1+1/t,2) - 1);
    t = (pow(AGC2_scales[stage],2)) / ntimes;  // Adjust scale per step for diffusion.
    AGC_coeffs.AGC2_polez[stage] = 1 + 1/t - sqrt(pow(1+1/t,2) - 1);
  }
}

//
// Initialize state for n_mics channels (default 1).
//
// TODO(dicklyon): Review whether storing state in the same struct as
// the design is a good thing, or whether we want another level of
// object.  I like fewer structs and class types.  function CF_struct
// = CARFAC_Init(CF_struct, n_mics)
//
// Initialize state for n_mics channels (default 1).
//
// TODO(dicklyon): Review whether storing state in the same struct as
// the design is a good thing, or whether we want another
// level of object.  I like fewer structs and class types.
//
void CF_class::CARFAC_Init(int n_mics)
{
  if (n_mics == -1) {
    n_mics = 1;
  }

  // TODO(snessnet) - Just hacking this for now to be 2
  n_mics = 2;

  std::vector<double> AGC_time_constants = CF_AGC_params.time_constants;
  int n_AGC_stages = AGC_time_constants.size();
  filter_state_class tmp_filter_state;
  tmp_filter_state.z1_memory.assign(n_ch,0.0);
  tmp_filter_state.z2_memory.assign(n_ch,0.0);
  tmp_filter_state.zA_memory.assign(n_ch,0.0);
  tmp_filter_state.zB_memory.assign(n_ch,0.0);
  tmp_filter_state.dzB_memory.assign(n_ch,0.0);
  tmp_filter_state.zY_memory.assign(n_ch,0.0);
  tmp_filter_state.detect_accum.assign(n_ch,0.0);

  for (int mic = 0; mic < n_mics; mic++) {
    filter_state.push_back(tmp_filter_state);
  }

  // AGC loop filters' state:
  AGC_state_class tmp_AGC_state;
  tmp_AGC_state.sum_AGC.assign(n_ch,0.0);

  std::vector<double> tmp_AGC_memory(n_AGC_stages);
  for (int i = 0; i < n_ch; i++) {
    tmp_AGC_state.AGC_memory.push_back(tmp_AGC_memory);
  }

  for (int mic = 0; mic < n_mics; mic++) {
    AGC_state.push_back(tmp_AGC_state);
  }
}

ostream& operator<<(ostream& o, const CF_class& l)
{
  o << "*CF_class" << endl;
  if (l.printcoeffs) {
    o << "\tfs=" << l.fs << endl;
    o << "\tn_ch=" << l.n_ch << endl;
    o << "\tn_mics=" << l.n_mics << endl;
    o << "\tCF_filter_params=" << l.CF_filter_params << endl;
    o << "\tCF_AGC_params=" << l.CF_AGC_params << endl;
    o << "\tfilter_coeffs=" << l.filter_coeffs << endl;
    o << "\tAGC_coeffs=" << l.AGC_coeffs << endl;
  }

  if (l.printstate) {
    for (unsigned int i=0; i<l.filter_state.size(); i++) {
      o << "filter_state(" << i+1 << ")" << endl;
      o << l.filter_state[i];

      o << "AGC_state(" << i+1 << ")" << endl;
      o << l.AGC_state[i];
    }
  }

  return o;
}
}
