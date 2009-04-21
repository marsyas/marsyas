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

#include <string>
#include <sstream>

#include "RunningStatistics.h"

using namespace std;
using namespace Marsyas;

RunningStatistics::RunningStatistics(string name):MarSystem("RunningStatistics", name)
{
	/// Add any specific controls needed by this MarSystem.
	addControls();
}

RunningStatistics::RunningStatistics(const RunningStatistics& a) : MarSystem(a)
{
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
}


RunningStatistics::~RunningStatistics()
{
}

MarSystem*
RunningStatistics::clone() const
{
	return new RunningStatistics(*this);
}

void
RunningStatistics::addControls()
{
	// No controls (so far).
}

/**
 * Helper function for prepending the observation names with a prefix.
 * \par observationNames string of observation names (comma separated)
 * \par prefix the prefix the prepend to all the observation names.
 * \return new comma separated observation name string
 * \todo Put this in a more general file, and use it, search for example for occurrences of 'find(",")'
 */
mrs_string prependObservationNames(mrs_string observationNames, mrs_string prefix) {
	ostringstream oss;
	size_t startPos = 0, endPos=0;
	while ((endPos = observationNames.find(",", startPos)) != string::npos) {
		// Extract the observation name.
		mrs_string name = observationNames.substr(startPos, endPos-startPos);
		oss << prefix << name << ",";
		// Update the start position for the next name.
		startPos = endPos + 1;
	}
	return oss.str();
}

void
RunningStatistics::myUpdate(MarControlPtr sender)
{
	MRSDIAG("RunningStatistics.cpp - RunningStatistics:myUpdate");

	// Set the output slice format and rate based on the
	// input slice format and rate.
	ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
	onObservations_ = (mrs_natural)(2*ctrl_inObservations_->to<mrs_natural>());
	ctrl_onObservations_->setValue(onObservations_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	// Update the observation names.
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	mrs_string onObsNames("");
	onObsNames += prependObservationNames(inObsNames, "RunningMean_");
	onObsNames += prependObservationNames(inObsNames, "RunningStddev_");
	ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

	// Allocate and initialize the buffers and counters.
	this->sumBuffer_.stretch(onObservations_, 1);
	this->sumBuffer_.setval(0.0);
	this->squaredSumBuffer_.stretch(onObservations_, 1);
	this->squaredSumBuffer_.setval(0.0);
	this->samplecounter_ = 0;
}

void
RunningStatistics::myProcess(realvec& in, realvec& out)
{

	// Update the sample counter.
	this->samplecounter_ += inSamples_;
	mrs_real mean, var, stddev;

	for (o=0; o < inObservations_; o++)
	{
		// Gather the sums.
		for (t = 0; t < inSamples_; t++)
		{
			this->sumBuffer_(o, 0) += in(o, t);
			this->squaredSumBuffer_(o, 0) += in(o, t) * in(o, t);
		}
		// Calculate the mean
		out(o, 0) = mean = this->sumBuffer_(o, 0) / this->samplecounter_;
		// Calculate the standard deviation
		var = this->squaredSumBuffer_(o, 0) / this->samplecounter_  - mean * mean;
		out(inObservations_ + o, 0) =	sqrt(var);
	}
}


