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

#include "RunningAutocorrelation.h"

using namespace std;
using namespace Marsyas;

RunningAutocorrelation::RunningAutocorrelation(string name) :
	MarSystem("RunningAutocorrelation", name) {
	/// Add any specific controls needed by this MarSystem.
	addControls();
}

RunningAutocorrelation::RunningAutocorrelation(const RunningAutocorrelation& a) :
	MarSystem(a) {
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
	ctrl_maxLag_ = getctrl("mrs_natural/maxLag");
}

RunningAutocorrelation::~RunningAutocorrelation() {
}

MarSystem*
RunningAutocorrelation::clone() const {
	return new RunningAutocorrelation(*this);
}

void RunningAutocorrelation::addControls() {
	/// Add any specific controls needed by this MarSystem.
	addctrl("mrs_natural/maxLag", 15, ctrl_maxLag_);
	setctrlState("mrs_natural/maxLag", true);
}

void RunningAutocorrelation::myUpdate(MarControlPtr sender) {
	MRSDIAG("RunningAutocorrelation.cpp - RunningAutocorrelation:myUpdate");

	// Update the cache of the maxLag control value.
	maxLag_ = ctrl_maxLag_->to<mrs_natural> ();
	// TODO: check if it is to small (<1) or too large?

	// Output flow:
	ctrl_onSamples_->setValue(1, NOUPDATE);
	onObservations_ = (mrs_natural) ((maxLag_ + 1) * ctrl_inObservations_->to<
			mrs_natural> ());
	ctrl_onObservations_->setValue(onObservations_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	// Prefix the observation with Autocorr<lag>_.
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string> ();
	mrs_string onObsNames("");
	for (int lag = 0; lag <= maxLag_; lag++) {
		ostringstream oss("Autocorr");
		oss << lag << "_";
		onObsNames += obsNamesAddPrefix(inObsNames, oss.str());
	}
	ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

	// Allocate and initialize the buffers and counters.
	this->acBuffer_.stretch(onObservations_, 1);
	this->acBuffer_.setval(0.0);

}

void RunningAutocorrelation::myProcess(realvec& in, realvec& out) {
	/// Iterate over the observations and samples and do the processing.
	for (mrs_natural i = 0; i < inObservations_; i++) {
		for (mrs_natural lag = 0; lag <= maxLag_; lag++) {
			o = i * (maxLag_ + 1) + lag;
			for (mrs_natural n = lag; n < inSamples_; n++) {
				acBuffer_(o, 0) += in(i, n) * in(i, n - lag);
			}
			out(o, 0) = acBuffer_(o, 0);
		}
	}
}

