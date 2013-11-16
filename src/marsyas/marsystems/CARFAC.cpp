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

void CARFAC::CARFAC_AGCStep(const std::vector<std::vector<double> > &avg_detects)
{
  int n_AGC_stages = (int) CF.AGC_coeffs.AGC_epsilon.size();
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
            agcstep_AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain * CF.AGC_state[mic].AGC_memory[stage - 1][i];
          }
        } else {
          for (int i = 0; i < n_ch; i++) {
            agcstep_AGC_in[i] = CF.AGC_coeffs.AGC_stage_gain *
                                (CF.AGC_coeffs.AGC_mix_coeff * agcstep_prev_stage_mean[i] +
                                 (1 - CF.AGC_coeffs.AGC_mix_coeff) * CF.AGC_state[mic].AGC_memory[stage - 1][i]);
          }
        }
      }

      for (int i = 0; i < n_ch; i++) {
        CF.AGC_state[mic].AGC_memory[stage][i] = CF.AGC_state[mic].AGC_memory[stage][i] + epsilon * (agcstep_AGC_in[i] - CF.AGC_state[mic].AGC_memory[stage][i]);
      }

      DoubleExponentialSmoothing(CF.AGC_state[mic].AGC_memory[stage], polez1, polez2, n_ch);

      if (stage == 0) {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] = CF.AGC_state[mic].AGC_memory[stage][i];
        }
      } else {
        for (int i = 0; i < n_ch; i++) {
          CF.AGC_state[mic].sum_AGC[i] +=  CF.AGC_state[mic].AGC_memory[stage][i];
        }
      }
      if (!optimize_for_mono) {
        for (int i = 0; i < n_ch; i++) {
          agcstep_stage_sum[i] = agcstep_stage_sum[i] +  CF.AGC_state[mic].AGC_memory[stage][i];
        }
      }
    }
  }

}

CARFAC::CARFAC(mrs_string name):MarSystem("CARFAC", name)
{
  addControls();
}

CARFAC::CARFAC(const CARFAC& a) : MarSystem(a)
{
  ctrl_printcoeffs_ = getctrl("mrs_bool/printcoeffs");
  ctrl_printstate_ = getctrl("mrs_bool/printstate");
  ctrl_calculate_binaural_sai_ = getctrl("mrs_bool/calculate_binaural_sai");

  ctrl_sai_output_binaural_sai_ = getctrl("mrs_realvec/sai_output_binaural_sai");
  ctrl_sai_output_threshold_ = getctrl("mrs_realvec/sai_output_threshold");
  ctrl_sai_output_strobes_ = getctrl("mrs_realvec/sai_output_strobes");

  allocateVectors();
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
  // Add specific controls needed by this MarSystem.
  addctrl("mrs_bool/printcoeffs", true, ctrl_printcoeffs_);
  setControlState("mrs_bool/printcoeffs", true);

  addctrl("mrs_bool/printstate", true, ctrl_printstate_);
  setControlState("mrs_bool/printstate", true);

  // Output the Binaural SAI data to a control
  addctrl("mrs_bool/calculate_binaural_sai", false, ctrl_calculate_binaural_sai_);
  setControlState("mrs_bool/calculate_binaural_sai", true);

  // Controls for the Binaural SAI
  addctrl("mrs_natural/sai_width", 100, ctrl_sai_width_);
  setControlState("mrs_natural/sai_width", true);

  addctrl("mrs_real/sai_memory_factor", 0.8, ctrl_sai_memory_factor_);
  setControlState("mrs_real/sai_memory_factor", true);

  addctrl("mrs_bool/sai_summary_itd", false, ctrl_sai_summary_itd_);
  setControlState("mrs_bool/sai_summary_itd", true);

  addctrl("mrs_real/sai_threshold_alpha", 0.9999, ctrl_sai_threshold_alpha_);
  setControlState("mrs_real/sai_threshold_alpha", true);

  addctrl("mrs_real/sai_threshold_jump_factor", 1.5, ctrl_sai_threshold_jump_factor_);
  setControlState("mrs_real/sai_threshold_jump_factor", true);

  addctrl("mrs_real/sai_threshold_jump_offset", 0.1, ctrl_sai_threshold_jump_offset_);
  setControlState("mrs_real/sai_threshold_jump_offset", true);

  // The output of the SAI module
  addctrl("mrs_realvec/sai_output_binaural_sai", realvec(), ctrl_sai_output_binaural_sai_);
  addctrl("mrs_realvec/sai_output_threshold", realvec(), ctrl_sai_output_threshold_);
  addctrl("mrs_realvec/sai_output_strobes", realvec(), ctrl_sai_output_strobes_);
}

