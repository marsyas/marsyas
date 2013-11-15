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

#include "../common_source.h"
#include "TimeFreqPeakConnectivity.h"
#include <algorithm>

using std::max;
using namespace Marsyas;

//#define MATLAB_DBG_OUT
//#define SANITY_CHECK

static const mrs_real		costInit	= 1e30;
static const mrs_natural	listLenInit = 16;
class DoubleListEntries
{
public:
  DoubleListEntries () : maxListLength(0),currListLength(0)
  {
    list = 0;
    AllocMem (listLenInit);
  };
  ~DoubleListEntries ()
  {
    FreeMem ();
  };
  mrs_natural GetNumIndices (mrs_natural row, mrs_natural col)
  {
    mrs_natural count = 0;
    for (mrs_natural i = 0; i < currListLength; i++)
    {
      if (row == list[i][kRow])
        if (col == list[i][kCol])
          count++;
    }
    return count;
  };
  mrs_natural GetIndex (mrs_natural row, mrs_natural col, mrs_natural index)
  {
    for (mrs_natural i = 0; i < currListLength; i++)
    {
      if (row == list[i][kRow]) {
        if (col == list[i][kCol]) {
          if (index == 0) {
            return list[i][kIdx];
          } else {
            index--;
          }
        }
      }
    }
    MRSASSERT(true);
    return -1;
  };
  mrs_bool IsInList  (mrs_natural row, mrs_natural col, mrs_natural index)
  {
    for (mrs_natural i = 0; i < currListLength; i++)
    {
      if (row == list[i][kRow])
        if (col == list[i][kCol])
          if (index == list[i][kIdx])
            return true;
    }
    return false;
  };
  void AddIndex  (mrs_natural row, mrs_natural col, mrs_natural index)
  {
    if (IsInList (row,col,index))
      return;
    if (currListLength == maxListLength -1)
      AllocMem (maxListLength<<1);
    list[currListLength][kRow]	= row;
    list[currListLength][kCol]	= col;
    list[currListLength][kIdx]	= index;
    currListLength++;
  };
  void Reset ()
  {
    currListLength	= 0;
  };
private:
  void AllocMem(mrs_natural numNewListItems)
  {
    mrs_natural i;
    MRSASSERT(numNewListItems >= maxListLength);
    mrs_natural **tmpList = new mrs_natural* [numNewListItems];
    for (i = 0; i < maxListLength; i++)
      tmpList[i]	= list[i];
    for (i = maxListLength; i < numNewListItems; i++)
      tmpList[i]	= new mrs_natural [kNumListEntries];

    delete [] list;
    list			= tmpList;
    maxListLength	= numNewListItems;
    return;
  };
  void FreeMem ()
  {
    for (mrs_natural i = 0; i < maxListLength; i++)
      delete [] list[i];
    delete [] list;
  };

  enum ListEntries_t
  {
    kRow,
    kCol,
    kIdx,

    kNumListEntries
  };
  mrs_natural **list;
  mrs_natural maxListLength,
              currListLength;
};


static inline mrs_natural MyMax (mrs_natural a , mrs_natural b)
{
  return (a >= b) ? a : b;
}
static inline mrs_natural MyMin (mrs_natural a , mrs_natural b)
{
  return (a <= b) ? a : b;

}

TimeFreqPeakConnectivity::TimeFreqPeakConnectivity(mrs_string name) : MarSystem("TimeFreqPeakConnectivity", name)
{
  addControls();

  tracebackIdx_	= 0;
  peakIndices_	= 0;
  path_			= 0;
  numBands_		= 0;
  multipleIndices = 0;
}

TimeFreqPeakConnectivity::TimeFreqPeakConnectivity(const TimeFreqPeakConnectivity& a) : MarSystem(a)
{
  // IMPORTANT!
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
  ctrl_reso_	= getctrl("mrs_real/freqResolution");

  tracebackIdx_	= 0;
  peakIndices_	= 0;
  path_			= 0;
  numBands_		= 0;
  multipleIndices = 0;
}


