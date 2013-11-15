
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

#include <marsyas/peakView.h>
#include <algorithm>

using namespace std;
using namespace Marsyas;

peakView::peakView(realvec& vec): vec_(vec), fs_(0), frameSize_(0)
{
  //max number of peaks in each frame
  frameMaxNumPeaks_ = vec_.getRows() / nbPkParameters;
  numFrames_ = vec_.getCols();
}

peakView::~peakView()
{

}

mrs_natural
peakView::getNumGroups()
{
  mrs_natural numGroups = 0;

  for(mrs_natural f=0; f < numFrames_; ++f)
    for(mrs_natural p = 0; p < this->getFrameNumPeaks(f); ++p)
    {
      if((*this)(p, pkGroup, f) > numGroups)
        numGroups = (mrs_natural)(*this)(p, pkGroup, f);
    }
  numGroups++;
  return numGroups;
}

mrs_natural
peakView::getFrameNumPeaks(const mrs_natural frame, const mrs_natural group) const
{
  if(group==-1)//ignore group information
  {
    //count the number of detected peaks in the frame
    //
    //"peaks" with freq == 0 are considered non-existent
    //so just count peaks until we get the first "peak" with freq!=0
    mrs_natural p;
    for(p=0; p < frameMaxNumPeaks_; ++p)
    {
      if((*this)(p, pkFrequency, frame) == 0)
        return p;
    }
    return frameMaxNumPeaks_;
  }
  else //count peaks from "group" that exist in "frame"
  {
    mrs_natural numPeaks = 0;
    for(mrs_natural p = 0; p < frameMaxNumPeaks_; ++p)
    {
      if((*this)(p, pkFrequency, frame) == 0)
        return numPeaks;

      if((*this)(p, pkGroup, frame) == group)
        numPeaks++;
    }
    return numPeaks;
  }
}

mrs_natural
peakView::getTotalNumPeaks(const mrs_natural group) const
{
  mrs_natural numPeaks = 0;
  for(mrs_natural f=0; f < numFrames_; ++f)
  {
    numPeaks += this->getFrameNumPeaks(f, group);
  }
  return numPeaks;
}

void
peakView::removePeak (const mrs_natural peakIndex, const mrs_natural frame)
{
  mrs_natural p,
              totalNumPeaks = getTotalNumPeaks ();
  for (mrs_natural i = peakIndex; i < totalNumPeaks -1; i++)
  {
    for (p = 0; p < nbPkParameters; p++)
      vec_(peakIndex + p * frameMaxNumPeaks_, frame) = vec_(peakIndex + 1 + p * frameMaxNumPeaks_, frame);
  }
  for (p = 0; p < nbPkParameters; p++)
    vec_(totalNumPeaks -1 + p * frameMaxNumPeaks_, frame) = 0;
}

void
peakView::getPeaksParam(vector<realvec>& result, const pkParameter param, mrs_natural startFrame, mrs_natural endFrame) const
{
  if(startFrame < 0 || endFrame < 0)
  {
    MRSWARN("peakView::getPeaksParam: negative start/stop frame! Returning empty vector.");
    return;
  }
  if(startFrame >= vec_.getCols() || endFrame >= vec_.getCols())
  {
    MRSWARN("peakView::getPeaksParam: start/end frame bigger than vector column size! Returning empty vector.");
    return;
  }

  mrs_natural numPeaks;

  for(mrs_natural f = startFrame; f <= endFrame; ++f)
  {
    //get a vector with the parameter values for all the peaks
    //detected in the frame (i.e. whose freq != 0)
    numPeaks = getFrameNumPeaks(f);
    realvec valVec(numPeaks);

    for(mrs_natural p=0; p<numPeaks; ++p)
      valVec(p) = (*this)(p, param, f);

    result.push_back(valVec);
  }
}

mrs_string
peakView::getParamName(mrs_natural paramIdx)
{
  switch(paramIdx)
  {
  case 0:
    return "pkFrequency";
    break;
  case 1:
    return "pkAmplitude";
    break;
  case 2:
    return "pkPhase";
    break;
  case 3:
    return "pkDeltaFrequency";
    break;
  case 4:
    return "pkDeltaAmplitude";
    break;
  case 5:
    return "pkFrame";
    break;
  case 6:
    return "pkGroup";
    break;
  case 7:
    return "pkVolume";
    break;
  case 8:
    return "pkPan";
    break;
  case 9:
    return "pkBinLow";
    break;
  case 10:
    return "pkBin";
    break;
  case 11:
    return "pkBinHigh";
    break;
  case 12:
    return "nbPkParameters";
    break;
  default:
    return "MARSYAS_EMPTY";
    break;
  }
}

