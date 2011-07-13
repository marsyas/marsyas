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

#include "MarSystem.h"
#include "CARFAC_coeffs.h"

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

class CARFAC: public MarSystem
{

 private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_printcoeffs_;
  MarControlPtr ctrl_printstate_;

  std::vector<double> CARFAC_FilterStep(double input_waves, int mic);

  void CARFAC_AGCStep(std::vector<std::vector<double> > &avg_detects);

  CF_class CF;

 public:
  CARFAC(std::string name);
  CARFAC(const CARFAC& a);
  ~CARFAC();
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


  std::string toString();
  void printParams();
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