TimeFreqPeakConnectivity::~TimeFreqPeakConnectivity()
{
  FreeMemory ();
  if (multipleIndices)
    delete multipleIndices;
}

MarSystem*
TimeFreqPeakConnectivity::clone() const
{
  // Every MarSystem should do this.
  return new TimeFreqPeakConnectivity(*this);
}

void
TimeFreqPeakConnectivity::addControls()
{
  addctrl("mrs_string/frequencyIntervalInHz", "MARSYAS_EMPTY");
  setctrlState("mrs_string/frequencyIntervalInHz", true);

  addctrl("mrs_bool/inBark", false);
  addctrl("mrs_real/freqResolution", 25., ctrl_reso_);
  //addctrl("mrs_bool/inBark", true);
  //addctrl("mrs_real/freqResolution", .15, ctrl_reso_);

  addctrl("mrs_natural/textureWindowSize", 0);
}

void
TimeFreqPeakConnectivity::AllocMemory (mrs_natural numSamples)
{
  tracebackIdx_	= new unsigned char* [numBands_];
  peakIndices_	= new mrs_natural* [numBands_];
  for (mrs_natural i = 0; i < numBands_; i++)
  {
    tracebackIdx_[i]	= new unsigned char [numSamples];
    peakIndices_[i]		= new mrs_natural[numSamples];
  }
  path_	= new mrs_natural [numSamples];

  if (multipleIndices == 0)
    multipleIndices = new DoubleListEntries ();
  else
    multipleIndices->Reset ();
}

void
TimeFreqPeakConnectivity::FreeMemory ()
{
  if (tracebackIdx_)
  {
    for (mrs_natural i = 0; i < numBands_; i++)
      delete [] tracebackIdx_[i];
    delete [] tracebackIdx_;
  }
  if (peakIndices_)
  {
    for (mrs_natural i = 0; i < numBands_; i++)
      delete [] peakIndices_[i];
    delete [] peakIndices_;
  }

  delete [] path_;
}

