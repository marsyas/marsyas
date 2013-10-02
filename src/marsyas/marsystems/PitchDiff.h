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

#ifndef MARSYAS_PITCHDIFF_H
#define MARSYAS_PITCHDIFF_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Difference between detected and expected pitch

	Place this in a series after Yin (or any other
	pitch-detection MarSystem), and feed it the expected pitch as a
	control.  Outputs the difference as a midi pitch value.

	Controls:
	- \b mrs_real/expectedPitch [w] : expected pitch
	- \b mrs_bool/ignoreOctaves [w] : useful if the pitch
	  detection algorithm has octave errors, but is otherwise
	  relatively accurate (default false)
	- \b mrs_bool/absoluteValue [w] : outputs the absolute
	  difference (default false)
*/

class marsyas_EXPORT PitchDiff: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  MarControlPtr ctrl_expectedPitch_;
  MarControlPtr ctrl_ignoreOctaves_;
  MarControlPtr ctrl_absoluteValue_;
  // for efficiency ?
  mrs_real expectedMidiPitch_;

public:
  /// PitchDiff constructor.
  PitchDiff(std::string name);

  /// PitchDiff copy constructor.
  PitchDiff(const PitchDiff& a);

  /// PitchDiff destructor.
  ~PitchDiff();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_PITCHDIFF_H
