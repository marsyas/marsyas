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

#ifndef MARSYAS_TimeFreqPeakConnectivity_H
#define MARSYAS_TimeFreqPeakConnectivity_H

#include <marsyas/system/MarSystem.h>

class DoubleListEntries;

namespace Marsyas
{
/**
\ingroup Processing
\brief Uses dynamic programming to create a (symmetric) distance
matrix between the series of peaks at the inputs

The input is a matrix with the dimension 2x numpeaks,
row 1 is the frequency of the peak in bark, row 2 is
the block index of the peak.

Controls:
- \b mrs_string/frequencyIntervalInHz [w] : tells the system which frequency range to expect, e.g. 250_2500
- \b mrs_natural/textureWindowSize [rw] : tells the system how many block indices to expect per call
- \b mrs_bool/inBark [rw] : controls whether the computation is done in the frequency or in the bark domain
- \b mrs_bool/freqResolution [rw] : sets the frequency resolution of the system (in Hz or in Bark, depending on inBark
*/

class marsyas_EXPORT TimeFreqPeakConnectivity: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  // don't change the order!
  enum Direction_t
  {
    kFromDown,
    kFromLeft,
    kFromUp,

    kNumDirections
  };

  static inline mrs_real dtwFindMin (mrs_real *prevCost, unsigned char &tbIdx)
  {
    mrs_real	min = 1e30;
    tbIdx	= kFromLeft;

    for (unsigned char i = 0; i < kNumDirections; i++)
    {
      if (prevCost[i] <= min)
      {
        min		= prevCost[i];
        tbIdx	= i;
      }
    }
    return min;
  };

  void AllocMemory (mrs_natural numSamples);
  void FreeMemory ();

  void SetOutput(mrs_realvec &out, const mrs_real cost, mrs_natural idxRowA, mrs_natural idxColA, mrs_natural idxRowB, mrs_natural idxColB);
  mrs_natural Freq2RowIdx (mrs_real barkFreq, mrs_real bres);
  void CalcDp (mrs_realvec &Matrix, mrs_natural startr, mrs_natural startc, mrs_natural stopr, mrs_natural stopc);
  void InitMatrix (mrs_realvec &Matrix, unsigned char **traceback, mrs_natural rowIdx, mrs_natural colIdx);
  MarControlPtr	ctrl_reso_;
  mrs_realvec		peakMatrix_,
                costMatrix_;
  mrs_real		downFreq_,
              upFreq_;
  mrs_natural		numBands_,
                textWinSize_;
  mrs_natural		*path_,
                **peakIndices_;
  unsigned char** tracebackIdx_;
  DoubleListEntries *multipleIndices;

public:
  /// TimeFreqPeakConnectivity constructor.
  TimeFreqPeakConnectivity(std::string name);

  /// TimeFreqPeakConnectivity copy constructor.
  TimeFreqPeakConnectivity(const TimeFreqPeakConnectivity& a);

  /// TimeFreqPeakConnectivity destructor.
  ~TimeFreqPeakConnectivity();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_TimeFreqPeakConnectivity_H

