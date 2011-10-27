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

#include "Yin.h"

using namespace std;
using namespace Marsyas;

Yin::Yin(mrs_string name):MarSystem("Yin", name)
{
  yin_size_ = 0;
  yin_buffer_ = NULL;
  scratch_input_size_ = 0;
  scratch_input_ = NULL;
  addControls();
}

Yin::Yin(const Yin& a) : MarSystem(a)
{
  yin_size_ = 0;
  yin_buffer_ = NULL;
  scratch_input_size_ = 0;
  scratch_input_ = NULL;
  ctrl_tolerance_ = getctrl("mrs_real/tolerance");
}


Yin::~Yin()
{
  delete [] scratch_input_;
  delete [] yin_buffer_;
}

MarSystem*
Yin::clone() const
{
  return new Yin(*this);
}

void
Yin::addControls()
{
  
  // The value of 0.15 was the default in Aubio
  addctrl("mrs_real/tolerance", 0.15, ctrl_tolerance_);

}

void
Yin::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Yin.cpp - Yin:myUpdate");

  MarSystem::myUpdate(sender);

  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

  if (yin_size_ != inSamples_/2) {
    delete [] yin_buffer_;
    yin_size_ = inSamples_/2;
    yin_buffer_ = new mrs_real[yin_size_];
  }
  if (scratch_input_size_ != inSamples_) {
    delete [] scratch_input_;
    scratch_input_size_ = inSamples_;
    scratch_input_ = new mrs_real[scratch_input_size_];
  }

	// Add prefix to the observation names.
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Yin_"), NOUPDATE);
}

double Yin::aubio_quadfrac(double s0, double s1, double s2, double pf) {
  double tmp = s0 + (pf/2.) * (pf * ( s0 - 2.*s1 + s2 ) - 3.*s0 + 4.*s1 - s2);
  return tmp;
}

double Yin::vec_quadint_min(realvec *x,unsigned int pos, unsigned int span) {
  double step = 1./200.;
  /* init resold to - something (in case x[pos+-span]<0)) */
  double res, frac, s0, s1, s2, exactpos = (double)pos, resold = 100000.;
  if ((pos > span) && (pos < x->getSize()-span)) {
    //s0 = (*x)(0,pos-span);
    //s1 = (*x)(0,pos);
    //s2 = (*x)(0,pos+span);
    s0 = (*x)(pos-span);
    s1 = (*x)(pos);
    s2 = (*x)(pos+span);
    /* increase frac */
    for (frac = 0.; frac < 2.; frac = frac + step) {
      res = Yin::aubio_quadfrac(s0, s1, s2, frac);
      if (res < resold) {
        resold = res;
      } else {				
        exactpos += (frac-step)*span - span/2.;
        break;
      }
    }
  }
  return exactpos;
}

unsigned int Yin::vec_min_elem(realvec *s) 
{
  size_t i = 0;
  int pos=0;
  double tmp = (*s)(0,0);
//   for (i=0; i < s->channels; ++i)
  for (mrs_natural j=0; j < s->getSize(); j++) {
	pos = (tmp < (*s)(+6))? pos : j;
	tmp = (tmp < (*s)(i,j))? tmp : (*s)(i,j);
  }
//     }
  return pos;
}

void
Yin::myProcess(realvec& in, realvec& out)
{

  // The tolerance for the yin algorithm
  const mrs_real tol = ctrl_tolerance_->to<mrs_real>();

  mrs_real pitch = -1.0;

//   cout << "yin.getSize()=" << yin.getSize() << endl;
//   cout << "tol=" << tol << endl;

  // Calculate the pitch with the Yin method
  //mrs_natural c=0;
  mrs_real tmp2 = 0.;

  std::fill(yin_buffer_, yin_buffer_+yin_size_, 0.0);
  //yin_buffer_(c,0) = 1.;
  yin_buffer_[0] = 1.;
  // get copy of input to avoid excessive (long,long) lookups
  for (mrs_natural i=0;i<inSamples_;i++) {
    scratch_input_[i] = in(0, i);
  }

  for (mrs_natural tau=1;tau<yin_size_;tau++)
	{
	  for (mrs_natural j=0;j<yin_size_;j++)
		{
		  //mrs_real tmp = in(c,j) - in(c,j+tau);
		  const mrs_real tmp = scratch_input_[j] - scratch_input_[j+tau];
		  //yin_buffer_(c,tau) += tmp * tmp;
		  yin_buffer_[tau] += tmp * tmp;
		}
	  //tmp2 += yin_buffer_(c,tau);
	  //yin_buffer_(c,tau) *= tau/tmp2;
	  tmp2 += yin_buffer_[tau];
	  yin_buffer_[tau] *= tau/tmp2;
	  const mrs_natural period = tau-3;
	  //if(tau > 4 && (yin_buffer_(c,period) < tol) && 
		// (yin_buffer_(c,period) < yin_buffer_(c,period+1))) {
	  if(tau > 4 && (yin_buffer_[period] < tol) && 
		 (yin_buffer_[period] < yin_buffer_[period+1])) {
          // prepare realvec for function
          realvec yin_buffer_realvec(yin_size_);
          for (mrs_natural i=0; i<yin_size_; i++) {
            yin_buffer_realvec(i) = yin_buffer_[i];
          }
		  pitch = vec_quadint_min(&yin_buffer_realvec,period,1);
		  break;
	  }
	}
  if (pitch < 0) {
      // prepare realvec for function
      realvec yin_buffer_realvec(yin_size_);
      for (mrs_natural i=0; i<yin_size_; i++) {
          yin_buffer_realvec(i) = yin_buffer_[i];
      }
	  pitch = vec_quadint_min(&yin_buffer_realvec,
        vec_min_elem(&yin_buffer_realvec),1);
  }
  
  if (pitch !=0)
	  out(0,0) = ctrl_osrate_/pitch; 
  else 
	  out(0,0) = 0.0;

}
