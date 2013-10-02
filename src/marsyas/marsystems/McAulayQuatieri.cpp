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
#include "../common_source.h"

#include <marsyas/peakView.h>
#include <marsyas/NumericLib.h>

using std::ostringstream;
using std::abs;

using namespace Marsyas;

McAulayQuatieri::McAulayQuatieri(mrs_string name):MarSystem("McAulayQuatieri", name)
{
  addControls();
  nextGroup_ = 0;
}

McAulayQuatieri::McAulayQuatieri(const McAulayQuatieri& a) : MarSystem(a)
{
  ctrl_reset_ = getctrl("mrs_bool/reset");
  ctrl_useGroups_ = getctrl("mrs_bool/useGroups");
  ctrl_useMemory_ = getctrl("mrs_bool/useMemory");
  ctrl_delta_ = getctrl("mrs_real/delta");
  ctrl_matchThres_ = getctrl("mrs_real/matchThres");

  nextGroup_ = a.nextGroup_;
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
  addctrl("mrs_real/matchThres", 0.5);//[TODO][!]
}

void
McAulayQuatieri::myUpdate(MarControlPtr sender)
{
  MRSDIAG("McAulayQuatieri.cpp - McAulayQuatieri:myUpdate");

  MarSystem::myUpdate(sender);

  if(ctrl_reset_->to<mrs_bool>())
  {
    ctrl_reset_->setValue(false, NOUPDATE);
    memory_.stretch(0,0);
    nextGroup_ = 0;
  }
}

mrs_real
McAulayQuatieri::peakTrack(realvec& vec, mrs_natural frame, mrs_natural grpOne, mrs_natural grpTwo)
{
  mrs_real dist;
  mrs_natural candidate;
  mrs_natural lastMatched = -1;
  mrs_natural matchedTracks = 0;

  mrs_real delta = ctrl_delta_->to<mrs_real>();

  if(frame+1 >= vec.getCols())
  {
    MRSERR("McAulayQuatieri::peakTrack - frame index is bigger than the input vector!");
    return -1.0;
  }

  peakView tmpPeakView(vec);

  //get the trackID for any future track to be born (in STEP 3 - see below)
  mrs_natural nextTrack = tmpPeakView.getFrameNumPeaks(0, grpOne);

  //iterate over peaks in current frame
  for(mrs_natural n = 0; n < tmpPeakView.getFrameNumPeaks(frame, grpOne); ++n)
  {
    mrs_real lastdist = MAXREAL;
    candidate = -1;

    // STEP 1
    // find a candidate match on the next frame for each peak (i.e. track) in current frame
    for(mrs_natural m = lastMatched + 1; m < tmpPeakView.getFrameNumPeaks(frame+1, grpTwo); ++m)
    {
      //set track parameter of all peaks of next frame to -1 so we know later
      //which ones were not matched (=> BIRTH of new tracks)
      tmpPeakView(m, peakView::pkTrack, frame+1, grpTwo) = -1.0;

      dist = abs(tmpPeakView(n, peakView::pkFrequency, frame, grpOne) - tmpPeakView(m, peakView::pkFrequency, frame+1, grpTwo));
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
      if(n < tmpPeakView.getFrameNumPeaks(frame, grpOne)-1)
      {
        //check the next remaining peak in current frame and see if it is a better match for the found candidate
        dist = abs(tmpPeakView(n+1, peakView::pkFrequency, frame, grpOne) - tmpPeakView(candidate, peakView::pkFrequency, frame+1, grpTwo));
        if(dist < lastdist)
        {
          // it is a better match! Check two additional conditions:
          // 1. an unmatched lower freq candidate should exist
          // 2. it is inside the frequency interval specified by delta
          if(candidate - 1 > lastMatched)
          {
            if(abs(tmpPeakView(n, peakView::pkFrequency, frame, grpOne) - tmpPeakView(candidate-1, peakView::pkFrequency, frame+1, grpTwo)) < delta)
            {
              //found a peak to continue the track -> confirm candidate!
              tmpPeakView(candidate-1, peakView::pkTrack, frame+1, grpTwo) = tmpPeakView(n, peakView::pkTrack, frame, grpOne);
              lastMatched = candidate-1;
              matchedTracks++;
            }
          }
        }
        else
        {
          //no better match than this one, so confirm candidate!
          tmpPeakView(candidate, peakView::pkTrack, frame+1, grpTwo) = tmpPeakView(n, peakView::pkTrack, frame, grpOne);
          lastMatched = candidate;
          matchedTracks++;
        }
      }
      else
      {
        //if this was the last peak in current frame, so inherently it was the best match.
        //Candidate is therefore automatically confirmed and can be propagated.
        tmpPeakView(candidate, peakView::pkTrack, frame+1, grpTwo) = tmpPeakView(n, peakView::pkTrack, frame, grpOne);
        lastMatched = candidate;
        matchedTracks++;
      }
    }
  } //end of loop on peaks of current frame

  // STEP 3
  // check for any unmatched peaks in the next frame and give BIRTH to new tracks!
  for(mrs_natural m = 0; m < tmpPeakView.getFrameNumPeaks(frame+1, grpTwo); ++m)
  {
    if(tmpPeakView(m, peakView::pkTrack, frame+1, grpTwo) == -1.0)
      tmpPeakView(m, peakView::pkTrack, frame+1, grpTwo) = nextTrack++; //BIRTH of new track
  }

  return matchedTracks;
}

