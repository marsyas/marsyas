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

#ifndef MARSYAS_INDUCTIONGT_H
#define MARSYAS_INDUCTIONGT_H

#include "MarSystem.h"
using namespace std;

namespace Marsyas
{
/** 
    \class TextFileSources
	\ingroup Processing
    \brief Retrieves N first beat times (in frames), retrieved from
	peaks from an onset function.
	Supposed to have PeakerOnsets, or similar, as input.

	- \b mrs_natural/n1stOnsets [w] : Nr. of first beat times
	- \b mrs_natural/accSamples [w] : Nr. of accumulated frames -> corresponds to 
	the winSize used in ShiftInput as signal memory for analysis.
	- \b mrs_natural/lookAheadSamples [w] : Nr. of frames to adjust the lookahead 
	used in PeakerOnset
*/


class InductionGT: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_sourceFile_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_hopSize_;
	MarControlPtr ctrl_srcFs_;

	mrs_natural hopSize_;
	mrs_real srcFs_;
	mrs_natural inductionTime_;
	mrs_string line_;
	ifstream inStream_;
	mrs_natural t_;
	mrs_real maxScore_;
	mrs_natural ibi_;
	mrs_real beatTime1_;
	mrs_real beatTime2_;
	mrs_string sourceFile_;
	mrs_natural phase_;
	void myUpdate(MarControlPtr sender);

public:
  InductionGT(std::string name);
	InductionGT(const InductionGT& a);
  ~InductionGT();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
