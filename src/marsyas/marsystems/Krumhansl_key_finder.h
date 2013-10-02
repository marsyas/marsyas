/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_KRUMHANSL_KEY_FINDER_H
#define MARSYAS_KRUMHANSL_KEY_FINDER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Krumhansl_key_finder
	\ingroup Analysis
	\brief Krumhansl-Schmuckler Key-Finding Algorithm

Performs simple chord detection using the Krumhansl-Schmuckler Key-Finding Algorithm.
The input is a pitch class profile or chroma vector with 12 values one for each chromatic note.

	Controls:
*/

class marsyas_EXPORT Krumhansl_key_finder: public MarSystem
{
private:

  mrs_realvec major_profile_;
  mrs_realvec minor_profile_;
  mrs_realvec scores_;
  std::vector<mrs_string> key_names_;


  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  MarControlPtr ctrl_key_;
  MarControlPtr ctrl_key_name_;


public:
  /// Krumhansl_key_finder constructor.
  Krumhansl_key_finder(std::string name);

  /// Krumhansl_key_finder copy constructor.
  Krumhansl_key_finder(const Krumhansl_key_finder& a);

  /// Krumhansl_key_finder destructor.
  ~Krumhansl_key_finder();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_KRUMHANSL_KEY_FINDER_H

