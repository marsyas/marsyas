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

#include "EnhADRess.h"
#include "../common_source.h"

using std::ostringstream;
using std::abs;

using namespace Marsyas;

EnhADRess::EnhADRess(mrs_string name):MarSystem("EnhADRess", name)
{
  addControls();
}

EnhADRess::EnhADRess(const EnhADRess& a) : MarSystem(a)
{

}

EnhADRess::~EnhADRess()
{
}

MarSystem*
EnhADRess::clone() const
{
  return new EnhADRess(*this);
}

void
EnhADRess::addControls()
{
}

void
EnhADRess::myUpdate(MarControlPtr sender)
{
  MRSDIAG("EnhADRess.cpp - EnhADRess:myUpdate");
  (void) sender;  //suppress warning of unused parameter(s)

  N2_ = inObservations_ / 2; //i.e. we get two vertically stacked spectrums at the input
  N4_ = N2_/2 + 1; //i.e. for each spectrum, we have N/2+1 spectral bins

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(N4_*3, NOUPDATE); //output data for N/2+1 spectral bins, stacked vertically for Mag, phase and pan
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  for(mrs_natural n=0; n< N4_; ++n)
  {
    oss << "EnhADRess_Mag_bin_" << n <<",";
  }
  for(mrs_natural n=0; n< N4_; ++n)
  {
    oss << "EnhADRess_Phase_bin_" << n <<",";
  }
  for(mrs_natural n=0; n< N4_; ++n)
  {
    oss << "EnhADRess_Pan_bin_" << n <<",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
EnhADRess::myProcess(realvec& in, realvec& out)
{
  out.setval(0.0);

  for(mrs_natural t=0; t < inSamples_; ++t)
  {
    for (mrs_natural k=0; k < N4_; k++)
    {
      //get left channel spectrum bin
      if (k==0) //DC bin (i.e. 0)
      {
        rel_ = in(0,t);
        iml_ = 0.0;
      }
      else if (k == N4_-1) //Nyquist bin (i.e. N/2)
      {
        rel_ = in(1, t);
        iml_ = 0.0;
      }
      else //all other bins
      {
        rel_ = in(2*k, t);
        iml_ = in(2*k+1, t);
      }

      //get right channel spectrum bin
      if (k==0) //DC bin (i.e. 0)
      {
        rer_ = in(N2_,t);
        imr_ = 0.0;
      }
      else if (k == N4_-1) //Nyquist bin (i.e. N/2)
      {
        rer_ = in(N2_+1, t);
        imr_ = 0.0;
      }
      else //all other bins
      {
        rer_ = in(N2_ + 2*k, t);
        imr_ = in(N2_ + 2*k+1, t);
      }

      phaseL_ = atan2(iml_, rel_);		//left channel phases
      phaseR_ = atan2(imr_, rer_); //right channel phases

      deltaPhase_ = abs(phaseL_ - phaseR_);

      //wrap phase into the 0~2*PI range
      deltaPhase_ = (mrs_real)fmod((double)deltaPhase_, (double)2*PI);

      //left amplitude value
      Lk_ = sqrt(rel_*rel_ + iml_*iml_);

      //right amplitude value
      Rk_ = sqrt(rer_*rer_ + imr_*imr_);

      if(deltaPhase_ < PI/2)
      {
        minLk_ = Lk_ * sin(deltaPhase_);
        minRk_ = Rk_ * sin(deltaPhase_);

        if(Lk_ < Rk_) // i.e. minLk < minRk --> sound panned right
        {
          //store magnitude to output
          mrs_real mag = Rk_ - minLk_;
          // bins with amplitude inferior to -100dB are discarded
          if(20.0*log10(mag*mag+0.000000001) > -100.0)
          {
            out(k,t) = mag;

            //store phase to output
            out(k+N4_,t) = phaseR_;

            //store azimuth to output
            out(k+N4_*2,t) = 1.0 - Lk_ * cos(deltaPhase_) / Rk_ ; //-1.0-> L; 0.0-> C;
          }
        }
        else if(Lk_ > Rk_) // i.e. minLk > minRk --> sound panned left
        {
          //store magnitude to output
          mrs_real mag = Lk_ - minRk_;
          // bins with amplitude inferior to -100dB are discarded
          if(20.0*log10(mag*mag+0.000000001) > -100.0)
          {
            out(k,t) = mag;

            //store phase to output
            out(k+N4_,t) = phaseL_;

            //store azimuth to output
            out(k+N4_*2,t) = Rk_ * cos(deltaPhase_) / Lk_ -1.0; //0.0 -> C; 1.0-> R;
          }
        }
        else if(Lk_ == Rk_) //sound panned at the CENTER
        {
          //store magnitude to output
          mrs_real mag = Lk_ - minRk_;
          // bins with amplitude inferior to -100dB are discarded
          if(20.0*log10(mag*mag+0.000000001) > -100.0)
          {
            out(k,t) = mag;

            //store phase to output
            out(k+N4_,t) = phaseL_; //could have been phaseR_...

            //store azimuth to output
            out(k+N4_*2,t) = 0.0; //0.0 -> C;
          }
        }
      }
      else
      {
        if(20.0*log10(Lk_*Lk_+0.000000001) < -100.0)
          Lk_ = 0.0;
        if(20.0*log10(Rk_*Rk_+0.000000001) < -100.0)
          Rk_ = 0.0;

        if(Lk_ > Rk_)
        {
          out(k,t) = Lk_;
          out(k+N4_,t) = phaseL_;
          out(k+N4_*2,t) = 0.0; //-1.0;
        }
        else if(Rk_ > Lk_)
        {
          out(k,t) = Rk_;
          out(k+N4_,t) = phaseR_;
          out(k+N4_*2,t) = 0.0; //1.0;
        }
        else if(Lk_ == Rk_ && Lk_ != 0.0)
        {
          out(k,t) = Lk_;
          out(k+N4_,t) = phaseL_;
          out(k+N4_*2,t) = 0.0;
        }
      }
    }
  }
  //MATLAB_PUT(out, "out");
  //MATLAB_EVAL("figure(1);plot(out);figure(2)");
  //MATLAB_EVAL("plot(out(length(out)/3*2+1:end))")
}
