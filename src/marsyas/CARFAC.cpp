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

//////////

filter_state_class::filter_state_class()
{
}

filter_state_class::~filter_state_class()
{
}

ostream& operator<<(ostream& o, std::vector<double> a)
{
  int max_x = (a.size() < 3) ? a.size() : 3;
  for (int i=0; i<max_x; i++) {
    o << a[i] << " ";
  }
  return o;
}

ostream& operator<<(ostream& o, std::vector<std::vector<double> > a)
{
  int max_x = (a.size() < 3) ? a.size() : 3;
  int max_y = (a[0].size() < 3) ? a[0].size() : 3;
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
  o << "**filter_state_class" << endl;

  o << "\tz1_memory=" << l.z1_memory;
  o << "\tz2_memory=" << l.z2_memory;
  o << "\tzA_memory=" << l.zA_memory;
  o << "\tzB_memory=" << l.zB_memory;
  o << "\tdzB_memory=" << l.dzB_memory;
  o << "\tzY_memory=" << l.zY_memory;
  o << "\tdetect_accum=" << l.detect_accum;

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

  o << "\t\tr_coeffs=" << l.r_coeffs;
  o << "\t\ta_coeffs=" << l.a_coeffs;
  o << "\t\tc_coeffs=" << l.c_coeffs;
  o << "\t\th_coeffs=" << l.h_coeffs;
  o << "\t\tg_coeffs=" << l.g_coeffs;

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
  // CF_AGC_params = CF_AGC_params_class();
  // CF_filter_params = CF_filter_params_class();


  AGC_coeffs = CF_AGC_params;

  // TODO(snessnet) - We should get this from israte, but this won't
  // be setup until we load the soundfile.  For now only use
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


void CF_class::CARFAC_DesignFilters(CF_filter_params_class filter_params, double fs, std::vector<double> pole_freqs)
{
  int n_ch = pole_freqs.size();

  filter_coeffs.init(filter_params.velocity_scale, n_ch);

  // zero_ratio comes in via h.  In book's circuit D, zero_ratio is 1/sqrt(a),
  // and that a is here 1 / (1+f) where h = f*c.
  // solve for f:  1/zero_ratio^2 = 1 / (1+f)
  // zero_ratio^2 = 1+f => f = zero_ratio^2 - 1
  double f = pow(filter_params.zero_ratio,2) - 1;  // nominally 1 for half-octave

  // Make pole positions, s and c coeffs, h and g coeffs, etc.,
  // which mostly depend on the pole angle theta:
  std::vector<double> theta(n_ch);
  for (unsigned int i = 0; i < theta.size(); i++) {
    theta[i] = pole_freqs[i] * (2 * PI / fs);
  }

  // different possible interpretations for min-damping r:
  // r = exp(-theta * CF_filter_params.min_zeta);
  std::vector<double> r(n_ch);
  for (unsigned int i = 0; i < r.size(); i++) {
    r[i] = (1 - (sin(theta[i]) * filter_params.min_zeta)); // higher Q at highest thetas
  }
  filter_coeffs.r_coeffs = r;

  // undamped coupled-form coefficients:
  for (unsigned int i = 0; i < theta.size(); i++) {
    filter_coeffs.a_coeffs[i] = cos(theta[i]);
    filter_coeffs.c_coeffs[i] = sin(theta[i]);
  }

  // the zeros follow via the h_coeffs
  std::vector<double> h(n_ch);
  for (unsigned int i = 0; i < theta.size(); i++) {
    h[i] = sin(theta[i]) * f;
  }
  filter_coeffs.h_coeffs = h;

  std::vector<double> r2 = r; // aim for unity DC gain at min damping, here; or could try r^2
  for (unsigned int i = 0; i < theta.size(); i++) {
    filter_coeffs.g_coeffs[i] =
        1 / (1 + h[i] * r2[i] * sin(theta[i]) / (1 - 2 * r2[i] * cos(theta[i]) + pow(r2[i], 2)));
  }
}

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
    // epsilon is how much new input to take at each update step:
    AGC_coeffs.AGC_epsilon[stage] = 1 - exp(-decim / (tau * fs));
    // and these are the smoothing scales and poles for decimated rate:
    double ntimes = tau * (fs / decim);  // effective number of times the smoothing is done
    // divide the spatial variance by effective number of smoothings:
    double t = (pow(AGC1_scales[stage],2)) / ntimes;  // adjust scale per step for diffusion
    AGC_coeffs.AGC1_polez[stage] = 1 + 1/t - sqrt(pow(1+1/t,2) - 1);
    t = (pow(AGC2_scales[stage],2)) / ntimes;  // adjust scale per step for diffusion
    AGC_coeffs.AGC2_polez[stage] = 1 + 1/t - sqrt(pow(1+1/t,2) - 1);
  }


}

