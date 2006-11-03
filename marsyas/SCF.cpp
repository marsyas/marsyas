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
    \class Spectral Crest Factor
    \brief Spectral Crest Factor
    
    Spectral Crest Factor is a feature set very similar to the
    Spectral Flatness Measure defined in the MPEG-7 audio standard.
    Although it is not defined in the MPEG7 standard, its calculation
    is here based on the same definitions used for the MPEG7 SFM:
    1/4 octave frequency resolution, resulting in 24 frequency bands between
    250Hz and 16kHz.
    Based on code provided by Luis Gustavo Martins <lmartins@inescporto.pt>
*/

#include "SCF.h"

using namespace std;
using namespace Marsyas;

SCF::SCF(string name):MarSystem("SCF",name)
{
  //type_ = "SCF";
  //name_ = name;
}

SCF::~SCF()
{
}

MarSystem* 
SCF::clone() const 
{
  return new SCF(*this);
}

void
SCF::myUpdate()
{
	MRSDIAG("SCF.cpp - SCF:myUpdate");

	//MPEG-7 audio standard:
  //assumes an 1/4 octave frequency resolution,
  //resulting in 24 frequency bands between 250Hz and 16kHz.
  //If the signal under analysis does not contain frequencies
  //above a determined value (e.g. due to signal sampling rate or
  //bandwidth limitations), the nr of bands should be reduced. 
  
  //nrBands_ = getctrl("mrs_natural/nrbands");// can this be received as a control value?
  nrBands_ = 24;
  //can this parameter be dinamically modified, depending on the
  //sampling frequency?!?
  nrValidBands_ = nrBands_; 

  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_natural/onObservations", (mrs_natural)nrBands_); 
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural i;

  edge_.create(nrBands_ + 1);
  bandLoEdge_.create(nrBands_);
  bandHiEdge_.create(nrBands_);

  //nominal band edges (Hz) -> reference = 1kHz (MPEG7)
  for(i = 0 ; i < nrBands_ + 1 ; i++)
    {
      // 1/4 octave resolution (MPEG7)
      edge_(i)= 1000.0f * pow(2.0f, (0.25f * (i - 8))); 
    }
  // overlapped low and high band edges (Hz)
  for (i = 0; i < nrBands_; i++)
    {
      bandLoEdge_(i) = edge_(i) * 0.95f; //band overlapping (MPEG7) 
      bandHiEdge_(i) = edge_(i+1) * 1.05f; //band overlapping (MPEG7)
    }

  fftSize_ = getctrl("mrs_natural/inObservations")->toNatural();
  //fftBinFreqs_.create(fftSize_);

  // spectrum sampling rate - not audio 
  df_ = getctrl("mrs_real/israte")->toReal();

  //calculate the frequency (Hz) of each FFT bin
  //for (mrs_natural k=0; k < fftSize_ ; k++)
  //	fftBinFreqs_(k) = (float) k * df_;

  //calculate FFT bin indexes for each band's edges
  il_.resize(nrBands_);
  ih_.resize(nrBands_);
  for(i = 0; i < nrBands_; i++)
    {
      //round to nearest int (MPEG7)
      il_[i] = (mrs_natural)(bandLoEdge_(i)/df_ + 0.5f); 
      ih_[i] = (mrs_natural)(bandHiEdge_(i)/df_ + 0.5f); 
 
      //must verify if sampling rate is enough
      //for the specified nr of bands. If not, 
      //reduce nr of valid freq. bands
      if(ih_[i] >= fftSize_/2) //marsyas FFT returns fftSize/2 points  
	{
	  nrValidBands_ = i;
	  il_.resize(nrValidBands_);
	  ih_.resize(nrValidBands_);
	  break;
	}
    }
}


void 
SCF::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  
  mrs_natural i, k, bandwidth;
  double c, maxc;
  double aritMean ;

  //default SCF value = 1.0; (defines SCF=1.0 for silence)
  out.setval(1.0);
  
  //MPEG7 defines a grouping mechanism for the frequency bands above 1KHz
  //in order to reduce computational effort of the following calculation.
  //For now such grouping mechanism is not implemented...
  for(i = 0; i < nrValidBands_; i++)
    {
      aritMean = 0.0;
      maxc = 0.0;
	bandwidth = ih_[i] - il_[i] + 1;
	for(k = il_[i]; k <= ih_[i]; k++)
	  {
	    c = in(k); //power spectrum coef
	    aritMean += c / bandwidth;
	    if(c > maxc)
	      maxc = c;
	  }
	if (aritMean != 0.0)
	  {
	    out(i) = (float)(maxc/aritMean);
	  }
	//else //mean power = 0 => silence...
	//  out(i) = 1.0; //as used for the SFM (MPEG-7)...
    }
  //for freq bands above the nyquist freq
  //return SFM value defined in MPEG7 for silence
  //for(i = nrValidBands_; i < nrBands_; i++)
  //	out(i) = 1.0;
}







	

	

	
