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

#ifndef MARSYAS_CARFAC_H
#define MARSYAS_CARFAC_H

#include <marsyas/system/MarSystem.h>
#include "CARFAC_coeffs.h"

namespace Marsyas
{
/**
	\ingroup Processing Certified
	\brief CAR-FAC model of auditory processing in the cochlea.
*/

class CARFAC: public MarSystem
{

private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_printcoeffs_;
  MarControlPtr ctrl_printstate_;

  MarControlPtr ctrl_calculate_binaural_sai_;

  MarControlPtr ctrl_sai_width_;
  MarControlPtr ctrl_sai_memory_factor_;
  MarControlPtr ctrl_sai_summary_itd_;
  MarControlPtr ctrl_sai_threshold_alpha_;
  MarControlPtr ctrl_sai_threshold_jump_factor_;
  MarControlPtr ctrl_sai_threshold_jump_offset_;

  MarControlPtr ctrl_sai_output_binaural_sai_;
  MarControlPtr ctrl_sai_output_threshold_;
  MarControlPtr ctrl_sai_output_strobes_;

  bool calculate_binaural_sai_;
  int sai_width_;
  double sai_memory_factor_;
  double sai_threshold_alpha_;
  double sai_threshold_jump_factor_;
  double sai_threshold_jump_offset_;

  void DoubleExponentialSmoothing(std::vector<double> &data, double polez1, double polez2, int n_ch);
  void CARFAC_AGCStep(const std::vector<std::vector<double> > &avg_detects);

public:
  CARFAC(std::string name);
  CARFAC(const CARFAC& a);
  ~CARFAC();
  MarSystem* clone() const;

  realvec lastin;

  std::vector<std::vector<std::vector<double> > > naps;
  std::vector<std::vector<std::vector<double> > > prev_naps;
  std::vector<std::vector<std::vector<double> > > decim_naps;
  std::vector<std::vector<std::vector<double> > > sai;

  std::vector<double> agcstep_prev_stage_mean;
  std::vector<double> agcstep_stage_sum;
  std::vector<double> agcstep_AGC_in;
  std::vector<double> agcstep_AGC_stage;

  void allocateVectors();

  // TODO(snessnet) - should be private...
  CF_class CF;

  std::string toString();
  void printParams();
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
