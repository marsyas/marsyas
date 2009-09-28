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

#ifndef MARSYAS_PHASELOCK_H
#define MARSYAS_PHASELOCK_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class PhaseLock
	\ingroup Processing Basic
    \brief 

*/


class PhaseLock: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_beatHypotheses_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_nrPeriodHyps_;
	MarControlPtr ctrl_nrPhasesPerPeriod_;

	mrs_real srcFs_;
	mrs_natural nrPhasesPerPeriod_;
	mrs_natural nrPeriodHyps_;
	mrs_natural hopSize_;
	mrs_natural inductionTime_;
	mrs_natural nInitHyp_;
	mrs_realvec beatHypotheses_;
	mrs_realvec hypSignals_;
	mrs_realvec sum_;
	mrs_natural t_;

	void myUpdate(MarControlPtr sender);

public:
  PhaseLock(std::string name);
	PhaseLock(const PhaseLock& a);
  ~PhaseLock();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