// Preallocate any vectors that will get reused over and over.
void CARFAC::allocateVectors() {
  int n_ch = CF.n_ch;
  int n_samp = inSamples_;
  int n_mics = CF.n_mics;
  int decim = CF.CF_AGC_params.decimation;

  // Create the naps array.
  naps.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      naps[i][j].resize(n_mics);
    }
  }

  // Create the prev_naps array
  prev_naps.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    prev_naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      prev_naps[i][j].resize(n_mics);
    }
  }

  // Create the decim_naps array.
  int decim_naps_size = n_samp/decim;
  decim_naps.resize(decim_naps_size);
  for (int i = 0; i < decim_naps_size; i++) {
    decim_naps[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      decim_naps[i][j].resize(n_mics);
    }
  }

  // Create the sai array
  sai.resize(n_samp);
  for (int i = 0; i < n_samp; i++) {
    sai[i].resize(n_ch);
    for (int j = 0; j < n_ch; j++) {
      sai[i][j].resize(n_mics);
    }
  }

  // For AGCStep
  agcstep_prev_stage_mean.resize(n_ch);
  agcstep_stage_sum.resize(n_ch);
  agcstep_AGC_in.resize(n_ch);
  agcstep_AGC_stage.resize(n_ch);
}


void
CARFAC::myUpdate(MarControlPtr sender)
{
  // Binaural SAI
  calculate_binaural_sai_ = getctrl("mrs_bool/calculate_binaural_sai")->to<mrs_bool>();
  sai_width_ = getctrl("mrs_natural/sai_width")->to<mrs_natural>();
  sai_memory_factor_ = getctrl("mrs_real/sai_memory_factor")->to<mrs_real>();
  sai_threshold_alpha_ = getctrl("mrs_real/sai_threshold_alpha")->to<mrs_real>();
  sai_threshold_jump_factor_ = getctrl("mrs_real/sai_threshold_jump_factor")->to<mrs_real>();
  sai_threshold_jump_offset_ = getctrl("mrs_real/sai_threshold_jump_offset")->to<mrs_real>();

  MarControlAccessor acc_on(ctrl_sai_output_binaural_sai_);
  mrs_realvec& sai_output_binaural_sai = acc_on.to<mrs_realvec>();
  // sai_output_binaural_sai.stretch(onObservations_,inSamples_);
  // TODO(snessnet) - Set this correctly via n_ch
  sai_output_binaural_sai.stretch(96,sai_width_*2);

  MarControlAccessor acc_ot(ctrl_sai_output_threshold_);
  mrs_realvec& sai_output_threshold = acc_ot.to<mrs_realvec>();
  sai_output_threshold.stretch(onObservations_,inSamples_);

  MarControlAccessor acc_os(ctrl_sai_output_strobes_);
  mrs_realvec& sai_output_strobes = acc_os.to<mrs_realvec>();
  sai_output_strobes.stretch(onObservations_,inSamples_);

  // Initialize the arrays for the Filters and AGC
  CF.CARFAC_Init(inObservations_);

  MarSystem::myUpdate(sender);

  // TODO(snessnet) - Don't set n_ch here, set it from a control
  int n_ch = 96;
  ctrl_onObservations_->setValue(n_ch * 2, NOUPDATE);

  allocateVectors();

}