// Initialize state for n_mics channels (default 1).
//
// TODO: Review whether storing state in the same struct as
// the design is a good thing, or whether we want another
// level of object.  I like fewer structs and class types.
// function CF_struct = CARFAC_Init(CF_struct, n_mics)
//
// Initialize state for n_mics channels (default 1).
//
// TODO: Review whether storing state in the same struct as
// the design is a good thing, or whether we want another
// level of object.  I like fewer structs and class types.
void CF_class::CARFAC_Init(int n_mics)
{
  if (n_mics == -1) {
    n_mics = 1;
  }
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
  tmp_AGC_state.AGC_sum.assign(n_ch,0.0);
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
  o << "\tfs=" << l.fs << endl;
  o << "\tCF_filter_params=" << l.CF_filter_params << endl;
  o << "\tCF_AGC_params=" << l.CF_AGC_params << endl;
  o << "\tn_ch=" << l.n_ch << endl;
  o << "\tfilter_coeffs=" << l.filter_coeffs << endl;
  o << "\tAGC_coeffs=" << l.AGC_coeffs << endl;
  o << "\tn_mics=" << l.n_mics << endl;
  o << "filter_state=";
  for (unsigned int i=0; i<l.filter_state.size(); i++) {
    o << l.filter_state[i] << endl;
  }
  o << "AGC_state=";
  for (unsigned int i=0; i<l.AGC_state.size(); i++) {
    o << l.AGC_state[i] << endl;
  }

  return o;
}

//////////


CARFAC::CARFAC(mrs_string name):MarSystem("CARFAC", name)
{
	//Add any specific controls needed by CARFAC
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(),
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
	addControls();
}

CARFAC::CARFAC(const CARFAC& a) : MarSystem(a)
{
	// For any MarControlPtr in a MarSystem
	// it is necessary to perform this getctrl
	// in the copy constructor in order for cloning to work
	ctrl_gain_ = getctrl("mrs_real/gain");
}

CARFAC::~CARFAC()
{
}

MarSystem*
CARFAC::clone() const
{
	return new CARFAC(*this);
}

void
CARFAC::addControls()
{
	//Add specific controls needed by this MarSystem.
	// addctrl("mrs_real/gain", 1.0, ctrl_gain_);
}


void
CARFAC::myUpdate(MarControlPtr sender)
{
	// for efficiency
	// gainValue_ = ctrl_gain_->to<mrs_real>();
	// no change to network flow
  MarSystem::myUpdate(sender);

  // ctrl_onObservations_->setValue(channel_count_ * 2, NOUPDATE);
  int n_ch = 96;
  ctrl_onObservations_->setValue(n_ch * 2, NOUPDATE);

}

