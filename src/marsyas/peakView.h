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

#ifndef MARSYAS_PEAKVIEW_H
#define MARSYAS_PEAKVIEW_H

#include <marsyas/realvec.h>
#include <vector>
#include <marsyas/export.h>

namespace Marsyas
{
/**
\class peakView
\ingroup Notmar
\brief Helper class for accessing peaks in a realvec.
*/

class marsyas_EXPORT peakView
{
public:
  enum pkParameter {
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
    pkTrack,
    nbPkParameters
  };

private:
  realvec& vec_;

  mrs_real fs_;
  mrs_natural frameSize_;

  mrs_natural frameMaxNumPeaks_;
  mrs_natural numFrames_;

public:

  peakView(realvec& vec);
  ~peakView();

  void fromTable(const realvec& vecTable);
  void toTable(realvec& vecTable);

  mrs_real getFs() const {return fs_;};
  mrs_natural getFrameSize() const {return frameSize_;};

  mrs_natural getNumFrames() const {return numFrames_;};
  mrs_natural getNumGroups();

  mrs_natural getFrameMaxNumPeaks() const {return frameMaxNumPeaks_;};
  mrs_natural getFrameNumPeaks(const mrs_natural frame=0, const mrs_natural group=-1) const;
  mrs_natural getTotalNumPeaks(const mrs_natural group=-1) const;

  void getPeaksParam(std::vector<realvec>& result, const pkParameter param, mrs_natural startFrame = 0, mrs_natural endFrame = 0) const;

  mrs_real& operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame=0, const mrs_natural group=-1);
  mrs_real operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame=0, const mrs_natural group=-1) const;

  static std::string getParamName(mrs_natural paramIdx);

  bool peakWrite(std::string filename, mrs_real fs=0, mrs_natural frameSize=0);
  bool peakRead(std::string filename);

  void removePeak (const mrs_natural peakIndex, const mrs_natural frame);
};

inline
mrs_real& peakView::operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame, const mrs_natural group)
{
  //if peakIndex is a global index (i.e. independent of the group)
  //just return the corresponding parameter value
  if(group == -1)
    return vec_(peakIndex + param * frameMaxNumPeaks_, frame);
  else
  {
    //if peakIndex is an index of a peak belonging to the passed group
    //we have to convert it to the absolute (i.e. unrelated to any group) index first
    mrs_natural gp = 0;
    for(mrs_natural p = 0; p < this->getFrameNumPeaks(frame); ++p)
    {
      if(vec_(p + pkGroup * frameMaxNumPeaks_, frame) == group)
      {
        if(peakIndex == gp)
          return vec_(p + param * frameMaxNumPeaks_, frame);
        gp++;
      }
    }
    //if the passed peakIndex does not exist in the passed group
    //return an "invalid" value and issue an error...
    MRSERR("peakView::operator() - peakIndex " << peakIndex << " not found in passed group " << group);
    return vec_(peakIndex + param * frameMaxNumPeaks_, frame);
  }
}

inline
mrs_real peakView::operator()(const mrs_natural peakIndex, const pkParameter param, const mrs_natural frame, const mrs_natural group) const
{
  //if peakIndex is a global index (i.e. independent of the group)
  //just return the corresponding parameter value
  if(group == -1)
    return vec_(peakIndex + param * frameMaxNumPeaks_, frame);
  else
  {
    //if peakIndex is an index of a peak belonging to the passed group
    //we have to convert it to the absolute (i.e. unrelated to any group) index first
    mrs_natural gp = 0;
    for(mrs_natural p = 0; p < this->getFrameNumPeaks(frame); ++p)
    {
      if(vec_(p + pkGroup * frameMaxNumPeaks_, frame) == group)
      {
        if(peakIndex == gp)
          return vec_(p + param * frameMaxNumPeaks_, frame);
        gp++;
      }
    }
    //if the passed peakIndex does not exist in the passed group
    //return a "dummy" value and issue an error...
    MRSERR("peakView::operator() - peakIndex " << peakIndex << " not found in passed group " << group);
    return -1.0;
  }
}

}//namespace Marsyas

#endif /* !MARSYAS_PEAKVIEW_H */

