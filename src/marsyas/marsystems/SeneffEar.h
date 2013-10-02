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

#ifndef MARSYAS_SENEFFEAR_H
#define MARSYAS_SENEFFEAR_H

#include <marsyas/system/MarSystem.h>
#include "Filter.h"
#include "Cascade.h"
#include "Parallel.h"

namespace Marsyas
{
/**
    \class SeneffEar
	\ingroup none
    \brief SeneffEar auditory model

    SeneffEar is a perceptually-inspired auditory filterbank.
    This is a direct reimplementation of the corresponding model
    from the Auditory Toolbox in Matlab by Malcolm Slaney.
*/


class SeneffEar: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_natural stage;

  bool firstUpdate;

  mrs_natural channels;
  mrs_real fs;

  realvec PreemphasisRThetaCoeffs;
  realvec FilterBankRThetaCoeffs;
  realvec SeneffPreemphasisCoeffs;
  realvec SeneffFilterBankCoeffs;
  realvec SeneffForwardCoeffs;
  realvec SeneffBackwardCoeffs;
  realvec SeneffForwardCoeffsNormalized;

  realvec y;

  Filter* SeneffPreemphasisFilter;
  Cascade* SeneffFilterBank;
  Parallel* resonatorFilter;

  realvec slice_0;
  realvec slice_1;
  realvec slice_2;
  realvec slice_3;

  mrs_real hwrA;
  mrs_real hwrB;
  mrs_real hwrG;

  mrs_real Tua;
  mrs_real Tub;

  realvec Cn;

  mrs_real lpAlpha;
  Filter* lowPassFilter;

  mrs_real initial_yn;
  mrs_real alpha_agc;
  mrs_real kagc;
  Filter* AGCfilter;

  void polyConv(realvec&, realvec&, realvec&);
  void polyFlip(realvec&);

public:
  SeneffEar(std::string name);
  ~SeneffEar();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

