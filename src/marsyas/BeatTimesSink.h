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

#ifndef MARSYAS_BEATTIMESSINK_H
#define MARSYAS_BEATTIMESSINK_H

#include "MarSystem.h"

namespace Marsyas
{
/** 
    \class BeatTimesSink
	\ingroup Processing Basic
    \brief 
		  
	Controls:
	- \b mrs_natural/nBeats [w] : Nr. of considered beat timming hypotheses (m).
	- \b mrs_natural/nBpms [w] : Nr. of considered beat tempo hypotheses (n).
*/


class BeatTimesSink: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_hopSize_;
	MarControlPtr ctrl_winSize_;
	MarControlPtr ctrl_srcFs_;
	MarControlPtr ctrl_destFileName_;
	MarControlPtr ctrl_mode_;
	MarControlPtr ctrl_tickCount_;
	
	mrs_realvec ibiBPMVec_;
	mrs_natural inc_;
	mrs_string destFile_;
	mrs_string destFile2_;
	mrs_string destFile3_;
	mrs_string mode_;
	mrs_real lastBeatTime_;
	mrs_real ibiBPM_;
	mrs_bool initialOut_;
	mrs_bool initialOut2_;
	mrs_bool initialOut3_;
	mrs_real beatTime_;
	mrs_real srcFs_;
	mrs_real adjustment_;
	mrs_natural hopSize_;
	mrs_natural winSize_;
	mrs_natural t_;
	mrs_real ibiBPMSum_;
	mrs_natural beatCount_;

	void myUpdate(MarControlPtr sender);

public:
  BeatTimesSink(std::string name);
	BeatTimesSink(const BeatTimesSink& a);
  ~BeatTimesSink();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
