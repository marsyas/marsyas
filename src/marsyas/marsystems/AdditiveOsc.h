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

#ifndef MARSYAS_ADDITIVEOSC_H
#define MARSYAS_ADDITIVEOSC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Synthesis
	\brief Generates what is perceptually a saw wave.

	Creates what is perceptually a saw wave using wave guides to
	generate each harmonic.

	TODO: Make actually generate a saw wave.
	      Add ability to choose wave shape
		      eg: Saw, Square, Triangle
		  Add ability to vary pitch over time

	Controls:
	- \b mrs_real/frequency [w] : the frequency in hz.
	- \b mrs_bool/noteon [w] : currently not implemented.
*/


class AdditiveOsc: public MarSystem
{
private:

  mrs_real harmonics_; // How many harmonics our signal will have

  realvec x1n1_;  // List of Last x1s
  realvec x2n1_;  // List of Last x2s
  realvec k_;     // List of filter coefficient

  mrs_real israte_; // Sampling rate of the system


  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  AdditiveOsc(std::string name);
  AdditiveOsc(const AdditiveOsc& a);
  ~AdditiveOsc();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
