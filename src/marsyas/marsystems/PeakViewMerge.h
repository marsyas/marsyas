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

#ifndef MARSYAS_PeakViewMerge_H
#define MARSYAS_PeakViewMerge_H

#include <marsyas/system/MarSystem.h>

#include <fstream>
#include <iostream>

namespace Marsyas
{
/**
\ingroup IO
\brief This MarSystem receives two stacked realvecs peak information (using peakView)
and merges them.

Controls:
- \b mrs_string/mode [w] : AND (output only peaks in both), OR (output all peaks), XOR
(output only the peaks that are not in both inputs).
- \b mrs_natural/totalNumPeaks [r] : number of peaks at the output
- \b mrs_natural/frameMaxNumPeaks1 [w] : maximum number of peaks per frame in input 1
- \b mrs_natural/frameMaxNumPeaks2 [w] : maximum number of peaks per frame in input 2
- \b mrs_natural/discardNegativeGroups [w] : discard all peaks with negative group indices when true
*/


class PeakViewMerge: public MarSystem
{
private:

  void addControls();
  void myUpdate(MarControlPtr sender);

  enum InputViews
  {
    kMat1,
    kMat2,

    kNumMatrices
  };

  MarControlPtr	ctrl_mode_,
                ctrl_totalNumPeaks_,
                ctrl_frameMaxNumPeaks1_,
                ctrl_frameMaxNumPeaks2_,
                ctrl_noNegativeGroups_;
  mrs_realvec peakViewIn_[kNumMatrices];

public:
  PeakViewMerge(std::string name);
  PeakViewMerge(const PeakViewMerge& a);
  ~PeakViewMerge();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

} //namespace Marsyas

#endif // MARSYAS_PeakViewMerge_H
