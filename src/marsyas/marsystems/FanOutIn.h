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

#ifndef MARSYAS_FanOutIn_H
#define MARSYAS_FanOutIn_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class FanOutIn
\ingroup Composites
\brief FanOutIn of MarSystem objects

Combines a parallel set of MarSystem objects to a single MarSystem
corresponding to executing the System objects in parallel using the
same input, and combining their outputs using a sum or a multiplication
combinator.

Controls:
- \b mrs_string/combinator [rw] : choose a * or + combination operation

*/

class FanOutIn: public MarSystem
{
private:
  realvec enabled_;

  mrs_natural enable_, enableChildIndex_;
  mrs_natural disable_, disableChildIndex_;

  mrs_string enableChild_;
  mrs_string disableChild_;

  bool wrongOutConfig_;

  MarControlPtr ctrl_combinator_;

  std::vector<realvec*> slices_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  void deleteSlices();

public:
  FanOutIn(std::string name);
  FanOutIn(const FanOutIn& a);
  ~FanOutIn();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif




