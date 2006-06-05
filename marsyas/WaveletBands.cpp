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


WaveletBands::WaveletBands(string name)
{
  type_ = "WaveletBands";
  name_ = name;
  iwvpt_ = NULL;
  addControls();
}

WaveletBands::~WaveletBands()
{
  delete iwvpt_;
}


// copy constructor 
WaveletBands::WaveletBands(const WaveletBands& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  
  inSamples_ = a.inSamples_;
  inObservations_ = a.inObservations_;
  onSamples_ = a.onSamples_;
  onObservations_ = a.onObservations_;
  dbg_ = a.dbg_;
  mute_ = a.mute_;
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
  addDefaultControls();
  addctrl("natural/nBands", 4);
  setctrlState("natural/nBands", true);
  addctrl("natural/startBand", 5);
  setctrlState("natural/startBand", true);

  delete iwvpt_;
  iwvpt_ = new WaveletPyramid("iwvpt");

}

void
WaveletBands::update()
{
  
  MRSDIAG("WaveletBands.cpp - WaveletBands:update");
  natural nBands = getctrl("natural/nBands").toNatural();

  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations").toNatural() * nBands);
  setctrl("real/osrate", getctrl("real/israte"));


  iwvpt_->setctrl("bool/forward", (MarControlValue)false);
  
  iwvpt_->updctrl("natural/inSamples", getctrl("natural/inSamples"));
  iwvpt_->updctrl("natural/inObservations", getctrl("natural/inObservations"));
  iwvpt_->updctrl("real/israte", getctrl("real/israte"));


  band_.create(getctrl("natural/inObservations").toNatural(), 
	       getctrl("natural/inSamples").toNatural());
  tband_.create(getctrl("natural/inObservations").toNatural(), 
		getctrl("natural/inSamples").toNatural());
  

  defaultUpdate();
  
}


void 
WaveletBands::process(realvec& in, realvec& out) 
{
  checkFlow(in,out);
  

  natural level;
  natural hlevel, llevel;
  natural base = getctrl("natural/startBand").toNatural();

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


	
	
	
