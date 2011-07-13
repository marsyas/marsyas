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

#ifndef MARSYAS_BinauralCARFAC_H
#define MARSYAS_BinauralCARFAC_H

#include "MarSystem.h"
#include "CARFAC_coeffs.h"
namespace Marsyas
{
/**
   \ingroup Processing Basic Certified
   \brief Multiply input realvec with a fixed value.

   Multiply all the values of the input realvec with
   mrs_real/gain and put them in the output vector.

   Controls:
   - \b mrs_real/gain [w] : adjust the gain multiplier.

*/

class BinauralCARFAC: public MarSystem
{

 private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_printcoeffs_;
  MarControlPtr ctrl_printstate_;

  std::vector<double> BinauralCARFAC_FilterStep(double input_waves, int mic);

  void BinauralCARFAC_AGCStep(std::vector<std::vector<double> > avg_detects);
  std::vector<double> filter(std::vector<double> a, std::vector<double>b, std::vector<double> x, std::vector<double>& state);

 public:
  BinauralCARFAC(std::string name);
  BinauralCARFAC(const BinauralCARFAC& a);
  ~BinauralCARFAC();
  MarSystem* clone() const;

  realvec lastin;


  //
  // Vectors that are reused in the filter, FilterStep and AGCStep
  // functions.  Create them just once and reuse them.
  //
  std::vector<double> filter1_a;
  std::vector<double> filter1_b;
  std::vector<double> filter1_x;
  std::vector<double> filter1_Z_state;
  std::vector<double> filter1_junk;

  std::vector<double> filter2_a;
  std::vector<double> filter2_b;
  std::vector<double> filter2_x;
  std::vector<double> filter2_Z_state;
  std::vector<double> filter2_out;

  std::vector<double> filter3_a;
  std::vector<double> filter3_b;
  std::vector<double> filter3_x;
  std::vector<double> filter3_Z_state;
  std::vector<double> filter3_out;

  std::vector<std::vector<std::vector<double> > > naps;
  std::vector<std::vector<std::vector<double> > > decim_naps;
  std::vector<std::vector<std::vector<double> > > sai;

  // int sai_width = 100;

  std::vector<double> filterstep_inputs;
  std::vector<double> filterstep_zA;
  std::vector<double> filterstep_zB;
  std::vector<double> filterstep_zY;
  std::vector<double> filterstep_r;
  std::vector<double> filterstep_z1;
  std::vector<double> filterstep_z2;
  std::vector<double> filterstep_detect;

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
