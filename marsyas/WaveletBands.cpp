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

/** 
    \class WaveletBands
    \brief Calculate time-domain wavelet-based bands

    Calculate time-domain wavelet-based bands by 
zeroing out all the coefficients except for the band 
we are interested in and then performing the Inverse 
Wavelet Transform to get back to time domain. 
*/

#include "WaveletBands.h"

using namespace std;
using namespace Marsyas;

WaveletBands::WaveletBands(string name):MarSystem("WaveletBands",name)
{
  iwvpt_ = NULL;
  
  addControls();
}

WaveletBands::~WaveletBands()
{
  delete iwvpt_;
}


// copy constructor 
WaveletBands::WaveletBands(const WaveletBands& a):MarSystem(a)
{
  iwvpt_ = NULL;
}



MarSystem* 
WaveletBands::clone() const 
{
  return new WaveletBands(*this);
}

void 
WaveletBands::addControls()
{
  addctrl("mrs_natural/nBands", 4);
  setctrlState("mrs_natural/nBands", true);
  addctrl("mrs_natural/startBand", 5);
  setctrlState("mrs_natural/startBand", true);
  

}

void
WaveletBands::myUpdate()
{
  
  MRSDIAG("WaveletBands.cpp - WaveletBands:myUpdate");
  mrs_natural nBands = getctrl("mrs_natural/nBands")->toNatural();

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->toNatural() * nBands);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  if (!iwvpt_) 
    iwvpt_ = new WaveletPyramid("iwvpt");    
  

  iwvpt_->setctrl("mrs_bool/forward", false);
  iwvpt_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  iwvpt_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  iwvpt_->updctrl("mrs_real/israte", getctrl("mrs_real/israte"));


  band_.create(getctrl("mrs_natural/inObservations")->toNatural(), 
	       getctrl("mrs_natural/inSamples")->toNatural());
  tband_.create(getctrl("mrs_natural/inObservations")->toNatural(), 
		getctrl("mrs_natural/inSamples")->toNatural());
   
}


void 
WaveletBands::myProcess(realvec& in, realvec& out) 
{
  

  mrs_natural level;
  mrs_natural hlevel, llevel;
  mrs_natural base = getctrl("mrs_natural/startBand")->toNatural();

  for (o = 0; o < onObservations_; o++)
    {
      for (t=0; t < inSamples_; t++)
	{
	  band_(t) = in(0,t);
	}
      level = 7 + o;
      hlevel = base << level;
      llevel = base << (level -1);
      band_.setval(hlevel, inSamples_, 0.0);
      band_.setval(0, llevel, 0.0);
      
      iwvpt_->process(band_, tband_);

      for (t=0; t < inSamples_; t++)
	out(o,t) = tband_(t);
    }
  
  
  
}


	
	
	