void
peakView::toTable(realvec& vecTable)
{
  //resize and initialize the table (assuming the largest possible number of peaks + the header for now...)
  vecTable.create(frameMaxNumPeaks_*numFrames_+1, nbPkParameters);


  //In Table format, the 1st row is a "header row"
  vecTable(0, 0) = -1;
  vecTable(0, 1) =  fs_;
  vecTable(0, 2) =  frameSize_;
  vecTable(0, 3) =  frameMaxNumPeaks_;
  vecTable(0, 4) =  numFrames_;
  vecTable(0, 5) = -1;
  vecTable(0, pkGroup) = -2;
  for (mrs_natural i = pkGroup+1 ; i < nbPkParameters ; ++i) //i = pkGroup or i = pkGroup+1 [?]
    vecTable(0, i)=0;


  //fill the table with peak data
  mrs_natural l = 1; //l = peak index for table format (i.e. row index)
  for (mrs_natural j=0 ; j < vec_.getCols(); ++j) //j = frame index
    for (mrs_natural i=0 ; i < frameMaxNumPeaks_; ++i) //i = peak index
    {
      //just output existing peaks at each frame (i.e. freq != 0)
      if(vec_(i, j) != 0.0)
      {
        for(mrs_natural k = 0; k < nbPkParameters; k++)// k = parameter index
          vecTable(l, k) = (*this)(i, pkParameter(k), j);
        l++;
      }
    }

  //resize the table to the correct (i.e. possibly smaller)
  //number of peaks (i.e. nr of rows)

  vecTable.stretch(l, nbPkParameters);
}

void
peakView::fromTable(const realvec& vecTable)
{
  //get data from header
  fs_  = vecTable(0, 1);
  frameSize_ = (mrs_natural)vecTable(0, 2);
  frameMaxNumPeaks_ = (mrs_natural)vecTable(0, 3);
  numFrames_ = (mrs_natural)vecTable(0, 4);

  //get the first frame in table (it may not be 0!!!)
  mrs_natural frame = (mrs_natural)vecTable(1, pkFrame); // start frame [!]

  //resize (and set to zero) vec_ for new peak data
  //(should accommodate empty frames before the first frame in table!)
  vec_.create(frameMaxNumPeaks_*nbPkParameters, numFrames_+frame); //[!]

  mrs_natural p = 0; // peak index inside each frame
  mrs_natural r = 1;//peak index in table (i.e. row index) - ignore header row

  //check in case realvec has less parameters than nbPkParameters
  mrs_natural actualNbPkParams = (mrs_natural)min((mrs_real)nbPkParameters, (mrs_real)vecTable.getCols());

  //iterate over table rows (i.e. peaks) - excluding header row
  while(r < vecTable.getRows()-1)
  {
    //get parameters for this peak
    for(mrs_natural prm = 0; prm < actualNbPkParams; ++prm)
    {
      (*this)(p, pkParameter(prm), frame) = vecTable(r, prm);
    }
    ++r; //move on to next table row (i.e. peak)
    p++;
    //if the next row in table is form a different frame,
    //reset peak index and get new frame index
    if(vecTable(r, pkFrame) != frame)
    {
      frame = (mrs_natural)vecTable(r, pkFrame);
      p = 0;
    }
  }
}

bool
peakView::peakWrite(mrs_string filename, mrs_real fs, mrs_natural frameSize)
{
  //we may want to write this peakVector with its internal parameters
  if(fs != 0)
    fs_ = fs;
  if(frameSize != 0)
    frameSize_ = frameSize;


  //convert vec_ to table format and save to file
  realvec resVec;
  this->toTable(resVec);
  return resVec.write(filename);
}

bool
peakView::peakRead(mrs_string filename)
{
  //read .peak into a realvec
  realvec vec_Table;
  if(vec_Table.read(filename))
  {
    //read data from the table
    this->fromTable(vec_Table);
    return true;
  }
  else
    return false; //problem reading .peak file
}
