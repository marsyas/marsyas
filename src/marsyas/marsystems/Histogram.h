/*
** Copyright (C) 1998-2017 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_HISTOGRAM_H
#define MARSYAS_HISTOGRAM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Histogram
	\ingroup Processing
	\brief Calculate amplitude histogram into output slide 

	Controls:
	- \b mrs_real/gain [w] : sets the gain multiplier.
	- \b mrs_bool/normalize [wr]: normalizes the histogram 
*/

class marsyas_EXPORT Histogram: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  MarControlPtr ctrl_gain_;
  MarControlPtr ctrl_normalize_;
  MarControlPtr ctrl_reset_;
  MarControlPtr ctrl_histoSize_;
public:
  /// Histogram constructor.
  Histogram(std::string name);

  /// Histogram copy constructor.
  Histogram(const Histogram& a);

  /// Histogram destructor.
  ~Histogram();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_HISTOGRAM_H

