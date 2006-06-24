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
    \class WaveletPyramid
    \brief Pyramid wavelet algorithm

   Discrete Wavelet Transform (DWT) pyramid algorithm.
Based on the Numerical Recipies wavelet code. 
*/

#include "WaveletPyramid.h"

using namespace std;
using namespace Marsyas;

WaveletPyramid::WaveletPyramid(string name)
{
  type_ = "WaveletPyramid";
  name_ = name;
  waveletStep_ = NULL;
  addControls();

}

WaveletPyramid::~WaveletPyramid()
{
  delete waveletStep_;
}


// copy constructor 
WaveletPyramid::WaveletPyramid(const WaveletPyramid& a)
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
  waveletStep_ = NULL;
}



MarSystem* 
WaveletPyramid::clone() const 
{
  return new WaveletPyramid(*this);
}

void 
WaveletPyramid::addControls()
{
  addDefaultControls();
  addctrl("mrs_bool/forward", true);

  delete waveletStep_;
  waveletStep_ = new Daub4("daub4");
}

void
WaveletPyramid::update()
{
  
  MRSDIAG("WaveletPyramid.cpp - WaveletPyramid:update");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  waveletStep_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  waveletStep_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  waveletStep_->updctrl("mrs_real/israte", getctrl("mrs_real/israte"));
		       

  defaultUpdate();
  
}


void 
WaveletPyramid::process(realvec& in, realvec& out) 
{
  checkFlow(in,out);
  
  mrs_natural nn;
  mrs_natural n;
  bool forward;
  
  
  n = getctrl("mrs_natural/inSamples").toNatural();
  if (n < 4) return;


  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
      }  

  forward = getctrl("mrs_bool/forward").toBool();
  waveletStep_->setctrl("mrs_bool/forward", (MarControlValue)forward);
  if (forward) 
    {
      for (nn= n; nn >=4; nn >>=1) 
	{
	  waveletStep_->setctrl("mrs_natural/processSize",nn);
	  waveletStep_->process(out, out);	  
	}
    }
  else 
    {
      for (nn=4; nn <= n; nn <<=1)
	{
	  waveletStep_->setctrl("mrs_natural/processSize", nn);
	  waveletStep_->process(out, out);
	}
    }
  
  
}


	
	
