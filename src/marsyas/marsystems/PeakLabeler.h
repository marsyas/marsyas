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

#ifndef MARSYAS_PELABELER_H
#define MARSYAS_PELABELER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Special
	\brief Assigns clustering labels to the "pkGroup" feature of the input peak realvec,
	based on the information received at the mrs_realvec/labels control.

	Controls:
	- \b mrs_realvec/labels [w] : cluster labels to be assigned to each peak received at the input.

*/

class PeakLabeler: public MarSystem
{
private:
  MarControlPtr ctrl_peakLabels_;
  mrs_natural labelIdx_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PeakLabeler(std::string name);
  PeakLabeler(const PeakLabeler& a);
  ~PeakLabeler();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