void
CARFAC::myProcess(realvec& in, realvec& out)
{
  cout << "CARFAC::myProcess" << endl;

  int n_samp = inSamples_;
  int n_ch = CF.n_ch;
  int n_mics = CF.n_mics;

  int decim_k = -1;

  bool make_decim_naps = true;

  int cum_k = -1;
  int decim = 16;

  // Create the naps array
  std::vector<std::vector<std::vector<double> > > naps;
  naps.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      naps[i][j].resize(n_mics);
    }
  }

  // Create the decim_naps array
  std::vector<std::vector<std::vector<double> > > decim_naps;
  int decim_naps_size = n_samp/decim;
  decim_naps.resize(decim_naps_size);
  for (int i = 0; i < decim_naps_size; i++) {
    decim_naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      decim_naps[i][j].resize(n_mics);
    }
  }

  std::vector<double> input_to_filterstep(CF.n_mics, 0.0);
  std::vector<double> detect;
  std::vector<double> avg_detect(n_ch);

  for (mrs_natural k = 0; k < inSamples_; k++) {
    // cout << "k=" << k << endl;
    cum_k = cum_k + 1;
    for (mrs_natural o=0; o < inObservations_; o++) {
      input_to_filterstep[o] = in(o,k);
    }
    for (int mic = 0; mic < n_mics; mic++) {
      detect = CARFAC_FilterStep(input_to_filterstep,mic);
      for (int i=0; i < detect.size(); i++) {
        naps[k][i][mic] = detect[i];
      }
    }
    // conditionally update all the AGC stages and channels now
    if ((cum_k+1) % decim == 0) { // using cum time in case we're doing segments
      // just for the plotting option:
      decim_k = decim_k + 1; // index of decimated signal for display
      if (make_decim_naps) {
        for (int mic = 0; mic < n_mics; mic++) {
          for (int i=0; i < n_ch; i++) {
            for (int j=0; j < n_ch; j++) {
              avg_detect[j] = CF.filter_state[mic].detect_accum[i] / decim;
              decim_naps[decim_k][j][mic] = avg_detect[j]; // for cochleagram
            }
          }
        }
      }
      std::vector<std::vector<double> > avg_detects(n_ch, std::vector<double>(n_mics));

      for (int mic = 0; mic < n_mics; mic++) {
        for (int j=0; j < n_ch; j++) {
          avg_detects[j][mic] = CF.filter_state[mic].detect_accum[j] / decim;
          CF.filter_state[mic].detect_accum[j] = 0.0;  // zero the detect accumulator
        }
      }

      CARFAC_AGCStep(avg_detects);
      for (int mic = 0; mic < n_mics; mic++) {
        for (int i = 0; i < n_ch; i++) {
          CF.filter_state[mic].dzB_memory[i] =
              (CF.AGC_state[mic].sum_AGC[i] - CF.filter_state[mic].zB_memory[i]) / decim;
        }
      }
    }
  }


  // Copy the nap data to the output
  for (int row = 0; row < n_ch; row++) {
    for (int col = 0; col < inSamples_; col++) {
      out(row,col) = naps[col][row][0];
      out(row+n_ch,col) = naps[col][row][1];
    }
  }
}

