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

#ifndef MARSYAS_BeatAgent_H
#define MARSYAS_BeatAgent_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class BeatAgent
	\ingroup Processing Basic
    \brief Tests Beat Phase + Tempo hypothesis in comparison to
	the actual onsetdetection function.
	Output Format: [Beat/Eval/None|Tempo|PrevBeat|Inner/Outter|Error|Score]

	- \b mrs_natural/phase = Given beat phase hypothesis (in frames);
	- \b mrs_natural/tempobpm_ = Given tempo hypothesis (in BPM);
	- \b mrs_real/srcFs [w] : Input sampling rate of the sound file source 
	(given by "SoundFileSource/src/mrs_real/israte") -> by default = 1.0Hz.
	- \b mrs_natural/hopSize [w] : hopsize of the analysis -> by default = 1.

*/


class BeatAgent: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_period_;
	MarControlPtr ctrl_phase_;
	MarControlPtr ctrl_identity_;
	MarControlPtr ctrl_timming_;
	MarControlPtr ctrl_isNewOrUpdated_;
	MarControlPtr ctrl_agentControl_;
	MarControlPtr ctrl_scoreFunc_;
	MarControlPtr ctrl_lftOutterMargin_;
	MarControlPtr ctrl_rgtOutterMargin_;
	MarControlPtr ctrl_innerMargin_;

	mrs_real lftOutterMargin_;
	mrs_real rgtOutterMargin_;
	mrs_real innerMargin_;
	mrs_string scoreFunc_;
	mrs_natural innerWin_;
	mrs_natural outterWinLft_;
	mrs_natural outterWinRgt_;
	mrs_bool isNewOrUpdated_;
	mrs_string identity_;
	mrs_real fraction_;
	mrs_real score_;
	mrs_natural error_;
	mrs_natural curBeat_;
	mrs_natural prevBeat_;
	mrs_natural beatCount_;
	mrs_natural period_;
	mrs_natural ibi_;
	mrs_natural phase_;
	mrs_natural t_;
	mrs_natural lastBeatPoint_;
	mrs_real curBeatPointValue_;
	mrs_realvec history_;
	mrs_natural myIndex_;
	mrs_realvec agentControl_;

	void fillOutput(realvec& out, mrs_real flag, mrs_real tempo, mrs_real phase, 
					mrs_real tolerance, mrs_real error, mrs_real score);
	mrs_natural getChildIndex();
	void myUpdate(MarControlPtr sender);
	mrs_real calcDScore(realvec& in);
	mrs_real calcDScoreCorrSquare(realvec& in);

public:
  BeatAgent(std::string name);
	BeatAgent(const BeatAgent& a);
  ~BeatAgent();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
