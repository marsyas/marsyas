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

#ifndef MARSYAS_SILENCEREMOVE_H
#define MARSYAS_SILENCEREMOVE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class SilenceRemove
	\ingroup Composites

	\brief Blocks tick()s if the output is silent.

	SilenceRemove takes as argument another Marsystem
	and ticks it ignoring the output when it is silent. Effectively
	tick silence remove is the same as playing the sound with
	silences removed.

	Controls:
	- \b mrs_real/threshold [rw] : threshold value of "silence".
*/


class SilenceRemove: public MarSystem
{
private:
  mrs_real threshold_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  MarControlPtr ctrl_hasData_;
  MarControlPtr ctrl_threshold_;

public:
  SilenceRemove(std::string name);
  SilenceRemove(const SilenceRemove&);
  ~SilenceRemove();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

