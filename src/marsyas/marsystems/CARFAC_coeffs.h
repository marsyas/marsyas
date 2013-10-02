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

#ifndef MARSYAS_CARFAC_COEFFS_H
#define MARSYAS_CARFAC_COEFFS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \ingroup Processing

   \brief This function designs the CARFAC (Cascade of Asymmetric
   Resonators with Fast-Acting Compression); that is, it take bundles
   of parameters and computes all the filter coefficients needed to
   run it.

   The original code for CARFAC was designed and written by Dick Lyon
   (dicklyon@google.com) in MATLAB.  Steven Ness (sness@sness.net)
   ported this code to C++.  I've written this to be as standard C++
   as possible so that we can easily port this filter to other
   frameworks like AIM-C.

   Controls:
   - \b mrs_natural/num_channels [w] : The number of output channels.


*/

class CF_class;

class filter_state_class {
public:
  std::vector<double> z1_memory;
  std::vector<double> z2_memory;
  std::vector<double> zA_memory;
  std::vector<double> zB_memory;
  std::vector<double> dzB_memory;
  std::vector<double> zY_memory;
  std::vector<double> detect_accum;

  std::vector<double> filterstep_inputs;
  std::vector<double> filterstep_zA;
  std::vector<double> filterstep_zB;
  std::vector<double> filterstep_zY;
  std::vector<double> filterstep_r;
  std::vector<double> filterstep_z1;
  std::vector<double> filterstep_z2;
  // std::vector<double> filterstep_detect;

  bool init;

public:
  filter_state_class();
  ~filter_state_class();

  std::vector<double> FilterStep(CF_class &CF, double input_waves, std::vector<double> &detect);

  friend std::ostream& operator<<(std::ostream&, const filter_state_class&);
  friend std::ostream& operator<<(std::ostream&, std::vector<std::vector<double> > a);
  friend std::ostream& operator<<(std::ostream&, std::vector<double> a);
};

class AGC_state_class {
public:
  std::vector<std::vector<double> > AGC_memory;
  std::vector<double> sum_AGC;


public:
  AGC_state_class();
  ~AGC_state_class();

  friend std::ostream& operator<<(std::ostream&, const AGC_state_class&);
  friend std::ostream& operator<<(std::ostream&, std::vector<double> a);
};

class strobe_state_class {
public:
  std::vector<double> lastdata;
  std::vector<double> thresholds;

  std::vector<int> trigger_index;
  std::vector<int> sai_index;

public:
  strobe_state_class();
  ~strobe_state_class();

  friend std::ostream& operator<<(std::ostream&, const strobe_state_class&);
  friend std::ostream& operator<<(std::ostream&, std::vector<double> a);
};

class filter_coeffs_class {
public:
  double velocity_scale;

  std::vector<double> r_coeffs;
  std::vector<double> a_coeffs;
  std::vector<double> c_coeffs;
  std::vector<double> h_coeffs;
  std::vector<double> g_coeffs;

public:
  filter_coeffs_class();
  ~filter_coeffs_class();

  void init(double velocity_scale, int n_ch);

  friend std::ostream& operator<<(std::ostream&, const filter_coeffs_class&);
};

class CF_AGC_params_class {
public:
  int n_stages;                       // The number of AGC stages
  std::vector<double> time_constants;
  int AGC_stage_gain;                 // Gain from each stage to next slower stage
  int decimation;                     // How often to update the AGC states
  std::vector<double> AGC1_scales;    // In units of channels
  std::vector<double> AGC2_scales;    // Spread more toward base
  double detect_scale;                // Max nap will be 10 to 100 typically
  double AGC_mix_coeff;

public:
  CF_AGC_params_class();
  CF_AGC_params_class(const CF_AGC_params_class& a);
  ~CF_AGC_params_class();

  friend std::ostream& operator<<(std::ostream&, const CF_AGC_params_class&);
};

class AGC_coeffs_class {
public:
  double detect_scale;
  double AGC_stage_gain;
  double AGC_mix_coeff;
  std::vector<double> AGC_epsilon;
  std::vector<double> AGC1_polez;
  std::vector<double> AGC2_polez;

public:
  AGC_coeffs_class();
  ~AGC_coeffs_class();
};


class CF_filter_params_class {
public:
  double velocity_scale;
  double min_zeta;
  double first_pole_theta;
  double zero_ratio;
  double ERB_per_step;
  double min_pole_Hz;

public:
  CF_filter_params_class();
  ~CF_filter_params_class();

  friend std::ostream& operator<<(std::ostream&, const CF_filter_params_class&);
};

class CF_class {
public:
  int fs;                                                // Sampling frequency
  CF_filter_params_class CF_filter_params;               // The CARFAC filter parameters
  CF_AGC_params_class CF_AGC_params;                     // The Automatic Gain Control parameters
  int n_ch;                                              // Number of channels
  std::vector<double> pole_freqs;                        // The frequencies of each of the poles
  filter_coeffs_class filter_coeffs;                     // The filter coefficients
  AGC_coeffs_class AGC_coeffs;                           // The Automatic Gain Control coefficients
  int n_mics;                                            // Number of microphones (input observations)
  std::vector<filter_state_class> filter_state;          // The current state of the filter
  std::vector<AGC_state_class> AGC_state;                // The current state of the AGC
  std::vector<strobe_state_class> strobe_state;          // The current state of the strobes
  std::vector<std::vector<std::vector<double> > > nap;   // The Neural Activity Pattern, the output of this filter
  int cum_k;                                             // Global time step in concatenated segments

  double strobe_threshold_start;

public:
  CF_class();
  CF_class(const CF_class& a);
  ~CF_class();

  void CARFAC_Design(double fs = -1, double ERB_break_freq = -1, double ERB_q = -1);
  void CARFAC_DesignFilters();
  void CARFAC_DesignAGC();
  void CARFAC_Init(int n_mics = -1);

  double ERB_Hz(double CF_Hz, double ERB_break_freq = -1, double ERB_Q = -1);

  bool printstate;
  bool printcoeffs;

  friend std::ostream& operator<<(std::ostream&, const CF_class&);
};


}//namespace Marsyas

#endif
