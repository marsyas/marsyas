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

#ifndef MARSYAS_TEMPOHYPOTHESES_H
#define MARSYAS_TEMPOHYPOTHESES_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class TempoHypotheses
	\ingroup Processing Basic
    \brief 
		  Organizes N x 2 matrix with tempo hypotheses as [ BPMi  | Beatj  ]
		  where N = m * n.		     					  | BPMi  | Beatj+1|
														  |  ...  |  ...   |
														  | BPMi  | Beatn  |
														  |BPMi+1 | Beatj  |
														  |BPMi+1 | Beatj+1|
														  |  ...  |  ...   |
														  | BPMm  | Beatj  |
														  |  ...  |  ...   | 
														  [ BPMm  | Beatn  ]
	Controls:
	- \b mrs_natural/nBeats [w] : Nr. of considered beat timming hypotheses (m).
	- \b mrs_natural/nBpms [w] : Nr. of considered beat tempo hypotheses (n).
*/


class TempoHypotheses: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_nPhases_;
	MarControlPtr ctrl_nPeriods_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_srcFs_;
	MarControlPtr ctrl_hopSize_;

	mrs_natural hopSize_;
	mrs_real srcFs_;
	mrs_natural inductionTime_;
	mrs_natural t_;
	mrs_natural nPhases_;
	mrs_natural nPeriods_;
	mrs_bool noBPMs_;
	void myUpdate(MarControlPtr sender);

public:
  TempoHypotheses(std::string name);
	TempoHypotheses(const TempoHypotheses& a);
  ~TempoHypotheses();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
