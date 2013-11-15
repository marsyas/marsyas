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

#include "Spectrum2Mel.h"
#include <algorithm>

using namespace std;
using namespace Marsyas;

Spectrum2Mel::Spectrum2Mel(mrs_string name):MarSystem("Spectrum2Mel", name)
{
  addControls();

  pmelBands_ = 0;
  pbandWidth_ = 0.0;
  pbandLowEdge_ = 0.0;
  pbandHighEdge_ = 0.0;
  phtkMel_ = false;
  pconstAmp_ = false;
}

Spectrum2Mel::Spectrum2Mel(const Spectrum2Mel& a) : MarSystem(a)
{
  ctrl_melBands_ = getctrl("mrs_natural/melBands");
  ctrl_bandWidth_ = getctrl("mrs_real/bandWidth");
  ctrl_bandLowEdge_ = getctrl("mrs_real/bandLowEdge");
  ctrl_bandHighEdge_ = getctrl("mrs_real/bandHighEdge");
  ctrl_htkMel_ = getctrl("mrs_bool/htkMel");
  ctrl_constAmp_ = getctrl("mrs_bool/constAmp");

  melMap_ = a.melMap_;
  pmelBands_ = a.pmelBands_;
  pbandWidth_ = a.pbandWidth_;
  pbandLowEdge_ = a.pbandLowEdge_;
  pbandHighEdge_ = a.pbandHighEdge_;
  phtkMel_ = a.phtkMel_;
  pconstAmp_ = a.pconstAmp_;

}

Spectrum2Mel::~Spectrum2Mel()
{
}

MarSystem*
Spectrum2Mel::clone() const
{
  return new Spectrum2Mel(*this);
}

void
Spectrum2Mel::addControls()
{
  addctrl("mrs_natural/melBands", 40, ctrl_melBands_);
  addctrl("mrs_real/bandWidth", 1.0, ctrl_bandWidth_);
  addctrl("mrs_real/bandLowEdge", 0.0, ctrl_bandLowEdge_);
  addctrl("mrs_real/bandHighEdge", -1.0, ctrl_bandHighEdge_);
  addctrl("mrs_bool/htkMel", false, ctrl_htkMel_);
  addctrl("mrs_bool/constAmp", false, ctrl_constAmp_);

  ctrl_melBands_->setState(true);
  ctrl_bandWidth_->setState(true);
  ctrl_bandLowEdge_->setState(true);
  ctrl_bandHighEdge_->setState(true);
  ctrl_htkMel_->setState(true);
  ctrl_constAmp_->setState(true);
}