void
TimeFreqPeakConnectivity::myUpdate(MarControlPtr sender)
{
  MRSDIAG("TimeFreqPeakConnectivity.cpp - TimeFreqPeakConnectivity:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  const mrs_bool	isInBark	= getctrl("mrs_bool/inBark")->to<mrs_bool>();

  FreeMemory ();

  // compute matrix dimensions
  if(getctrl("mrs_string/frequencyIntervalInHz")->to<mrs_string>() != "MARSYAS_EMPTY")
  {
    realvec conv(2);
    string2parameters(getctrl("mrs_string/frequencyIntervalInHz")->to<mrs_string>(), conv, '_'); //[!]
    downFreq_	= (isInBark)? hertz2bark (conv(0)) : conv(0);
    upFreq_		= (isInBark)? hertz2bark (conv(1)) : conv(1);
    numBands_	= (mrs_natural)((upFreq_-downFreq_)/ctrl_reso_->to<mrs_real>() + .5);
  }
  else
  {
    numBands_	= 0;
    downFreq_	= 0;
    upFreq_		= 0;
  }
  textWinSize_ = getControl ("mrs_natural/textureWindowSize")->to<mrs_natural>();

  // create peak matrix
  peakMatrix_.create (numBands_, textWinSize_);
  costMatrix_.create (numBands_, textWinSize_);

  updControl ("mrs_natural/onObservations", inSamples_);
  updControl ("mrs_natural/onSamples", inSamples_);

  AllocMemory (textWinSize_);
}

void
TimeFreqPeakConnectivity::myProcess(realvec& in, realvec& out)
{
  // get pitch resolution
  const mrs_real	reso		= ctrl_reso_->to<mrs_real>();
  const mrs_bool	isInBark	= getctrl("mrs_bool/inBark")->to<mrs_bool>();

  // a matrix indicating where peaks are in the time frequency plane
  MRSASSERT(textWinSize_ >= in(1,inSamples_-1)-in(1,0));
  peakMatrix_.stretch(numBands_, textWinSize_);

  // init
  peakMatrix_.setval (1);
  for (mrs_natural t = 0; t < textWinSize_; t++)
    for (mrs_natural o=0; o < numBands_; o++)
      peakIndices_[o][t]	= -1;

  // initialized pseudo spectrogram representation
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    mrs_natural	row = (isInBark)? Freq2RowIdx (in(0,t),reso) : Freq2RowIdx (bark2hertz(in(0,t)),reso),	// input is in bark frequency, so we might have to convert it back
                col = (mrs_natural)(in(1,t)-in(1,0)+.1);
    MRSASSERT(col >= 0 && col < textWinSize_);
    MRSASSERT(row >= 0 && row < numBands_);
    // check whether more than one peak are at that matrix pos, i.e. we already have set the entry
    if (peakIndices_[row][col] != -1)
    {
      multipleIndices->AddIndex (row,col,peakIndices_[row][col]);
      multipleIndices->AddIndex (row,col,t);
      peakIndices_[row][col]	= -2;
    }
    else
      peakIndices_[row][col]	= t; // a matrix indicating which matrix bin corresponds to which input index
    peakMatrix_(row, col)	= 0;
  }

  // initialize output
  out.setval (costInit);

#ifdef MATLAB_DBG_OUT
#ifdef MARSYAS_MATLAB
  MATLAB_PUT(peakMatrix_, "peakMatrix");
  MATLAB_EVAL ("figure(1),imagesc(peakMatrix),colorbar");
#endif
#endif

  // iteration over all pairs
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    for (mrs_natural o=inSamples_-1; o >= t; o--)
    {
      // don't compute distance if we already have it
      if (out(t,o) != costInit)
        continue;

      // get peak matrix indices
      mrs_natural rowt = (isInBark)? Freq2RowIdx (in(0,t),reso) : Freq2RowIdx (bark2hertz(in(0,t)),reso),	// input is in bark frequency, so we might have to convert it back
                  rowo = (isInBark)? Freq2RowIdx (in(0,o),reso) : Freq2RowIdx (bark2hertz(in(0,o)),reso),	// input is in bark frequency, so we might have to convert it back
                  colt = (mrs_natural)(in(1,t)-in(1,0)+.1),
                  colo = (mrs_natural)(in(1,o)-in(1,0)+.1),
                  pathLength;

      MRSASSERT(colt >= 0 && colt < textWinSize_);
      MRSASSERT(colo >= 0 && colo < textWinSize_);
      MRSASSERT(rowt >= 0 && rowt < numBands_);
      MRSASSERT(rowo >= 0 && rowo < numBands_);


      // self similarity and similarity with overlapping peaks
      if ((t == o) || (rowt == rowo && colt == colo))
      {
        SetOutput(out, 0, rowt, colt, rowo, colo);
        continue;
      }

      // check if path calculation makes sense with the current dp step size
      if (abs(rowt - rowo) > abs(colt-colo))
      {
        SetOutput(out, 1, rowt, colt, rowo, colo);
        continue;
      }

      // let's calculate only from left to right
      if (colo < colt)
        continue;

      // dynamic programming
      CalcDp (peakMatrix_, rowt, colt, rowo, colo);
      pathLength	= colo-colt+1;

#ifdef MATLAB_DBG_OUT
#ifdef MARSYAS_MATLAB
      MATLAB_PUT(costMatrix_, "cost");
      MATLAB_EVAL ("figure(2),imagesc(cost,[0 10]),colorbar");
#endif
#endif

      // set cost for this path and all subpaths
      for (mrs_natural i = 0; i < pathLength; i++)
      {
        if (peakMatrix_(path_[i],colt + i) > 0)
        {
          MRSASSERT(i>0);
          continue;
        }
        for (mrs_natural j = i; j < pathLength; j++)
        {
          if (peakMatrix_(path_[j],colt + j) > 0)
            continue; // this path entry is not a peak

          mrs_real cost = (costMatrix_(path_[j],colt + j) - costMatrix_(path_[i],colt + i)) /  (j-i+1);
          MRSASSERT (cost >= 0 && cost <=1);

          // assign this (and related) output
          SetOutput(out, cost, path_[i], colt + i, path_[j], colt + j);
        }
      }
    }
  }
  multipleIndices->Reset ();

  // scale output because of RBF without std??
  //out	*= 10.;

  // convert distance to similarity
  //out	*= -1;
  //out += 1;
