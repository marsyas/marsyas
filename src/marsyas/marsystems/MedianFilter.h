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

#ifndef MARSYAS_MEDIANFILTER_H
#define MARSYAS_MEDIANFILTER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup	Processing
\brief		Apply median filter on input vector
\author		Matthias Varewyck
\date		20090518

This class represents a median filter.  Each element in an input vector	is
replaced by the median of the elements that fall in a window surrounding the element,
defined as: element index + [0,1.. N-1] - floor(N/2) with N, the size of the window.

Controls:
- \b mrs_real/WindowSize [rw] : adjust the window size (N)
*/

class MedianFilter: public MarSystem
{
public:
  MedianFilter(mrs_string inName);
  MedianFilter(const MedianFilter& inToCopy);

  ~MedianFilter();

  MarSystem* clone() const;

  void addControls();
  void myUpdate(MarControlPtr inSender);
  void myProcess(realvec& inVec, realvec& outVec);

private:
  // Pointer to MarControllers
  MarControlPtr ctrl_WindowSize_;

  // Member variable
  mrs_natural WindowSize_;		// Size of moving window
};

}	// End namespace

#endif