void
CARFAC::myProcess(realvec& in, realvec& out)
{
  MarControlAccessor acc_ob(ctrl_sai_output_binaural_sai_);
  mrs_realvec& sai_output_binaural_sai_ = acc_ob.to<mrs_realvec>();

  MarControlAccessor acc_ot(ctrl_sai_output_threshold_);
  mrs_realvec& sai_output_threshold = acc_ot.to<mrs_realvec>();

  MarControlAccessor acc_os(ctrl_sai_output_strobes_);
  mrs_realvec& sai_output_strobes = acc_os.to<mrs_realvec>();

  lastin = in;

  int n_ch = CF.n_ch;
  int n_mics = CF.n_mics;

  int decim_k = -1;

  bool make_decim_naps = false;

  int cum_k = -1;
  int decim = CF.CF_AGC_params.decimation;

  std::vector<double> detect(n_ch);
  std::vector<double> avg_detect(n_ch);
  std::vector<int> threshold_histogram(n_ch,0);

  for (mrs_natural k = 0; k < inSamples_; k++) {
    cum_k = cum_k + 1;
    for (int mic = 0; mic < n_mics; mic++) {
      double input_to_filterstep = in(mic,k);
      // detect = CARFAC_FilterStep(input_to_filterstep,mic);
      CF.filter_state[mic].FilterStep(CF, input_to_filterstep,detect);
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

      CARFAC_AGCStep(avg_detects);
      for (int mic = 0; mic < n_mics; mic++) {
        for (int i = 0; i < n_ch; i++) {
          CF.filter_state[mic].dzB_memory[i] =
            (CF.AGC_state[mic].sum_AGC[i] - CF.filter_state[mic].zB_memory[i]) / decim;
        }
      }
    }

    // Detect strobe points
    for (int mic = 0; mic < n_mics; mic++) {
      int othermic = 1 - mic;
      std::vector<bool> above_threshold(n_ch,false);
      for (int i = 0; i < n_ch; i++) {
        if ((CF.strobe_state[mic].lastdata[i] > CF.strobe_state[mic].thresholds[i]) &&
            (CF.strobe_state[mic].lastdata[i] > naps[k][i][mic])) {
          above_threshold[i] = true;
        } else {
          above_threshold[i] = false;
        }
        if (above_threshold[i]) {
          CF.strobe_state[mic].thresholds[i] = naps[k][i][mic] * sai_threshold_jump_factor_ + sai_threshold_jump_offset_;
        } else {
          CF.strobe_state[mic].thresholds[i] = CF.strobe_state[mic].thresholds[i] * sai_threshold_alpha_;
        }
        CF.strobe_state[mic].lastdata[i] = naps[k][i][mic];

        // Copy the thresholds to the output control
        // TODO(snessnet) - Executive decision to just do first microphone
        if (calculate_binaural_sai_ && mic == 0) {
          sai_output_threshold(i,k) = CF.strobe_state[0].thresholds[i];
          sai_output_strobes(i,k) = above_threshold[i];
        }
      }

      // If above threshold, copy data from the trigger point onwards
      for (int i = 0; i < n_ch; i++) {
        if (above_threshold[i] && mic == 0) {
          threshold_histogram[i]++;
        }

        if (above_threshold[i]) {
          CF.strobe_state[mic].trigger_index[i] = k;
          CF.strobe_state[mic].sai_index[i] = 0;
        }

        if ((CF.strobe_state[mic].sai_index[i] < sai_width_) && (CF.strobe_state[mic].trigger_index[i] < inSamples_)) {
          sai[CF.strobe_state[mic].sai_index[i]][i][mic] = naps[CF.strobe_state[mic].trigger_index[i]][i][othermic] + sai[CF.strobe_state[mic].sai_index[i]][i][mic] * sai_memory_factor_;
        }
        CF.strobe_state[mic].trigger_index[i]++;
        CF.strobe_state[mic].sai_index[i]++;
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

  // Copy the sai data to the output
  if (calculate_binaural_sai_) {
    for (int row = 0; row < n_ch; row++) {
      for (int col = 0; col < sai_width_; col++) {
        sai_output_binaural_sai_(row,sai_width_-col) = sai[col][row][0];
        sai_output_binaural_sai_(row,sai_width_+col) = sai[col][row][1];
      }
    }

    // Save the nap data for the next iteration
    for (int i = 0; i < inSamples_; i++) {
      for (int j = 0; j < n_ch; j++) {
        for (int k = 0; k < n_mics; k++) {
          prev_naps[i][j][k] = naps[i][j][k];
        }
      }
    }
  }

}


void CARFAC::DoubleExponentialSmoothing(std::vector<double> &data, double polez1, double polez2, int n_ch)
{
  double state = 0;
  double input;
  for (int index = n_ch - 10; index < n_ch; index++) {
    input = data[index];
    state = state + (1 - polez1) * (input - state);
  }

  // smooth backward with polez2, starting with state from above:
  for (int index = n_ch - 1; index >= 0; index--) {
    input = data[index];
    state = state + (1 - polez2) * (input - state);
    data[index] = state;
  }

  // smooth forward with polez1, starting with state from above:
  for (int index = 0; index < n_ch; index++) {
    state = state + (1 - polez1) * (data[index] - state);
    data[index] = state;
  }

}


std::string
CARFAC::toString()
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
