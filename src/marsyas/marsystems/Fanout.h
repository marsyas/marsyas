/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_FANOUT_H
#define MARSYAS_FANOUT_H

#include <marsyas/system/MarSystem.h>
#include <cstddef>
#include <vector>

namespace Marsyas
{
/**
	\class Fanout
	\ingroup Composites
	\brief Fanout of MarSystem objects

	All the children Marsystems of this composite receive the same
	input and their outputs are stacked as rows of observations.

	Controls:
	- \b mrs_natural/disable [w] : passes in the number of the child
	MarSystem to disable.
	- \b mrs_natural/enable	[w] : pass in the number of the child MarSystem to
	enable.  (by default all children are enabled)
	- \b mrs_string/enableChild	[w] : pass in the type/name of the child MarSystem to
	enable.  (by default all children are enabled)
	- \b mrs_string/disableChild	[w] : pass in the type/name of the child MarSystem to
	disable.
  -\b mrs_realvec/enabled [r] : vector with the list of enabled (i.e. 1.0 valued) children
 (no point writing to this control - just use it for reading the current enable/disable status of children)
 -\b mrs_realvec/muted [rw] : vector with the list of muted (i.e. 1.0 valued) children. This control can be used
 to bypass processing of some children (which when muted just keep outputing the last computed result).
*/


class marsyas_EXPORT Fanout: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);
  void deleteSlices();

  MarControlPtr ctrl_enabled_;
  MarControlPtr ctrl_muted_;

  struct child_info {
    child_info(): enabled(true) {}
    bool enabled;
    bool output;
    //int observation_count;
    realvec buffer;
  };

  std::vector<child_info> children_info_;

  //realvec buffer_;

public:
  Fanout(std::string name);
  Fanout(const Fanout& a);
  ~Fanout();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif




