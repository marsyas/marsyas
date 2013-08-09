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

#include "ADRessSpectrum.h"


using std::ostringstream;
using std::abs;

using namespace Marsyas;

ADRessSpectrum::ADRessSpectrum(mrs_string name):MarSystem("ADRessSpectrum", name)
{
  addControls();
  N2_ = 0;
}

ADRessSpectrum::ADRessSpectrum(const ADRessSpectrum& a) : MarSystem(a)
{
  N2_ = a.N2_;
  ctrl_d_ = getctrl("mrs_real/d");
  ctrl_H_ = getctrl("mrs_real/H");
}

ADRessSpectrum::~ADRessSpectrum()
{
}

MarSystem*
ADRessSpectrum::clone() const
{
  return new ADRessSpectrum(*this);
}

void
ADRessSpectrum::addControls()
{
  addctrl("mrs_real/d", 0.5, ctrl_d_);
  addctrl("mrs_real/H", 0.5, ctrl_H_);
}

void
ADRessSpectrum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  mrs_real pN2 = N2_;

  //left and right channels ADRess info is stacked vertically in the input
  N2_ = ctrl_inObservations_->to<mrs_natural>() / 2; // = N/2+1 spectrum points for each channel

  ctrl_onSamples_->setValue(1, NOUPDATE);
  //output a complex spectrum for a single channel
  ctrl_onObservations_->setValue(((N2_-1)*2)); //N

  if(pN2 != N2_)
  {
    ostringstream oss;
    //Left channel
    oss << "ADRessSpectrum_rbin_0" << ","; //DC bin (only has real part)
    oss << "ADRessSpectrum_rbin_" << N2_-1 << ","; // = N/2, i.e. Nyquist bin (only has real part)
    for (mrs_natural n=2; n < N2_; n++)
    {
      oss << "ADRessSpectrum_rbin_" << n-1 << ",";
      oss << "ADRessSpectrum_ibin_" << n-1 << ",";
    }
    ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
  }

  beta_ = ctrl_onSamples_->to<mrs_natural>()-2;
}

void
ADRessSpectrum::myProcess(realvec& in, realvec& out)
{
  mrs_natural t;
  out.setval(0.0);

  //output spectrum of the "selected" source, given d and H
  mrs_natural H = (mrs_natural)(beta_* ctrl_H_->to<mrs_natural>());
  if(H < 0)
  {
    H = 0;
    ctrl_H_->setValue(0.0);
  }
  if(H > beta_)
  {
    H = beta_;
    ctrl_H_->setValue(1.0);
  }

  mrs_natural H2 = H/2;

  mrs_natural d = (mrs_natural)(beta_*ctrl_d_->to<mrs_real>());
  if(d < 0)
  {
    d = 0;
    ctrl_d_->setValue(0.0);
  }
  if(d > beta_)
  {
    d = beta_;
    ctrl_d_->setValue(1.0);
  }

  mrs_real mag = 0;
  mrs_real phase = 0;
  mrs_real azim = 0;


  for(mrs_natural k=0; k < N2_; ++k)
  {
    //get magnitude, phase and azimuth of bin k from input
    mag = 0.0;
    for(t=0; t <= beta_; ++t)
    {
      //search for non-zero values in azimuth plane
      azim = -1;
      if(in(k,t+1) > 0.0)
      {
        azim = t;
        mag = in(k,t+1);
        phase = in(k, 0);
        break;
      }
      if(in(k+N2_,t+1) > 0.0)
      {
        azim = beta_*2-t;
        mag = in(k+N2_,t+1);
        phase = in(k+N2_, 0);
        break;
      }
    }

    if(azim < 0)
    {
      //no sound at this bin,
      //so do not send anything to output
      continue;
    }

    //check if bin is inside specified range,
    //otherwise, send nothing to output
    if(abs(d-azim) <= H2)
    {
      //convert back to rectangular form
      re_ = mag*cos(phase);
      im_ = mag*sin(phase);

      //write bin to output
      if (k==0)
      {
        out(0,0) = re_; //DC
      }
      else if (k == N2_-1)
      {
        out(1, 0) = re_; //Nyquist
      }
      else
      {
        out(2*k, 0) = re_;  //all other bins
        out(2*k+1, 0) = im_;
      }
    }
  }
}
