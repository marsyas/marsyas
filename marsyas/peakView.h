/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
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
\class peakView
\class Notmar
\brief Helper class for accessing peaks in a realvec. 
*/

#ifndef MARSYAS_PEAKVIEW_H
#define MARSYAS_PEAKVIEW_H 

#include "realvec.h"
#include <vector>

namespace Marsyas
{
	class peakView
	{
	public:
		enum pkParameter{
			pkFrequency,
			pkAmplitude,
			pkPhase,
			pkDeltaFrequency,
			pkDeltaAmplitude,
			pkFrame,
			pkGroup,
			pkVolume,
			pkPan,
			pkBinLow,
			pkBin,
			pkBinHigh,
			nbPkParameters
		};

	private:
		realvec& vec_;

		mrs_real fs_;
		mrs_natural frameSize_;
		
		mrs_natural frameMaxNumPeaks_;
		mrs_natural numFrames_;

		void toTable(realvec& vecTable);
		void fromTable(const realvec& vecTable);

	public:
		peakView(realvec& vec);
		~peakView();

		mrs_real getFs() const {return fs_;};
		mrs_natural getFrameSize() const {return frameSize_;};

		mrs_natural getNumFrames() const {return numFrames_;};
		mrs_natural getFrameMaxNumPeaks() const {return frameMaxNumPeaks_;};
		mrs_natural getFrameNumPeaks(const mrs_natural frame=0) const;
		mrs_natural getTotalNumPeaks() const;
		
		void getPeaksParam(std::vector<realvec>& result, const pkParameter param, mrs_natural startFrame = 0, mrs_natural endFrame = 0) const;
		
		mrs_real& operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame=0);
		mrs_real operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame=0) const;

		static std::string getParamName(mrs_natural paramIdx);

		bool peakWrite(std::string filename, mrs_real fs=0, mrs_natural frameSize=0);
		bool peakRead(std::string filename);	
	};

	inline
	mrs_real& peakView::operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame)
	{
		return vec_(peakIndex + param * frameMaxNumPeaks_, frame);
	}

	inline
	mrs_real peakView::operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame) const
	{
		return vec_(peakIndex + param * frameMaxNumPeaks_, frame);
	}

}//namespace Marsyas

#endif /* !MARSYAS_PEAKVIEW_H */

