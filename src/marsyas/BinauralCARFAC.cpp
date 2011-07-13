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

#include "BinauralCARFAC.h"

using std::cout;
using std::endl;
using std::ostream;

namespace Marsyas
{



std::vector<double> BinauralCARFAC::BinauralCARFAC_FilterStep(double input_waves, int mic)
{
  // Use each stage previous Y as input to next.
  filterstep_inputs[0] = input_waves;

  for (unsigned int i=0; i < CF.filter_state[mic].zY_memory.size()-1; i++) {
    filterstep_inputs[i+1] = CF.filter_state[mic].zY_memory[i];
  }

  // AGC interpolation.
  for (int i=0; i < CF.n_ch; i++) {
    CF.filter_state[mic].zB_memory[i] = CF.filter_state[mic].zB_memory[i] + CF.filter_state[mic].dzB_memory[i];
    filterstep_r[i] = CF.filter_coeffs.r_coeffs[i] - CF.filter_coeffs.c_coeffs[i] * (CF.filter_state[mic].zA_memory[i] + CF.filter_state[mic].zB_memory[i]);

    // Now reduce filter_state by r and rotate with the fixed cos/sin
    // coeffs.
    double z1_tmp = filterstep_r[i] * (CF.filter_coeffs.a_coeffs[i] * CF.filter_state[mic].z1_memory[i] -
                                       CF.filter_coeffs.c_coeffs[i] * CF.filter_state[mic].z2_memory[i]);
    double z1_mem = CF.filter_state[mic].z1_memory[i];
    CF.filter_state[mic].z1_memory[i] = z1_tmp + filterstep_inputs[i];
    CF.filter_state[mic].z2_memory[i] = filterstep_r[i] * (CF.filter_coeffs.c_coeffs[i] * z1_mem +
              CF.filter_coeffs.a_coeffs[i] * CF.filter_state[mic].z2_memory[i]);
  }

  // Update the "velocity" for cubic nonlinearity, into zA.
  for (int i=0; i<CF.n_ch; i++) {
    CF.filter_state[mic].zA_memory[i] = pow(((CF.filter_state[mic].z2_memory[i] - CF.filter_state[mic].z2_memory[i]) * CF.filter_coeffs.velocity_scale), 2);
  }

  // Simulate Sigmoidal OHC effect on damping.
  for (int i=0; i<CF.n_ch; i++) {
    CF.filter_state[mic].zA_memory[i] = (1 - pow((1 - CF.filter_state[mic].zA_memory[i]), 4)) / 4;  // soft max at 0.25
  }

  // Get outputs from inputs and new z2 values.
  for (int i=0; i<CF.n_ch; i++) {
    CF.filter_state[mic].zY_memory[i] = CF.filter_coeffs.g_coeffs[i] * (filterstep_inputs[i] + CF.filter_coeffs.h_coeffs[i] * CF.filter_state[mic].z2_memory[i]);
  }

  // TODO(dicklyon): Generalize to a detection nonlinearity.
  double maxval = 0.0;
  for (int i=0; i<CF.n_ch; i++) {
    filterstep_detect[i] = CF.filter_state[mic].zY_memory[i] > maxval ? CF.filter_state[mic].zY_memory[i] : maxval;
  }

  for (int i=0; i<CF.n_ch; i++) {
    CF.filter_state[mic].detect_accum[i] = CF.filter_state[mic].detect_accum[i] + filterstep_detect[i];
  }

  return filterstep_detect;
}

void BinauralCARFAC::BinauralCARFAC_AGCStep(std::vector<std::vector<double> > avg_detects)
{
  int n_AGC_stages = CF.AGC_coeffs.AGC_epsilon.size();
  int n_mics = CF.n_mics;
  int n_ch = CF.n_ch;

  bool optimize_for_mono = (n_mics == 1) ? true : false;

  for (int stage = 0; stage < n_AGC_stages; stage++) {

    if (!optimize_for_mono) {
      if (stage > 0) {
        for (int i = 0; i < n_ch; i++) {
          agcstep_prev_stage_mean[i] = agcstep_stage_sum[i] / n_mics;
        }
      }
      for (int i = 0; i < n_ch; i++) {
          agcstep_stage_sum[i] = 0; // sum accumulating over mics at this stage
      }
    }
    double epsilon = CF.AGC_coeffs.AGC_epsilon[stage];
    double polez1 = CF.AGC_coeffs.AGC1_polez[stage];
    double polez2 = CF.AGC_coeffs.AGC2_polez[stage];

    for (int mic = 0; mic < n_mics; mic++) {
      if (stage == 0) {
        for (int i = 0; i < n_ch; i++) {
          agcstep_AGC_in[i] = CF.AGC_coeffs.detect_scale * avg_detects[i][mic];
        }
      } else {
        if (optimize_for_mono) {
          // Mono optimization ignores AGC_mix_coeff,
          // assuming all(agcstep_prev_stage_mean == AGC_memory(:, stage - 1));
          // but we also don't even allocate or compute the sum or mean.
          for (int i = 0; i < n_ch; i++) {
            agcstep_AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain * CF.AGC_state[mic].AGC_memory[i][stage - 1];
          }
        } else {
          for (int i = 0; i < n_ch; i++) {
            agcstep_AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain *
                (CF.AGC_coeffs.AGC_mix_coeff * agcstep_prev_stage_mean[i] +
                 (1 - CF.AGC_coeffs.AGC_mix_coeff) * CF.AGC_state[mic].AGC_memory[i][stage - 1]);
          }
        }
      }

      for (int i = 0; i < n_ch; i++) {
        agcstep_AGC_stage[i] = CF.AGC_state[mic].AGC_memory[i][stage];
      }

      // first-order recursive smooting filter update:
      for (int i = 0; i < n_ch; i++) {
        agcstep_AGC_stage[i] = agcstep_AGC_stage[i] + epsilon * (agcstep_AGC_in[i] - agcstep_AGC_stage[i]);
      }

      int npts = n_ch;
      double state = 0;
      double input;
      for (int index = npts - 10; index < npts; index++) {
        input = agcstep_AGC_stage[index];
        state = state + (1 - polez1) * (input - state);
      }

      // smooth backward with polez2, starting with state from above:
      for (int index = npts; index >= 0; index--) {
        input = agcstep_AGC_stage[index];
        state = state + (1 - polez2) * (input - state);
        agcstep_AGC_stage[index] = state;
      }

      // smooth forward with polez1, starting with state from above:
      for (int index = 0; index < npts; index++) {
        state = state + (1 - polez1) * (agcstep_AGC_stage[index] - state);
        agcstep_AGC_stage[index] = state;
      }

      // Copy over to AGC_memory
      for (int i = 0; i < n_ch; i++) {
        CF.AGC_state[mic].AGC_memory[i][stage] = agcstep_AGC_stage[i];
      }
      if (stage == 0) {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] = agcstep_AGC_stage[i];
        }
      } else {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] += agcstep_AGC_stage[i];
        }
      }
      if (!optimize_for_mono) {
        for (int i = 0; i < n_ch; i++) {
          agcstep_stage_sum[i] = agcstep_stage_sum[i] + agcstep_AGC_stage[i];
        }
      }
    }
  }

}

