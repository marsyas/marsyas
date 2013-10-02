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

#ifndef MARSYAS_PEAKPERIODS2BPM_H
#define MARSYAS_PEAKPERIODS2BPM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief PeakPeriods2BPM: convert peaks to beats per minute.

	This block should have as input a vector with the peak amplitudes in its
	pair indexes and the peak arguments (in frames) in its odd indexes.
	It was created to convert the input given by "MaxArgMax" which outputs the chosen
	number of peaks in the referred form.

	Controls:
	- \b mrs_real/srcFs [w] : Input sampling rate of the sound file source
	(given by "SoundFileSource/src/mrs_real/israte") -> by default = 1.0Hz.

	- \b mrs_natural/hopSize [w] : hopsize of the analysis -> by default = 1.
*/


class PeakPeriods2BPM: public MarSystem
{
private:
  void addControls();
  mrs_real factor_;

  mrs_real srate_;

  void myUpdate(MarControlPtr sender);

public:
  PeakPeriods2BPM(std::string name);
  ~PeakPeriods2BPM();


  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


