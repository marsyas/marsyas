/** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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


#ifndef MARSYAS_PeakEnhancer_H
#define MARSYAS_PeakEnhancer_H

#define DEFAULT_ITNUM 1

#include <marsyas/system/MarSystem.h>
#include "HalfWaveRectifier.h"
#include "Filter.h"

namespace Marsyas
{
/**
\ingroup Processing Analysis
\brief Enhance peaks that are harmonic

Enhance harmonic peaks resulting from autocorrelation periodicity
detection by applying half wave rectification and then subtracting the
upsampled fvec from the original fvec.
*/
class PeakEnhancer: public MarSystem
{

private:
  //Add specific controls needed by this MarSystem.
  void addControls();

  MarControlPtr ctrl_itnum_;

  MarSystem* rect_;
  MarSystem* lowpass_;

  //mrs_string resaModeOld_;
  void myUpdate(MarControlPtr sender);

public:
  PeakEnhancer(std::string name);
  PeakEnhancer(const PeakEnhancer& a);
  ~PeakEnhancer();
  MarSystem* clone() const;


  void myProcess(realvec& in, realvec& out);


};
}

#endif


