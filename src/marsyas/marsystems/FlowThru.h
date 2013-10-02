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

#ifndef MARSYAS_FLOWTHRU_H
#define MARSYAS_FLOWTHRU_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class FlowThru
	\ingroup Composites
	\brief FlowThru of MarSystem objects

Combines a series of MarSystem objects to a single MarSystem
corresponding to executing the System objects one after the other
in sequence, but forwards the original composite input flow to the output.

	Controls:
	- \b mrs_realvec/innerOut [r] : this control contains the output of the last child of the composite
*/


class FlowThru: public MarSystem
{
private:
  MarControlPtr ctrl_innerOut_;
  realvec empty_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  FlowThru(std::string name);
  FlowThru(const FlowThru& a);
  ~FlowThru();
  MarSystem* clone() const;


  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
