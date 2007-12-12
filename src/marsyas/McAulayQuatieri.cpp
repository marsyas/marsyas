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

#include "McAulayQuatieri.h"
#include "peakView.h"

using namespace std;
using namespace Marsyas;

McAulayQuatieri::McAulayQuatieri(string name):MarSystem("McAulayQuatieri", name)
{
	addControls();
}

McAulayQuatieri::McAulayQuatieri(const McAulayQuatieri& a) : MarSystem(a)
{
	ctrl_reset_ = getctrl("mrs_bool/reset");
	ctrl_useGroups_ = getctrl("mrs_bool/useGroups");
	ctrl_useMemory_ = getctrl("mrs_bool/useMemory");
	ctrl_delta_ = getctrl("mrs_real/delta");
}

McAulayQuatieri::~McAulayQuatieri()
{
}

MarSystem*
McAulayQuatieri::clone() const
{
	return new McAulayQuatieri(*this);
}

void
McAulayQuatieri::addControls()
{
	addctrl("mrs_bool/reset", false);
	setctrlState("mrs_bool/reset", true);

	addctrl("mrs_bool/useMemory", false);
	//setctrlState("mrs_bool/useMemory", true);

	addctrl("mrs_bool/useGroups", false);
	//setctrlState("mrs_bool/useGroups", true);

	addctrl("mrs_real/delta", 0.5); //[TODO][!]
}

void
McAulayQuatieri::myUpdate(MarControlPtr sender)
{
	MRSDIAG("McAulayQuatieri.cpp - McAulayQuatieri:myUpdate");

	MarSystem::myUpdate(sender);	

 	if(ctrl_reset_->to<mrs_bool>())
 	{
 		ctrl_reset_->setValue(false, NOUPDATE);
 		memory_.stretch(0);
 	}
}

void
McAulayQuatieri::myProcess(realvec& in, realvec& out)
{
	mrs_real dist;
	mrs_natural candidate;
	mrs_natural lastMatched = 0;
	mrs_natural nextTrack;
	realvec* outPtr;

	mrs_real delta = ctrl_delta_->to<mrs_real>();

	out(o,t) = in(o,t);
	
	//if memory is not empty and set to be used...
	if(ctrl_useMemory_->to<mrs_bool>() && memory_.getSize() != 0)
	{
		//concatenate memory column vector with current input
		//so we can continue peak tracking from previous input
		tmp_.stretch(onObservations_, onSamples_+1);
		for(o = 0; o < onObservations_; ++o)
			tmp_(o, 0) = memory_(o);
		for(o = 0; o < onObservations_; ++o)
			for(c = 0; c < onSamples_; ++c)
				tmp_(o,c+1) = in(o,c);
		outPtr = &tmp_;
	}
	else
	{
		//no need to concatenate memory information with
		//current input. Just do it inplace in the output realvec (avoid extra copy)!
		outPtr = &out;
	}
		
	peakView tmpPeakView(*outPtr);

	//if no memory being used (or no memory stored yet), we must use peaks in
	//first frame to give birth to new tracks
	if(!ctrl_useMemory_->to<mrs_bool>() || memory_.getSize() == 0)
	{
		for(mrs_natural n = 0; n < tmpPeakView.getFrameNumPeaks(0); ++n)
			tmpPeakView(n, peakView::pkTrack, 0) = (mrs_real) n;
	}
	
	//get the trackID for any future track to be born (in STEP 3 - see below)
	nextTrack = tmpPeakView.getFrameNumPeaks(0);

		//iterate over input frames
	for(mrs_natural k=0; k < tmpPeakView.getNumFrames()-1; ++k)
	{
		lastMatched = 0;

		//iterate over peaks in current frame
		for(mrs_natural n = 0; n < tmpPeakView.getFrameNumPeaks(k); ++n)
		{
			mrs_real lastdist = MAXREAL;
			candidate = -1;

			// STEP 1
			// find a candidate match on the next frame for each peak (i.e. track) in current frame
			for(mrs_natural m = lastMatched + 1; m < tmpPeakView.getFrameNumPeaks(k+1); ++m)
			{
				//set track parameter of all peaks of next frame to -1 so we know later
				//which ones were not matched (=> BIRTH of new tracks)
				tmpPeakView(m, peakView::pkTrack, k+1) = -1.0;
				
				dist = abs(tmpPeakView(n, peakView::pkFrequency, k) - tmpPeakView(m, peakView::pkFrequency, k+1));
				if (dist < delta && dist < lastdist)
				{
					//found a candidate!
					lastdist  = dist;
					candidate = m;
				}
			}

			// STEP 2
			// must confirm candidate (if any)
			if(candidate >= 0) //check if a candidate was found
			{
				//confirm if this is not the last peak in current frame
				if(n < tmpPeakView.getFrameNumPeaks(k)-1)
				{
					//check the next remaining peak in current frame and see if it is a better match for the found candidate
					dist = abs(tmpPeakView(n+1, peakView::pkFrequency, k) - tmpPeakView(candidate, peakView::pkFrequency, k+1));
					if(dist < lastdist)
					{
						// it is a better match! Check two additional conditions: 
						// 1. an unmatched lower freq candidate should exist
						// 2. it is inside the frequency interval specified by delta
						if(candidate - 1 > lastMatched)
						{
							if(abs(tmpPeakView(n, peakView::pkFrequency, k) - tmpPeakView(candidate-1, peakView::pkFrequency, k+1)) < delta)
							{
								//found a peak to continue the track -> confirm candidate!
								tmpPeakView(candidate-1, peakView::pkTrack, k+1) = tmpPeakView(n, peakView::pkTrack, k);
								lastMatched = candidate-1;
							}
						}
					}
					else
					{
						//no better match than this one, so confirm candidate!
						tmpPeakView(candidate, peakView::pkTrack, k+1) = tmpPeakView(n, peakView::pkTrack, k);
						lastMatched = candidate;
					}
				}
				else
				{
					//if this was the last peak in current frame, so inherently it was the best match.
					//Candidate is therefore automatically confirmed and can be propagated.
					tmpPeakView(candidate, peakView::pkTrack, k+1) = tmpPeakView(n, peakView::pkTrack, k);
					lastMatched = candidate;
				}
			}
		} //end of loop on peaks of current frame

		// STEP 3
		// check for any unmatched peaks in the next frame and give BIRTH to new tracks!
		for(mrs_natural m = 0; m < tmpPeakView.getFrameNumPeaks(k+1); ++m)
		{
			if(tmpPeakView(m, peakView::pkTrack, k+1) == -1.0)
				tmpPeakView(m, peakView::pkTrack, k+1) = nextTrack++; //BIRTH of new track
		}
	}	

	//if using memory...
	if(ctrl_useMemory_->to<mrs_bool>())
	{
		if(memory_.getSize() != 0)
		{
			//if using a non-empty memory, we should now fill the trackID parameters
			//computed above (and stored in the tmp realvec) into the actual output
			peakView outPeakView(out);
			for(c = 0; c < outPeakView.getNumFrames(); ++c)
				for(o = 0; o < outPeakView.getFrameNumPeaks(c); ++o)
					outPeakView(o, peakView::pkTrack, c) = tmpPeakView(o, peakView::pkTrack, c+1);
		}
		
		//store the last frame of current output for next time 
		memory_.stretch(onObservations_);
		for(o = 0; o < onObservations_; ++o)
			memory_(o) = out(o, onSamples_-1); 
	}
}


