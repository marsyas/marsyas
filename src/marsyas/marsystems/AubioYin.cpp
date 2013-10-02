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

#include "AubioYin.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

AubioYin::AubioYin(mrs_string name):MarSystem("AubioYin", name)
{
  addControls();
}

AubioYin::AubioYin(const AubioYin& a) : MarSystem(a)
{
  ctrl_tolerance_ = getctrl("mrs_real/tolerance");
}


AubioYin::~AubioYin()
{
}

MarSystem*
AubioYin::clone() const
{
  return new AubioYin(*this);
}

void
AubioYin::addControls()
{

  // The value of 0.15 was the default in Aubio
  addctrl("mrs_real/tolerance", 0.15, ctrl_tolerance_);

}

void
AubioYin::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AubioYin.cpp - AubioYin:myUpdate");

  MarSystem::myUpdate(sender);

  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

}

double AubioYin::aubio_quadfrac(double s0, double s1, double s2, double pf) {
  double tmp = s0 + (pf/2.) * (pf * ( s0 - 2.*s1 + s2 ) - 3.*s0 + 4.*s1 - s2);
  return tmp;
}

double AubioYin::vec_quadint_min(realvec *x,unsigned int pos, unsigned int span) {
  double step = 1./200.;
  /* init resold to - something (in case x[pos+-span]<0)) */
  double res, frac, s0, s1, s2, exactpos = (double)pos, resold = 100000.;
  if ((pos > span) && (pos < x->getSize()-span)) {
    s0 = (*x)(0,pos-span);
    s1 = (*x)(0,pos);
    s2 = (*x)(0,pos+span);
    /* increase frac */
    for (frac = 0.; frac < 2.; frac = frac + step) {
      res = AubioYin::aubio_quadfrac(s0, s1, s2, frac);
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

unsigned int AubioYin::vec_min_elem(realvec *s)
{
  int i = 0;
  int j = 0;
  int pos=0;
  double tmp = (*s)(0,0);
//   for (i=0; i < s->channels; ++i)
  for (j=0; j < s->getSize(); j++) {
    pos = (tmp < (*s)(i,j))? pos : j;
    tmp = (tmp < (*s)(i,j))? tmp : (*s)(i,j);
  }
//     }
  return pos;
}

void
AubioYin::myProcess(realvec& in, realvec& out)
{
  mrs_natural c=0;

  // Make a temporary realvec to build up the yin function in
  // sness - This is very inefficient - Move to update function
  realvec yin((mrs_natural)(inSamples_/2.0));

  // The tolerance for the yin algorithm
  mrs_real tol = ctrl_tolerance_->to<mrs_real>();

  double pitch = -1.0;

//   cout << "yin.getSize()=" << yin.getSize() << endl;
//   cout << "tol=" << tol << endl;

  // Calculate the pitch with the Yin method
  int j,tau = 0;
  int period;
  double tmp = 0., tmp2 = 0.;
  yin(c,0) = 1.;
  for (tau=1; tau<yin.getSize(); tau++)
  {
// 	  cout << "tau=" << tau << endl;
    yin(c,tau) = 0.;
    for (j=0; j<yin.getSize(); j++)
    {
      tmp = in(c,j) - in(c,j+tau);
      yin(c,tau) += tmp * tmp;
    }
    tmp2 += yin(c,tau);
    yin(c,tau) *= tau/tmp2;
    period = tau-3;
    if(tau > 4 && (yin(c,period) < tol) &&
        (yin(c,period) < yin(c,period+1))) {
      pitch = vec_quadint_min(&yin,period,1);
      break;
    }
  }
  if (pitch < 0) {
    pitch = vec_quadint_min(&yin,vec_min_elem(&yin),1);
  }

  out(0,0) = ctrl_osrate_/pitch;

}