#ifdef SANITY_CHECK
  for (mrs_natural t=0; t < inSamples_; t++)
    for (mrs_natural o=0; o < inSamples_; o++)
      MRSASSERT (out(t,o) >= 0 && out(t,o) <=1);
#endif
#ifdef MATLAB_DBG_OUT
#ifdef MARSYAS_MATLAB
  MATLAB_PUT(out, "out");
  MATLAB_EVAL ("figure(3),imagesc(out),colorbar");
#endif
#endif

}


void TimeFreqPeakConnectivity::SetOutput(mrs_realvec &out, const mrs_real cost, mrs_natural idxRowA, mrs_natural idxColA, mrs_natural idxRowB, mrs_natural idxColB)
{
  mrs_natural ml = 0,
              nl = 0,
              rowIdx = peakIndices_[idxRowA][idxColA],
              colIdx = peakIndices_[idxRowB][idxColB];

  if (rowIdx == -2)
  {
    ml = multipleIndices->GetNumIndices (idxRowA, idxColA);
    MRSASSERT(ml>0);
    rowIdx	= multipleIndices->GetIndex (idxRowA, idxColA, 0);
  }
  if (colIdx == -2)
  {
    nl = multipleIndices->GetNumIndices (idxRowB, idxColB);
    MRSASSERT(nl>0);
    colIdx	= multipleIndices->GetIndex (idxRowB, idxColB, 0);
  }

  if (out(rowIdx, colIdx) != costInit)
  {
    MRSASSERT(out(rowIdx, colIdx) == cost);
    return; // cost has already been computed	- nothing to do
  }
  //if ((rowIdx==53 && colIdx==55) || (rowIdx==55 && colIdx==55) || (rowIdx==55 && colIdx==53))
  //	cout << out(t,o) << endl;

  out(rowIdx,colIdx)	= cost;
  out(colIdx,rowIdx)	= cost;

  // only for the case of overlapping peaks
  if (ml > 0 || nl > 0)
  {
    mrs_natural m,n;

    // both > 0
    if (ml * nl)
    {
      for (m = 0; m < ml; m++)
      {
        rowIdx	= multipleIndices->GetIndex (idxRowA, idxColA, m);
        for (n = 0; n < nl; n++)
        {
          colIdx	= multipleIndices->GetIndex (idxRowB, idxColB, n);
          //if ((rowIdx==53 && colIdx==55) || (rowIdx==55 && colIdx==55) || (rowIdx==55 && colIdx==53))
          //	cout << out(t,o) << endl;
          out(rowIdx,colIdx)	= cost;
          out(colIdx,rowIdx)	= cost;
        }
      }
    }
    else if (ml > 0)
    {
      for (m = 0; m < ml; m++)
      {
        rowIdx	= multipleIndices->GetIndex (idxRowA, idxColA, m);
        //if ((rowIdx==53 && colIdx==55) || (rowIdx==55 && colIdx==55) || (rowIdx==55 && colIdx==53))
        //	cout << out(t,o) << endl;
        out(rowIdx,colIdx)	= cost;
        out(colIdx,rowIdx)	= cost;
      }

    }
    else if (nl > 0)
    {
      for (n = 0; n < nl; n++)
      {
        colIdx	= multipleIndices->GetIndex (idxRowB, idxColB, n);
        //if ((rowIdx==53 && colIdx==55) || (rowIdx==55 && colIdx==55) || (rowIdx==55 && colIdx==53))
        //	cout << out(t,o) << endl;
        out(rowIdx,colIdx)	= cost;
        out(colIdx,rowIdx)	= cost;
      }
    }
  }
}