void
Spectrum2Mel::myUpdate(MarControlPtr sender)
{
  mrs_natural t,o;
  (void) sender;  //suppress warning of unused parameter(s)

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_melBands_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // allocate memory for melMap_
  // I'm not familiar with this algorithm so this might be
  // wrong!  I'm just closing some memory faults!  -gp
  // size based on its usage in ::myProcess
  onObservations_ = ctrl_onObservations_->to<mrs_natural>();


  if (pmelBands_ != ctrl_melBands_->to<mrs_natural>())
  {
    pmelBands_ = ctrl_melBands_->to<mrs_natural>();
    ostringstream oss;
    for (mrs_natural n=0; n < pmelBands_; n++)
    {
      oss << "MelBand_" << n << ",";
    }
    ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  // calculate the Mel map
  // based in the fft2melmx.m MATLAB script by Dan Ellis
  // http://labrosa.ee.columbia.edu/projects/coversongs/
  //////////////////////////////////////////////////////////////////////////////////////////////
  if(tinObservations_ != inObservations_ ||
      tonObservations_ != onObservations_ ||
      tisrate_ != israte_ ||
      pbandWidth_ != ctrl_bandWidth_->to<mrs_real>() ||
      pbandLowEdge_ != ctrl_bandLowEdge_->to<mrs_real>() ||
      pbandHighEdge_ != ctrl_bandHighEdge_->to<mrs_real>() ||
      phtkMel_ != ctrl_htkMel_->to<mrs_bool>() ||
      pconstAmp_ != ctrl_constAmp_->to<mrs_bool>())
  {
    melMap_.allocate(onObservations_, inObservations_);

    mrs_natural nfilts = ctrl_melBands_->to<mrs_natural>();
    bool htkmel = ctrl_htkMel_->to<mrs_bool>();
    mrs_natural N2 = inObservations_;// we get N/2+1 spectrum points at the input...
    mrs_natural N = (N2-1)*2; //fft size

    //get the original audio sampling rate
    mrs_real srate = israte_*N;

    if(ctrl_bandHighEdge_->to<mrs_real>() == -1.0)
      ctrl_bandHighEdge_->setValue(srate/2.0, NOUPDATE);

    pbandWidth_ = ctrl_bandWidth_->to<mrs_real>();
    pbandLowEdge_ = ctrl_bandLowEdge_->to<mrs_real>();
    pbandHighEdge_ = ctrl_bandHighEdge_->to<mrs_real>();
    phtkMel_ = ctrl_htkMel_->to<mrs_bool>();
    pconstAmp_ = ctrl_constAmp_->to<mrs_bool>();

    //calculate the frequencies (in Hz) for each spectrum bin
    realvec fftfreqs(N2);
    for(o=0; o < N2; ++o) {
      fftfreqs(o) = o / (float)N * srate;
    }

    // 'Center freqs' of mel bands - uniformly spaced between limits
    mrs_real minmel = hertz2mel(ctrl_bandLowEdge_->to<mrs_real>(), htkmel);
    mrs_real maxmel = hertz2mel(ctrl_bandHighEdge_->to<mrs_real>(), htkmel);
    realvec binfrqs(nfilts+2);
    realvec binbin(nfilts+2);
    for(t=0; t < nfilts+2; ++t)
    {
      binfrqs(t) = mel2hertz(minmel + t/(nfilts+1.0)*(maxmel-minmel) , htkmel);
      binbin(t) = floor(binfrqs(t)/srate*(N-1) + 0.5);
    }

    realvec fs(3);
    mrs_real width = ctrl_bandWidth_->to<mrs_real>();
    realvec loslope(N2);
    realvec hislope(N2);

    for(mrs_natural i=0; i < nfilts; ++i)
    {
      for(t=0; t<3; ++t) {
        fs(t) = binfrqs(i+t);
      }
      //cout<<endl;

      //scale by width
      for(t=0; t<3; ++t)
        fs(t) = fs(1)+ width*(fs(t) - fs(1));

      //lower and upper slopes for all bins
      for(t=0; t < N2; ++t)
      {
        loslope(t) = (fftfreqs(t) - fs(0)) / (fs(1) - fs(0));
        hislope(t) = (fs(2) - fftfreqs(t)) / (fs(2) - fs(1));
      }

      // .. then intersect them with each other and zero
      for(t=0; t < N2; ++t)
        melMap_(i,t) = max((mrs_real)0.0, (mrs_real)min(loslope(t), hislope(t)));
    }

    if(ctrl_constAmp_->to<mrs_bool>() == false)
    {
      //Slaney-style mel is scaled to be approx constant E per channel
      mrs_real diagMatrix;
      for(o = 0; o < nfilts; ++o)
      {
        diagMatrix = 2.0 / (binfrqs(o+2) - binfrqs(o));
        for(t=0; t< N2; ++t)
          melMap_(o,t) *= diagMatrix;
      }
    }
  }
}

void
Spectrum2Mel::myProcess(realvec& in, realvec& out)
{
  //input must contain spectral magnitude/power/density/etc
  //(e.g. output of PowerSpectrum MarSystem)
  mrs_natural o,t;
  out.setval(0.0);
  for(t=0; t< inSamples_; ++t)
  {
    for(o=0; o< onObservations_; ++o)
    {
      for(mrs_natural i=0; i< inObservations_; ++i)
      {
        out(o,t)+= in(i,t)*melMap_(o,i);
      }
    }
  }
}
