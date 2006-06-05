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
    \class MFCC
    \brief MFCC Mel-Frequency Cepstral Coefficients

    Mel-Frequency cepstral coefficients are features frequently 
used in Speech Recognition. The code is based on the correspdonging 
function in the Auditory Toolbox by Malcolm Slaney. 
*/

#ifndef MARSYAS_MFCC_H
#define MARSYAS_MFCC_H

#include "MarSystem.h"	


class MFCC: public MarSystem
{
private: 
  void addControls();
  
  real lowestFrequency_;
  natural linearFilters_;
  real linearSpacing_;
  natural logFilters_;
  real logSpacing_;
  natural totalFilters_;

  natural fftSize_, pfftSize_;
  natural samplingRate_, psamplingRate_;
  natural cepstralCoefs_;


  
  
  realvec freqs_;
  realvec lower_;
  realvec center_;
  realvec upper_;
  realvec triangle_heights_;

  realvec fftFreqs_;
  realvec mfccFilterWeights_;
  realvec mfccDCT_;
  realvec earMagnitude_;
  realvec fmagnitude_;

  bool init_;
  


public:
  MFCC(std::string name);
  ~MFCC();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

#endif

	
