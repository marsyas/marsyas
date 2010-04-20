/*
** Copyright (C) 2009 Stefaan Lippens <soxofaan@gmail.com>
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

#ifndef MARSYAS_RUNNINGSTATISTICS_H
#define MARSYAS_RUNNINGSTATISTICS_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class RunningStatistics
	\ingroup Analysis
	\brief Gathers the running average, variance, standard deviation, etc.

	Outputs the running average and standard deviation of all the input so far.

	Controls:
	- \b mrs_natural/enableMean: enable outputting of the mean values (on by
		default).
	- \b mrs_natural/enableStddev: enable outputting of the standard deviation
		values (on by default).
	- \b mrs_natural/enableSkewness: enable outputting of the skewness
		values (off by default).


	\todo: add kurtosis
	\todo: add option to output running energy (we're calculating it anyway)
	\todo: add control to reset on every tick (disabling the "running" aspect)
*/

class RunningStatistics: public MarSystem
{
private:

	/// Add specific controls needed by this MarSystem.
	void addControls();

	/// Reads changed controls and sets up variables if necessary.
	void myUpdate(MarControlPtr sender);

	/// Internal buffer for keeping the running sum.
	realvec sumxBuffer_;

	/// Internal buffer for keeping the running sum of sample^2.
	realvec sumx2Buffer_;

	/// Internal buffer for keeping the running sum of sample^3.
	realvec sumx3Buffer_;

	/// Internal counter for counting the number of processed samples.
	mrs_natural samplecounter_;

	/// MarControlPtr for enabling the mean.
	MarControlPtr ctrl_enable_mean_;
	/// Cache of the enable mean control value.
	mrs_bool enable_mean_;

	/// MarControlPtr for enabling the standard deviation.
	MarControlPtr ctrl_enable_stddev_;
	/// Cache of the enable stddev control value.
	mrs_bool enable_stddev_;

	/// MarControlPtr for enabling the skewness.
	MarControlPtr ctrl_enable_skewness_;
	/// Cache of the enable skewness control value.
	mrs_bool enable_skewsness_;


public:
	RunningStatistics(std::string name);
	RunningStatistics(const RunningStatistics& a);
	~RunningStatistics();
	MarSystem* clone() const;

	/// The core processing method of this MarSystem.
	void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_RUNNINGSTATISTICS_H