std::vector<double> CARFAC::CARFAC_FilterStep(std::vector<double> input_waves, int mic)
{
  // Use each stage previous Y as input to next:
  std::vector<double> inputs(CF.n_ch);
  inputs[0] = input_waves[0];
  for (int i=0; i<CF.filter_state[mic].zY_memory.size()-1; i++) {
    inputs[i+1] = CF.filter_state[mic].zY_memory[i];
  }

  // Local nonlinearity zA and AGC feedback zB reduce pole radius:
  std::vector<double> zA = CF.filter_state[mic].zA_memory;

  // AGC interpolation
  std::vector<double> zB(CF.n_ch);
  for (int i=0; i<CF.n_ch; i++) {
    zB[i] = CF.filter_state[mic].zB_memory[i] + CF.filter_state[mic].dzB_memory[i];
  }

  std::vector<double> r(CF.n_ch);
  for (int i=0; i<CF.n_ch; i++) {
    r[i] = CF.filter_coeffs.r_coeffs[i] - CF.filter_coeffs.c_coeffs[i] * (zA[i] + zB[i]);
  }

  // now reduce filter_state by r and rotate with the fixed cos/sin coeffs:
  std::vector<double> z1(CF.n_ch);
  std::vector<double> z2(CF.n_ch);
  for (int i=0; i<CF.n_ch; i++) {
    z1[i] = r[i] * (CF.filter_coeffs.a_coeffs[i] * CF.filter_state[mic].z1_memory[i] -
              CF.filter_coeffs.c_coeffs[i] * CF.filter_state[mic].z2_memory[i]);
    z1[i] = z1[i] + inputs[i];
    z2[i] = r[i] * (CF.filter_coeffs.c_coeffs[i] * CF.filter_state[mic].z1_memory[i] +
              CF.filter_coeffs.a_coeffs[i] * CF.filter_state[mic].z2_memory[i]);
  }

  // update the "velocity" for cubic nonlinearity, into zA:
  for (int i=0; i<CF.n_ch; i++) {
    zA[i] = pow(((CF.filter_state[mic].z2_memory[i] - z2[i]) * CF.filter_coeffs.velocity_scale), 2);
  }

  // simulate Sigmoidal OHC effect on damping:
  for (int i=0; i<CF.n_ch; i++) {
    zA[i] = (1 - pow((1 - zA[i]), 4)) / 4;  // soft max at 0.25
  }

  // Get outputs from inputs and new z2 values:
  std::vector<double> zY(CF.n_ch);
  for (int i=0; i<CF.n_ch; i++) {
    zY[i] = CF.filter_coeffs.g_coeffs[i] * (inputs[i] + CF.filter_coeffs.h_coeffs[i] * z2[i]);
  }

  // TODO:  Generalize to a detection nonlinearity
  double maxval = 0.0;
  std::vector<double> detect(CF.n_ch);

  for (int i=0; i<CF.n_ch; i++) {
    if (zY[i] > maxval) {
      detect[i] = zY[i];
    } else {
      detect[i] = maxval;
    }
  }

  // put new CF.filter_state[mic] back in place of old
  CF.filter_state[mic].z1_memory = z1;
  CF.filter_state[mic].z2_memory = z2;
  CF.filter_state[mic].zA_memory = zA;
  CF.filter_state[mic].zB_memory = zB;
  CF.filter_state[mic].zY_memory = zY;
  for (int i=0; i<CF.n_ch; i++) {
    CF.filter_state[mic].detect_accum[i] = CF.filter_state[mic].detect_accum[i] + detect[i];
  }

  return detect;
}