void
McAulayQuatieri::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o,c;
  t=0;
  o=0;
  c=0;

  realvec* outPtr;

  out(o,t) = in(o,t);          //    ??????

  //if we want to use memory and we already have data from
  //past inputs (i.e. memory is not empty)...
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

    //attempt matching of groups between the frame in memory
    //and the first frame from current input
    if(ctrl_useGroups_->to<mrs_bool>())
    {
      peakView inPV(in);
      mrs_realvec inFirstFrame;
      in.getCol(0, inFirstFrame);
      peakView inFirstFramePV(inFirstFrame);
      peakView memPV(memory_);
      peakView tmpPV(tmp_);

      //mrs_natural numInGroups = inPV.getNumGroups();
      mrs_natural numInFirstFrameGroups = inFirstFramePV.getNumGroups();
      mrs_natural numMemGroups = memPV.getNumGroups();

      //we must update the group numbers of the groups
      //in tmp realvec (i.e. [mem|current input])
      //so they do not clash with the previous ones
      if(nextGroup_ > 0)
        for(mrs_natural f=1; f < tmpPV.getNumFrames(); ++f)
          for(mrs_natural p = 0; p < tmpPV.getFrameNumPeaks(f); ++p)
            tmpPV(p, peakView::pkGroup, f) = tmpPV(p, peakView::pkGroup, f) + nextGroup_;

      // Try matching previous groups (in memory from last input)
      // with groups in current input

      // create a tmp copy of the frame in memory and the first frame
      // of current input, so we can do the group matching without
      // destroying the input values
      realvec frames2Match(inObservations_, 2);

      // calculate the matching score for all pairs of groups under matching
      realvec matchScores(numInFirstFrameGroups, numMemGroups);
      for(mrs_natural mg=0; mg < numMemGroups; ++mg)
      {
        for(mrs_natural ig = nextGroup_; ig < nextGroup_ + numInFirstFrameGroups; ++ig)
        {
          //since peakTrack(...) is destructible, we must reset frames2Match everytime... [!]
          for(o=0; o<inObservations_; ++o)
            for(c=0; c < 2; ++c)
              frames2Match(o, c) = tmp_(o, c);

          //use McAulay-Quatieri num of successful peak continuations as a score
          //for the group matching (may be replaced by some other metric in future)
          matchScores(ig-nextGroup_, mg) = peakTrack(frames2Match, 0, ig, mg);
        }
      }

      //Given the matchScores, try to find the optimal assignment
      //of the groups coming from previous input (stored in memory)
      //and the new groups in the current input
      //(using, for e.g. the hungarian method)
      realvec assignedGrp(numInFirstFrameGroups);

      //convert matchScores to costs
      mrs_real maxScore = matchScores.maxval();
      for(o=0; o < matchScores.getRows(); ++o)
        for(c=0; c < matchScores.getCols(); ++ c)
          matchScores(o,c) = maxScore - matchScores(o,c);

      NumericLib::hungarianAssignment(matchScores, assignedGrp); //!!!!!!!!!!!!!!! [TODO][!]

      // given the assignments, try to propagate the group IDs
      // to the groups in the current input
      mrs_natural ig;
      for(mrs_natural f=1; f < tmpPV.getNumFrames(); ++f)
      {
        for(mrs_natural p = 0; p < tmpPV.getFrameNumPeaks(f); ++p)
        {
          //get input group ID (converted to the range [0:...])
          ig = (mrs_natural)(tmpPV(p, peakView::pkGroup, f)) - nextGroup_;

          if(assignedGrp(ig) > -1) //a match was found for this group (ig)
          {
            //check if match is higher than the specified threshold
            if((maxScore - matchScores(ig, (mrs_natural)assignedGrp(ig))) / memPV.getFrameNumPeaks(0,(mrs_natural)assignedGrp(ig)) > ctrl_matchThres_->to<mrs_real>())
            {
              //match confirmed --> propagate group ID
              tmpPV(p, peakView::pkGroup, f) = assignedGrp(ig);
            }
            else //match below threshold --> set as new group
            {
              tmpPV(p, peakView::pkGroup, f) = nextGroup_;
              assignedGrp(ig) = nextGroup_;
              nextGroup_++;
            }
          }
          else //no match found for this group! --> set as new group
          {
            tmpPV(p, peakView::pkGroup, f) = nextGroup_;
            assignedGrp(ig) = nextGroup_;
            nextGroup_++;
          }
        }
      }
    }
  }
  else
  {
    //no need to concatenate memory information with
    //current input. Just do it inplace in the output realvec (avoid extra copy)!
    outPtr = &out;
  }

  peakView tmpPeakView(*outPtr);

  /////////////////////////////////////////////////////////////////////////////
  mrs_natural numGroups;
  mrs_natural g;
  if(ctrl_useGroups_->to<mrs_bool>())
  {
    numGroups = tmpPeakView.getNumGroups();
    g = 0;
  }
  else
  {
    numGroups = 0;
    g = -1;
  }
  //iterate over groups (if any or enabled)
  for(; g < numGroups; ++g)
  {
    //if no memory being used (or no memory stored yet), we must use peaks
    //in first frame to give birth to new tracks
    if(!ctrl_useMemory_->to<mrs_bool>() || memory_.getSize() == 0)
    {
      for(mrs_natural n = 0; n < tmpPeakView.getFrameNumPeaks(0, g); ++n)
        tmpPeakView(n, peakView::pkTrack, 0) = (mrs_real) n;
    }

    //iterate over input frames
    for(mrs_natural f=0; f < tmpPeakView.getNumFrames()-1; ++f)
      peakTrack(*outPtr, f, g, g);
  }

  //if using memory...
  if(ctrl_useMemory_->to<mrs_bool>())
  {
    if(memory_.getSize() != 0)
    {
      //if using a non-empty memory, we should now fill the trackID and GroupID parameters
      //computed above (and stored in the tmp realvec) into the actual output
      peakView outPeakView(out);
      for(mrs_natural f=0; f < outPeakView.getNumFrames(); ++f)
        for(mrs_natural p = 0; p < outPeakView.getFrameNumPeaks(f); ++p)
        {
          outPeakView(p, peakView::pkTrack, f) = tmpPeakView(p, peakView::pkTrack, f+1);
          outPeakView(p, peakView::pkGroup, f) = tmpPeakView(p, peakView::pkGroup, f+1);
        }
    }

    //store the last frame of current output for next time
    memory_.stretch(onObservations_, 1);
    for(o = 0; o < onObservations_; ++o)
      memory_(o, 0) = out(o, onSamples_-1);
  }
}
