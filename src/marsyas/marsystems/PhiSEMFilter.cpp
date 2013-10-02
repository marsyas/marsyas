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

/**
	\class PhiSEMFilter
	\ingroup Synthesis
	\brief Creates noise

	Controls:
	- \b mrs_natural/numFilters	: number of filters to use
	- \b mrs_real/frequences	: frequencies of resonances
	- \b mrs_real/resonances	: resonances of the filtersn
*/

#include "../common_source.h"
#include "PhiSEMFilter.h"


using std::ostringstream;
using std::cout;
using std::endl;


using namespace Marsyas;

PhiSEMFilter::PhiSEMFilter(mrs_string name)
  : MarSystem("PhiSEMFilter", name) {
  output_ = NULL;
  coeffs_ = NULL;
  resVec_.create(10);
  freqVec_.create(10);
  addControls();
}

PhiSEMFilter::~PhiSEMFilter() {
  //delete previously allocated coeffs and outputs
  if ( coeffs_ )
    delete[] coeffs_;
  if ( output_ )
    delete[] output_;
}

MarSystem*
PhiSEMFilter::clone() const {
  return new PhiSEMFilter(*this);
}

void
PhiSEMFilter::addControls() {
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/numFilters", 1, numFilters_);
  setctrlState("mrs_natural/numFilters", true);

  addctrl("mrs_realvec/frequencies", freqVec_);
  setctrlState("mrs_realvec/frequencies", true);

  addctrl("mrs_realvec/resonances", resVec_);
  setctrlState("mrs_realvec/resonances", true);
}

void
PhiSEMFilter::myUpdate(MarControlPtr sender) {
  MRSDIAG("PhiSEMFilter.cpp - PhiSEMFilter::myUpdate()");

  setctrl("mrs_natural/numFilters",	getctrl("mrs_natural/numFilters"));
  freqVec_ = getctrl("mrs_realvec/frequencies")->to<mrs_realvec>();
  resVec_ = getctrl("mrs_realvec/resonances")->to<mrs_realvec>();

  mrs_natural numFilters = getctrl("mrs_natural/numFilters")->to<mrs_natural>();
  //mrs_natural numFilters = numFilters_->to<mrs_natural>();
  if ( numFilters > 10 ) {
    setctrl("mrs_natural/numFilters",	10);
    numFilters = 10;
  }

  //delete previously allocated coeffs and outputs
  if ( coeffs_ )
    delete[] coeffs_;
  if ( output_ )
    delete[] output_;

  coeffs_ = new mrs_realpair[numFilters * 2];
  output_ = new mrs_realpair[numFilters * 2];

  cout << "Num Filters: " << numFilters << endl;
  for(int i=0; i < numFilters; ++i) {
    coeffs_[i].zero = 1.0 - resVec_(i) * 2.0 * cos(freqVec_(i) * TWOPI / israte_);
    coeffs_[i].one = resVec_(i) * resVec_(i);
    output_[i].zero = output_[i].one = 0.0;

    cout << "->Filter " << i << ": freq=" << freqVec_(i)
         << " res=" << resVec_(i)
         << " coeff0=" << coeffs_[i].zero << " coeff1=" << coeffs_[i].one << endl;
  }
  MarSystem::myUpdate(sender);
}

void
PhiSEMFilter::myProcess(realvec& in, realvec& out) {
  mrs_real temp = 0.0;
  mrs_real output = 0.0;
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++ ) {
    for (t=0; t < inSamples_; t++ ) {
      //apply each filter
      for(int i=0; i < numFilters_->to<mrs_natural>(); ++i) {
        temp = in(o,t);
        temp -= output_[i].zero * coeffs_[i].zero;
        temp -= output_[i].one * coeffs_[i].one;
        output_[i].one = output_[i].zero;
        output_[i].zero = temp;
        output += output_[i].zero - output_[i].one;
      }
      out(o,t) = output;
    }
  }
}
