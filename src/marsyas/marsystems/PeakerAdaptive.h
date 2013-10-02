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

#ifndef MARSYAS_PEAKER1_H
#define MARSYAS_PEAKER1_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Pick peaks out of signal

	PeakerAdaptive is used to select peaks(or valleys) from the input vector. Various
	parameters of the peak selection process can be adjusted.
*/


class PeakerAdaptive: public MarSystem
{
private:
  mrs_real rms_;
  mrs_natural winCount_;
  mrs_natural peakHysterisis_;

  void addControls();

public:
  PeakerAdaptive(std::string name);
  ~PeakerAdaptive();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


