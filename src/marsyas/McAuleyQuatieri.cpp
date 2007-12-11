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

#include "McAuleyQuatieri.h"
#include "peakView.h"

using namespace std;
using namespace Marsyas;

McAuleyQuatieri::McAuleyQuatieri(string name):MarSystem("McAuleyQuatieri", name)
{
	addControls();
}

McAuleyQuatieri::McAuleyQuatieri(const McAuleyQuatieri& a) : MarSystem(a)
{
	ctrl_reset_ = getctrl("mrs_bool/reset");
	ctrl_useGroups_ = getctrl("mrs_bool/useGroups");
	ctrl_useMemory_ = getctrl("mrs_bool/useMemory");
	ctrl_delta_ = getctrl("mrs_real/delta");
}

McAuleyQuatieri::~McAuleyQuatieri()
{
}

MarSystem*
McAuleyQuatieri::clone() const
{
	return new McAuleyQuatieri(*this);
}

void
McAuleyQuatieri::addControls()
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
McAuleyQuatieri::myUpdate(MarControlPtr sender)
{
	MRSDIAG("McAuleyQuatieri.cpp - McAuleyQuatieri:myUpdate");

	MarSystem::myUpdate(sender);
}

void
McAuleyQuatieri::myProcess(realvec& in, realvec& out)
{
	mrs_real dist;
	mrs_natural candidate;
	mrs_natural lastMatched = 0;
	mrs_natural currentTrack;

	out(o,t) = in(o,t);
	peakView outPeakView(out);

	//no memory and no groups !!!!! (Yet)

	mrs_real delta = ctrl_delta_->to<mrs_real>();

	//initial track IDs for peaks in first frame (BIRTH)
	for(mrs_natural n = 0; n < outPeakView.getFrameNumPeaks(0); ++n)
		outPeakView(n, peakView::pkTrack, 0) = (mrs_real) n;

	//all the peaks in the first frame are new tracks (BIRTH)
	currentTrack = outPeakView.getFrameNumPeaks(0); //any new born track will get this track ID

	//iterate over input frames
	for(mrs_natural k=0; k < outPeakView.getNumFrames()-1; ++k)
	{
		lastMatched = 0;

		//iterate over peaks in current frame
		for(mrs_natural n = 0; n < outPeakView.getFrameNumPeaks(k); ++n)
		{
			mrs_real lastdist = MAXREAL;
			candidate = -1;

			// STEP 1
			// find a candidate match on the next frame for each peak (i.e. track) in current frame
			for(mrs_natural m = lastMatched + 1; m < outPeakView.getFrameNumPeaks(k+1); ++m)
			{
				//set track parameter of all peaks of next frame to -1 so we know later
				//which ones were not matched (=> BIRTH of new tracks)
				outPeakView(m, peakView::pkTrack, k+1) = -1.0;
				
				dist = abs(outPeakView(n, peakView::pkFrequency, k) - outPeakView(m, peakView::pkFrequency, k+1));
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
				//only need to confirm if this is not the last peak in current frame
				if(n < outPeakView.getFrameNumPeaks(k)-1)
				{
					//check the next remaining peak in current frame and see if it is a better match for the found candidate
					dist = abs(outPeakView(n+1, peakView::pkFrequency, k) - outPeakView(candidate, peakView::pkFrequency, k+1));
					if (dist < lastdist)
					{
						// it is a better match! Check two additional conditions: 
						if(candidate - 1 > lastMatched)
						{
							if(abs(outPeakView(n, peakView::pkFrequency, k) - outPeakView(candidate-1, peakView::pkFrequency, k+1)) < delta)
							{
								//found a peak to continue the track -> confirm candidate!
								outPeakView(candidate, peakView::pkTrack, k+1) = outPeakView(n, peakView::pkTrack, k);
								lastMatched = candidate;
							}
						}
					}
				}
				else
				{
					//if this was the last peak in current frame, so inherently it was the best match.
					//Candidate is therefore automatically confirmed and can be propagated.
					outPeakView(candidate, peakView::pkTrack, k+1) = outPeakView(n, peakView::pkTrack, k);
					lastMatched = candidate;
				}
			}
		} //end of loop on peaks of current frame

		// STEP 3
		// check for any unmatched peaks in the next frame -> give BIRTH to new tracks!
		for(mrs_natural m = 0; m < outPeakView.getFrameNumPeaks(k+1); ++m)
		{
			if(outPeakView(m, peakView::pkTrack, k+1) == -1.0)
				outPeakView(m, peakView::pkTrack, k+1) = currentTrack++; //BIRTH of new track
		}
	}		
}


