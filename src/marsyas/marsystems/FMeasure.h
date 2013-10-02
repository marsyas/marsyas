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

#ifndef MARSYAS_FMEASURE_H
#define MARSYAS_FMEASURE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class FMeasure
\brief Compute the FMeasure

compute the F-Measure from the detection counts given as controls;
output dimensions are 3x1 (F-Measure, Precision, Recall), the input
is not used

Controls:
- \b mrs_natural/numObservationsInReference [w] : number of relevant data points
- \b mrs_natural/numObservationsInTest [w] : number of detected data points
- \b mrs_natural/numTruePositives [w] : number of correctly detected data points
- \b mrs_bool/reset [w] : set to true to reset the internal variables
*/


class FMeasure: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr	ctrl_numObsInRef_,
                ctrl_numObsInTest_,
                ctrl_numTruePos_,
                ctrl_reset_;
  mrs_natural		numObsInRef_,
                numObsInTest_,
                numTruePos_;
  void myUpdate(MarControlPtr sender);

  enum OutputIdx_t
  {
    kFMeasure,
    kPrecision,
    kRecall,

    kNumOutputs
  };

public:
  FMeasure(std::string name);
  FMeasure(const FMeasure& a);
  ~FMeasure();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
