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

#ifndef MARSYAS_FILTER_H
#define MARSYAS_FILTER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Filter
	\ingroup Processing
	\brief Filter a Signal

	Filters a Signal using the coefficients stored in a FilterCoeffs
	object. The filter is implemented as a direct form II structure. This
	is a canonical form which has the minimum number of delay elements.

	\see OnePole
*/


class Filter: public MarSystem
{
private:
  /// Numerator order
  mrs_natural norder_;

  /// Denominator order
  mrs_natural dorder_;
  mrs_natural channels_;
  mrs_natural order_;
  realvec state_;
  mrs_real fgain_;
  realvec ncoeffs_;
  realvec dcoeffs_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Filter(std::string name);
  ~Filter();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  void write(std::string filename);
};

}//namespace Marsyas


#endif

