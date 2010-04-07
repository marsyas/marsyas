/*
** Copyright (C) 2010 Stefaan Lippens
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
	\ingroup Analysis
	\brief Running calculation (across slices) of the autocorrelation values.

	This MarSystem calculates the autocorrelation function of the input signal
	defined by successive input slices. Unlike the AutoCorrelation MarSystem,
	the calculations are done across slice boundaries in a seamless fashion
	(RunningAutocorrelation keeps an internal buffer of the appropriate
	number of samples from previous slices to implement this feature).
	Calculations are done in time domain for time lags from 0 to a user defined
	maximum lag (in number of samples).
	Note that this assumes that the input slices are non overlapping slices.

	The autocorrelation values are laid out in the output slice along the
	time/samples dimension from lag zero to the maximum lag.
	Multiple input observation channels are supported.
	For example, if there are two input channels and the maximum lag is 5,
	the output slice will have two rows and six (not five) columns:
		Rxx[0], Rxx[1], ..., Rxx[5]
		Ryy[0], Ryy[1], ..., Ryy[5]
	with Rxx[n] the autocorrelation of the first channel for lag n
	and Ryy[n] the autocorrelation of the second channel.

	TODO: provide a reset control

	TODO: support overlap between slices (e.g. provide a control for skipping
	a certain amount of samples).

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

