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

#ifndef MARSYAS_RUNNINGAUTOCORRELATION_H
#define MARSYAS_RUNNINGAUTOCORRELATION_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class RunningAutocorrelation
	\ingroup Processing
	\brief Running calculation of autocorrelation values.

	This MarSystem calculates the autocorrelation function of the input signal
	over successive input slices up to a user defined maximum time lag
	(in samples).
	This is unlike the AutoCorrelation MarSystem, which computes the
	autocorrelation per slice and uses FFT to calculate the autocorrelation.

	Note that RunningAutocorrelation works seamlessly over slice boundaries
	as it keeps a buffer of previous samples to correctly calculate
	the autocorrelation values. This assumes that there is no overlap between
	successive slices.
	TODO: support overlap between slices (e.g. provide a control for skipping
	a certain amount of samples)


	Controls:
	- \b mrs_natural/maxLag : the maximum time lag (in samples) to calculate
*/

class marsyas_EXPORT RunningAutocorrelation: public MarSystem
{
private:

	/// Add specific controls needed by this MarSystem.
	void addControls();

	/// Reads changed controls and sets up variables if necessary.
	void myUpdate(MarControlPtr sender);


	/// Cache of the maxLag control value
	mrs_natural maxLag_;

	/// MarControlPtr for the gain control
	MarControlPtr ctrl_maxLag_;

	/// Internal buffer of the running autocorrelation values.
	realvec acBuffer_;

	/// Buffer of previous samples.
	realvec memory_;

public:
	/// RunningAutocorrelation constructor.
	RunningAutocorrelation(std::string name);

	/// RunningAutocorrelation copy constructor.
	RunningAutocorrelation(const RunningAutocorrelation& a);

	/// RunningAutocorrelation destructor.
	~RunningAutocorrelation();

	/// Implementation of the MarSystem::clone() method.
	MarSystem* clone() const;

	/// Implementation of the MarSystem::myProcess method.
	void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_RUNNINGAUTOCORRELATION_H