mrs_natural TimeFreqPeakConnectivity::Freq2RowIdx (mrs_real barkFreq, mrs_real bres)
{
  mrs_natural result = (mrs_natural)((barkFreq-downFreq_)/bres +.5);

  if (result < 0)
    result = 0;
  if (result >= numBands_)
    result =numBands_-1;

  return result;
}

void TimeFreqPeakConnectivity::InitMatrix (mrs_realvec &Matrix, unsigned char **traceback, mrs_natural rowIdx, mrs_natural colIdx)
{
  mrs_natural i,j,
              numRows = Matrix.getRows (),
              numCols = Matrix.getCols ();
  mrs_natural iCol;

  Matrix.setval(0);

  traceback[rowIdx][colIdx]	= kFromLeft;

  // left of point of interest
  for (i = 0; i < numRows; i++)
  {
    for (j = 0; j < colIdx; j++)
    {
      Matrix(i,j)		= costInit;
      traceback[i][j]	= kFromLeft;
    }
  }
  //cout << Matrix << endl;
  //upper left corner
  for (i = 0; i < rowIdx; i++)
  {
    iCol	= MyMin (rowIdx - i + colIdx, numCols);
    for (j = colIdx; j < iCol; j++)
    {
      Matrix(i,j)		= costInit;
      traceback[i][j]	= kFromLeft;
    }
  }
  //cout << Matrix << endl;
  // lower left corner
  for (i = rowIdx + 1; i < numRows; i++)
  {
    iCol	= MyMin (i - rowIdx + colIdx, numCols);
    for (j = colIdx; j < iCol; j++)
    {
      Matrix(i,j)		= costInit;
      traceback[i][j]	= kFromLeft;
    }
  }
  //cout << Matrix << endl;
}

void TimeFreqPeakConnectivity::CalcDp (mrs_realvec &Matrix, mrs_natural startr, mrs_natural startc, mrs_natural stopr, mrs_natural stopc)
{
  mrs_natural i,j,
              numRows = Matrix.getRows (),
              numCols = Matrix.getCols ();
  mrs_real prevCost[kNumDirections]	= {0,0,0};

  costMatrix_.stretch ( numRows, numCols);

  // initialize cost and traceback matrix
  // upper and lower left corner
  InitMatrix (costMatrix_, tracebackIdx_, startr, startc);
  costMatrix_(startr, startc)	= Matrix(startr, startc);

  // compute cost matrix
  for (j = startc+1; j <= stopc; j++)
  {
    mrs_natural rowLoopStart	= MyMax (0, startr - (j - startc)),
                 rowLoopStop		= MyMin (numRows, startr + (j-startc) + 1);
    for (i = rowLoopStart; i < rowLoopStop; i++)
    {
      prevCost[kFromLeft]	= costMatrix_(i,j-1);
      prevCost[kFromUp]	= (i >= numRows-1) ? costInit : costMatrix_(i+1, j-1);			// the if isn't very nice here....
      prevCost[kFromDown]	= (i <= 0) ? costInit : costMatrix_(i-1, j-1);
      // assign cost
      costMatrix_(i,j)	= Matrix(i,j) + dtwFindMin (prevCost, tracebackIdx_[i][j]);
    }
  }

  // compute path
  i = stopr;
  for (j = stopc; j >= startc; j--)
  {
    path_[j-startc]	= i;
    i				-= (kFromLeft - tracebackIdx_[i][j]); // note: does work only for kFromUp, kFromLeft, kFromDown
  }
}
