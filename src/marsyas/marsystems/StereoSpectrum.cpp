/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "StereoSpectrum.h"

using namespace std;
using namespace Marsyas;

StereoSpectrum::StereoSpectrum(mrs_string name):MarSystem("StereoSpectrum",name)
{
  addControls();
}

StereoSpectrum::StereoSpectrum(const StereoSpectrum& a):MarSystem(a)
{

}

StereoSpectrum::~StereoSpectrum()
{
}

void
StereoSpectrum::addControls()
{
}

MarSystem*
StereoSpectrum::clone() const
{
  return new StereoSpectrum(*this);
}

void
StereoSpectrum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  N2_ = ctrl_inObservations_->to<mrs_natural>()/2;
  N4_ = N2_/2+1; //i.e. N/2+1

  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(N4_, NOUPDATE);// N/2+1
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>(), NOUPDATE);

  ostringstream oss;
  for (mrs_natural n=0; n < N4_; n++)
    oss << "stereobin_" << n << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
StereoSpectrum::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural t=0; t < N4_; t++)
  {
    //left channel
    if (t==0) //DC bin (i.e. 0)
    {
      rel_ = in(0,0);
      iml_ = 0.0;
    }
    else if (t == N4_-1) //Nyquist bin (i.e. N/2)
    {
      rel_ = in(1, 0);
      iml_ = 0.0;
    }
    else //all other bins
    {
      rel_ = in(2*t, 0);
      iml_ = in(2*t+1, 0);
    }
    //right channel
    if (t==0) //DC bin (i.e. 0)
    {
      rer_ = in(N2_,0);
      imr_ = 0.0;
    }
    else if (t == N4_-1) //Nyquist bin (i.e. N/2)
    {
      rer_ = in(N2_+1, 0);
      imr_ = 0.0;
    }
    else //all other bins
    {
      rer_ = in(N2_ + 2*t, 0);
      imr_ = in(N2_ + 2*t+1, 0);
    }

    mrs_real f1 = (rel_ * rer_)*(rel_ * rer_);
    mrs_real f2 = (iml_ * imr_)*(iml_ * imr_);
    mrs_real f3 = (iml_ * rer_)*(iml_ * rer_);
    mrs_real f4 = (imr_ * rel_)*(imr_ * rel_);
    mrs_real nf = sqrt(f1 + f2 + f3 + f4);
    mrs_real dfl = rel_*rel_ + iml_*iml_;
    mrs_real dfr = rer_ * rer_ + imr_*imr_;
    mrs_real df = dfl + dfr;

    mrs_real sign;
    if (nf / dfl - nf/dfr > 0)
      sign = 1.0;
    else
      sign = -1.0;

    if (df != 0.0)
      out(t,0) = (1.0 - 2 * (nf / df)) * sign;
    else
      out(t,0) = 0.0;
  }

  //MATLAB_PUT(out, "stereoSpectrum");
  //MATLAB_EVAL("plot(stereoSpectrum)");
}
