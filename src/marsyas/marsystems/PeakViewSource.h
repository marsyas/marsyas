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

#ifndef MARSYAS_PEAKVIEWSOURCE_H
#define MARSYAS_PEAKVIEWSOURCE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup IO
	\brief MarSystem to read .peak files and at each tick output the peaks in each frame

	Controls:
	- \b mrs_string/filename [w] : .peak file name to read
	- \b mrs_bool/ignoreGroups : flag to ignore the group labels in the peak files and set all group indices to 0
*/

class PeakViewSource: public MarSystem
{
private:
  std::string filename_;
  realvec peakData_;

  mrs_natural frameSize_;
  mrs_natural numFrames_;
  mrs_natural frameIdx_;

  MarControlPtr ctrl_filename_;
  MarControlPtr ctrl_hasData_;
  MarControlPtr ctrl_pos_;
  MarControlPtr ctrl_ignGroups_,
                ctrl_noNegativeGroups_;
  MarControlPtr ctrl_frameMaxNumPeaks_;
  MarControlPtr ctrl_totalNumPeaks_;
  MarControlPtr ctrl_nTimes_;
  MarControlPtr ctrl_size_;

  void discardNegativeGroups (mrs_realvec &output);
  void defaultConfig();
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PeakViewSource(std::string name);
  PeakViewSource(const PeakViewSource& a);
  ~PeakViewSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