void CARFAC::CARFAC_AGCStep(std::vector<std::vector<double> > avg_detects)
{
  int n_AGC_stages = CF.AGC_coeffs.AGC_epsilon.size();
  int n_mics = CF.n_mics;
  int n_ch = CF.n_ch;

  bool optimize_for_mono = (n_mics == 1) ? true : false;

  std::vector<double> prev_stage_mean(n_ch,0.0);
  std::vector<double> stage_sum(n_ch,0.0);

  std::vector<double> AGC_in(n_ch,0.0);
  std::vector<double> AGC_stage(n_ch,0.0);

  for (int stage = 0; stage < n_AGC_stages; stage++) {
    if (!optimize_for_mono) {
      if (stage > 0) {
        for (int i = 0; i < n_ch; i++) {
          prev_stage_mean[i] = stage_sum[i] / n_mics;
        }
      }
      for (int i = 0; i < n_ch; i++) {
          stage_sum[i] = 0; // sum accumulating over mics at this stage
      }
    }
    double epsilon = CF.AGC_coeffs.AGC_epsilon[stage];
    double polez1 = CF.AGC_coeffs.AGC1_polez[stage];
    double polez2 = CF.AGC_coeffs.AGC1_polez[stage];
    for (int mic = 0; mic < n_mics; mic++) {
      if (stage == 1) {
        for (int i = 0; i < n_ch; i++) {
          AGC_in[i] = CF.AGC_coeffs.detect_scale * avg_detects[i][mic];
        }
      } else {
        if (!optimize_for_mono) {
          // Mono optimization ignores AGC_mix_coeff,
          // assuming all(prev_stage_mean == AGC_memory(:, stage - 1));
          // but we also don't even allocate or compute the sum or mean.
          for (int i = 0; i < n_ch; i++) {
            AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain * CF.AGC_state[mic].AGC_memory[i][stage - 1];
          }
        } else {
          for (int i = 0; i < n_ch; i++) {
            AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain *
                (CF.AGC_coeffs.AGC_mix_coeff * prev_stage_mean[i] +
                 (1 - CF.AGC_coeffs.AGC_mix_coeff) * CF.AGC_state[mic].AGC_memory[i][stage - 1]);
          }
        }
      }
      for (int i = 0; i < n_ch; i++) {
        AGC_stage[i] = CF.AGC_state[mic].AGC_memory[i][stage];
      }

      // first-order recursive smooting filter update:
      for (int i = 0; i < n_ch; i++) {
        AGC_stage[i] = AGC_stage[i] + epsilon * (AGC_in[i] - AGC_stage[i]);
      }

      //
      // TODO(snessnet) - We can definitely make this more compact and probably faster.
      //
      int num_points = 10; // initialize state from 10 points
      std::vector<double> x1(num_points);
      for (int i = num_points-1; i >= 0; i--) {
        x1[i] = AGC_stage[n_ch-i];
      }

      std::vector<double> a1;
      a1.push_back(1-polez1);

      std::vector<double> b1;
      b1.push_back(1.0);
      b1.push_back(polez1 * -1.0);

      std::vector<double> Z_state;
      Z_state.push_back(0.0);

      std::vector<double> junk(num_points);

      // spatially spread it; using diffusion coeffs like in smooth1d
      junk = filter(a1, b1, x1, Z_state);

      // Second filter
      std::vector<double> x2(n_ch);
      for (int i = n_ch; i >= 0; i--) {
        x2[i] = AGC_stage[n_ch-i];
      }

      std::vector<double> a2;
      a2.push_back(1-polez2);

      std::vector<double> b2;
      b2.push_back(1.0);
      b2.push_back(polez2 * -1.0);

      for (unsigned int i=0; i<Z_state.size(); i++) {
        Z_state[i] = Z_state[i] * polez1 / polez2;
      }

      std::vector<double> out(n_ch);
      out = filter(a2,b2,x2,Z_state);

      // Third filter
      std::vector<double> x3(n_ch);
      for (int i = 0; i < n_ch; i++) {
        x3[i] = AGC_stage[i];
      }

      for (unsigned int i=0; i<Z_state.size(); i++) {
        Z_state[i] = Z_state[i] * polez1 / polez2;
      }

      out = filter(a1,a2,x3,Z_state);

      // Copy over to AGC_memory
      for (int i = 0; i < n_ch; i++) {
        CF.AGC_state[mic].AGC_memory[i][stage] = AGC_stage[i];
      }
      if (stage == 1) {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] = AGC_stage[i];
        }
      } else {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] = CF.AGC_state[mic].sum_AGC[i] + AGC_stage[i];
        }
      }
      if (!optimize_for_mono) {
        for (int i = 0; i < n_ch; i++) {
          stage_sum[i] = stage_sum[i] + AGC_stage[i];
        }
      }
    }
  }
}

// From Filter in Marsyas
std::vector<double> CARFAC::filter(std::vector<double> ncoeffs, std::vector<double> dcoeffs, std::vector<double> x, std::vector<double>& state)
{
  std::vector<double> out(x.size());
  int size = x.size();
  int norder = ncoeffs.size();
  int dorder = dcoeffs.size();
  int stateSize = state.size();
  int order = (norder > dorder) ? norder : dorder;
  int i,j;

  for (i = 0; i < size; ++i){
    out[i] = ncoeffs[0] * x[i] + state[0];
    for (j = 0; j < norder - 1; j++) {
      state[j] = ncoeffs[j+1] * x[i] + state[j+1] - dcoeffs[j+1] * out[i];
    }
    for (j = norder - 1; j < stateSize - 1; j++) {
      state[j] = state[j+1] - dcoeffs[j+1] * out[i];
    }
    state[stateSize - 1] = -dcoeffs[order - 1] * out[i];
  }
  return out;

}


std::string
CARFAC::toString()
{
  std::ostringstream oss;
  oss << CF << endl;
  return oss.str();
}
}