// From Filter in Marsyas
std::vector<double> BinauralCARFAC::filter(std::vector<double> ncoeffs, std::vector<double> dcoeffs, std::vector<double> x, std::vector<double>& state)
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

//////////


BinauralCARFAC::BinauralCARFAC(mrs_string name):MarSystem("BinauralCARFAC", name)
{
	//Add any specific controls needed by BinauralCARFAC
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(),
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
	addControls();
}

BinauralCARFAC::BinauralCARFAC(const BinauralCARFAC& a) : MarSystem(a)
{
	// For any MarControlPtr in a MarSystem
	// it is necessary to perform this getctrl
	// in the copy constructor in order for cloning to work
	ctrl_printcoeffs_ = getctrl("mrs_bool/printcoeffs");
    allocateVectors();
}

BinauralCARFAC::~BinauralCARFAC()
{
}

MarSystem*
BinauralCARFAC::clone() const
{
	return new BinauralCARFAC(*this);
}

void
BinauralCARFAC::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_bool/printcoeffs", true, ctrl_printcoeffs_);
  setControlState("mrs_bool/printcoeffs", true);

  addctrl("mrs_bool/printstate", true, ctrl_printstate_);
  setControlState("mrs_bool/printstate", true);
}

// Preallocate any vectors that will get reused over and over.
void BinauralCARFAC::allocateVectors() {
  int num_points = 10; // initialize state from 10 points
  int n_ch = CF.n_ch;
  int n_samp = inSamples_;
  int n_mics = CF.n_mics;
  int decim = CF.CF_AGC_params.decimation;

  filter1_a.resize(1);
  filter1_b.resize(2);
  filter1_x.resize(num_points);
  filter1_Z_state.resize(1);
  filter1_junk.resize(num_points);

  filter2_a.resize(1);
  filter2_b.resize(2);
  filter2_x.resize(n_ch);
  filter2_Z_state.resize(1);
  filter2_out.resize(n_ch);

  filter3_a.resize(1);
  filter3_b.resize(2);
  filter3_x.resize(n_ch);
  filter3_Z_state.resize(1);
  filter3_out.resize(n_ch);

  // Create the naps array
  naps.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      naps[i][j].resize(n_mics);
    }
  }

  // Create the decim_naps array
  int decim_naps_size = n_samp/decim;
  decim_naps.resize(decim_naps_size);
  for (int i = 0; i < decim_naps_size; i++) {
    decim_naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      decim_naps[i][j].resize(n_mics);
    }
  }

  // Create the sai array
  naps.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      naps[i][j].resize(n_mics);
    }
  }

  // For FilterStep
  filterstep_inputs.resize(n_ch);
  filterstep_zA.resize(n_ch);
  filterstep_zB.resize(n_ch);
  filterstep_zY.resize(n_ch);
  filterstep_r.resize(n_ch);
  filterstep_z1.resize(n_ch);
  filterstep_z2.resize(n_ch);
  filterstep_detect.resize(n_ch);

  // For AGCStep
  agcstep_prev_stage_mean.resize(n_ch);
  agcstep_stage_sum.resize(n_ch);
  agcstep_AGC_in.resize(n_ch);
  agcstep_AGC_stage.resize(n_ch);
}


