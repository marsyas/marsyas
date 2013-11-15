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

#include "ADRess.h"
#include <algorithm>

using std::ostringstream;
using std::min;
using std::max;


using namespace Marsyas;

ADRess::ADRess(mrs_string name):MarSystem("ADRess", name)
{
  addControls();
}

ADRess::ADRess(const ADRess& a) : MarSystem(a)
{
  ctrl_beta_ = getctrl("mrs_natural/beta");
}

ADRess::~ADRess()
{
}

MarSystem*
ADRess::clone() const
{
  return new ADRess(*this);
}

void
ADRess::addControls()
{
  addctrl("mrs_natural/beta", 100, ctrl_beta_);
}

void
ADRess::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  N2_ = inObservations_ / 2; //i.e. we get two vertically stacked spectrums at the input
  N4_ = N2_/2 + 1; //i.e. for each spectrum, we have N/2+1 spectral bins

  ctrl_onSamples_->setValue(1 + ctrl_beta_->to<mrs_natural>()+1, NOUPDATE);//one column for the phases, the others for the panning map [0:beta]
  ctrl_onObservations_->setValue(N4_*2, NOUPDATE); //output data for N/2+1 spectral bins, stacked vertically for each channel
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  for(mrs_natural n=0; n< N4_; ++n)
    oss << "Left_bin_" << n <<",";
  for(mrs_natural n=0; n< N4_; ++n)
    oss << "Right_bin_" << n <<",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
ADRess::myProcess(realvec& in, realvec& out)
{
  mrs_real a, b, g;
  mrs_natural beta = ctrl_beta_->to<mrs_natural>();

  for (mrs_natural k=0; k < N4_; k++)
  {
    minAZr_ = MAXREAL;
    minAZl_ = MAXREAL;
    maxAZr_ = MINREAL;
    maxAZl_ = MINREAL;

    //get left channel spectrum bin
    if (k==0) //DC bin (i.e. 0)
    {
      rel_ = in(0,0);
      iml_ = 0.0;
    }
    else if (k == N4_-1) //Nyquist bin (i.e. N/2)
    {
      rel_ = in(1, 0);
      iml_ = 0.0;
    }
    else //all other bins
    {
      rel_ = in(2*k, 0);
      iml_ = in(2*k+1, 0);
    }

    //get right channel spectrum bin
    if (k==0) //DC bin (i.e. 0)
    {
      rer_ = in(N2_,0);
      imr_ = 0.0;
    }
    else if (k == N4_-1) //Nyquist bin (i.e. N/2)
    {
      rer_ = in(N2_+1, 0);
      imr_ = 0.0;
    }
    else //all other bins
    {
      rer_ = in(N2_ + 2*k, 0);
      imr_ = in(N2_ + 2*k+1, 0);
    }

    //store phases in first column of the output
    out(k,0) = 0;//atan2(iml_, rel_);		//left channel phases
    out(k+N4_, 0) = 0;//atan2(imr_, rer_); //right channel phases

    for(mrs_natural i=0; i <= beta; ++i)
    {
      g = i*1.0/beta;

      //left freq-azimuth map (AZl)
      a = rer_ - g*rel_;
      b = imr_ - g*iml_;
      out(k,i+1) = sqrt(a*a + b*b);
      //get maximums and minimums
      if(out(k,i+1) > maxAZl_)
        maxAZl_ = out(k,i+1);
      if(out(k,i+1) < minAZl_)
        minAZl_ = out(k,i+1);

      //right freq-azimuth map (AZr)
      a = rel_ - g*rer_;
      b = iml_ - g*imr_;
      out(k+N4_,i+1) = sqrt(a*a + b*b);
      //get maximums and minimums
      if(out(k+N4_,i+1) > maxAZr_)
        maxAZr_ = out(k+N4_,i+1);
      if(out(k+N4_,i+1) < minAZr_)
        minAZr_ = out(k+N4_,i+1);
    }

    //compute the magnitudes of the frequency dependent nulls
    for(mrs_natural i=0; i <= beta; ++i)
    {
      //left channel
      if(out(k,i+1)== min(minAZl_, minAZr_))
      {
        out(k,i+1) = max(maxAZl_, maxAZr_) - min(minAZl_,minAZr_);
        //just filter out bins with amplitude inferior to -100dB
        if(20.0*log10(out(k,i+1)*out(k,i+1)+0.000000001)<-100.0)
          out(k,i+1) = 0.0;
      }
      else
        out(k,i+1) = 0.0;
      //right channel
      if(out(k+N4_,i+1) == min(minAZl_, minAZr_))
      {
        out(k+N4_,i+1) = max(maxAZl_, maxAZr_) - min(minAZl_,minAZr_);
        //just filter out bins with amplitude inferior to -100dB
        if(20.0*log10(out(k+N4_,i+1)*out(k+N4_,i+1)+0.000000001)<-100.0)
          out(k+N4_,i+1) = 0.0;
      }
      else
        out(k+N4_,i+1) = 0.0;
    }
  }

// 	MATLAB_PUT(out, "out");
// 	MATLAB_EVAL("AZl = out(1:end/2,2:end);");
// 	MATLAB_EVAL("AZr = out(end/2+1:end,2:end);");
// 	MATLAB_EVAL("panMap = [AZl(:,1:end-1),AZr(:,end:-1:1)];figure(1);imagesc(panMap);");
}
