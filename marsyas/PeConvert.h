/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \class PeConvert
    \brief PeConvert

    PeConvert N real and imaginary spectrum values to 
N/2+1 pairs of magnitude and phase values. The phases 
are unwrapped and successive phase differences are 
used to compute estimates of the instantaneous frequencies 
for each phase vocoder analysis channel; decimation rate 
and sampling rate are used to render these frequencies 
directly in Hz. 
*/

#ifndef MARSYAS_PECONVERT_H
#define MARSYAS_PECONVERT_H

#include "MarSystem.h"	

namespace Marsyas
{

class PeConvert: public MarSystem
{
private:
  realvec lastphase_;
  realvec phase_;
  realvec mag_;
	  realvec magCorr_;
	realvec frequency_;
  realvec lastmag_;
	realvec lastfrequency_;
  realvec deltamag_;
	realvec deltafrequency_;

  mrs_real fundamental_;
  mrs_real factor_;
  mrs_real cuttingFrequency_;
  mrs_natural time_;
  mrs_natural nbParameters_;
  mrs_natural nbPeaks_;
  mrs_natural kmax_;
  mrs_natural size_, psize_, skip_;
  
  void addControls();
	void getBinInterval(realvec& interval, realvec& index, realvec& mag);
	void getShortBinInterval(realvec& interval, realvec& index, realvec& mag);
  void getLargeBinInterval(realvec& interval, realvec& index, realvec& mag);
	void myUpdate(MarControlPtr sender);
  
public:
  PeConvert(std::string name);
  
  ~PeConvert();
  MarSystem* clone() const;    

  void myProcess(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif

	

	

	