void
BinauralCARFAC::myUpdate(MarControlPtr sender)
{
  // no change to network flow
  MarSystem::myUpdate(sender);

  int n_ch = 96;
  ctrl_onObservations_->setValue(n_ch * 2, NOUPDATE);

  allocateVectors();

  // CF.BinauralCARFAC_Design();
  // CF.BinauralCARFAC_DesignFilters(CF.CF_filter_params, CF.fs, CF.pole_freqs);
  // CF.BinauralCARFAC_DesignAGC(CF.fs);

  // // TODO(snessnet) - This should get updated from inObservations
  // int n_mics = 2;

  // CF.BinauralCARFAC_Init(n_mics);

  // printcoeffs = false;
  // printstate = false;

}

void
BinauralCARFAC::myProcess(realvec& in, realvec& out)
{
  lastin = in;

  int n_ch = CF.n_ch;
  int n_mics = CF.n_mics;

  int decim_k = -1;

  bool make_decim_naps = false;

  int cum_k = -1;
  int decim = CF.CF_AGC_params.decimation;

  std::vector<double> detect;
  std::vector<double> avg_detect(n_ch);

  for (mrs_natural k = 0; k < inSamples_; k++) {
    cum_k = cum_k + 1;
    for (int mic = 0; mic < n_mics; mic++) {
      double input_to_filterstep = in(mic,k);
      detect = BinauralCARFAC_FilterStep(input_to_filterstep,mic);
      for (unsigned int i=0; i < detect.size(); i++) {
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

      // Run the AGC update step
      BinauralCARFAC_AGCStep(avg_detects);

      // Connect the feedback from AGC_state to filter_state:
      for (int mic = 0; mic < n_mics; mic++) {
        for (int i = 0; i < n_ch; i++) {
          CF.filter_state[mic].dzB_memory[i] =
              (CF.AGC_state[mic].sum_AGC[i] - CF.filter_state[mic].zB_memory[i]) / decim;
        }
      }
    }

    // // Detect strobe points
    // double threshold_alpha = 0.95;
    // double threshold_jump = 1.5;
    // double threshold_offset = 0.01;
    // for (int mic = 0; mic < n_mics; mic++) {
    //   std::vector<double> above_threshold(n_ch);
    //   for (int i = 0; i < n_ch; i++) {
    //     if naps[k][i][mic] >
    //   }
    // }

  }

  // Copy the nap data to the output
  for (int row = 0; row < n_ch; row++) {
    for (int col = 0; col < inSamples_; col++) {
      out(row,col) = naps[col][row][0];
      out(row+n_ch,col) = naps[col][row][1];
    }
  }
}


std::string
BinauralCARFAC::toString()
{
  std::ostringstream oss;

  CF.printcoeffs = getctrl("mrs_bool/printcoeffs")->to<mrs_bool>();
  CF.printstate = getctrl("mrs_bool/printstate")->to<mrs_bool>();

  if (lastin.getSize() > 0) {
    cout << "signal";

    oss.precision(5);
    oss.flags(std::ios::fixed);

    for (int i = 0; i < 10; i++) {
      cout << lastin(0,i) << " ";
    }
    cout << endl;
  }

  oss.precision(4);
  oss.flags(std::ios::scientific);
  oss << CF << endl;

  return oss.str();
}
}
