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

#ifndef MARSYAS_PVFOLD_H
#define MARSYAS_PVFOLD_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing

	Multiply with window (both length Nw) using modulus arithmetic;
	fold and rotate windowed input into output array (FFT) (length N)
	according to current input time (t)
*/


class PvFold: public MarSystem
{
private:
  realvec awin_;
  void addControls();
  void myUpdate(MarControlPtr sender);
  mrs_natural N_, Nw_, PNw_, PN_;
  int n_;
  MarControlPtr ctrl_rmsIn_;


public:
  PvFold(std::string name);
  PvFold(const PvFold& a);

  ~PvFold();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif






