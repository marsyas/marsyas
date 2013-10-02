/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_BEATHISTOGRAM_H
#define MARSYAS_BEATHISTOGRAM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class BeatHistogram
    \ingroup Analysis
    \brief BeatHistogram

    Calculate Beat Histogram. The input should be the autocorrelation of either
	the time waveform or some kind of onset detection function.
*/


class BeatHistogram: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_natural startBin_;
  mrs_natural endBin_;
  bool reset_;
  mrs_real factor_;
  mrs_real alpha_;

public:
  BeatHistogram(std::string name);
  ~BeatHistogram();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


