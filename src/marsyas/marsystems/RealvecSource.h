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

#ifndef MARSYAS_REALVECSOURCE_H
#define MARSYAS_REALVECSOURCE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class RealvecSource
	\ingroup IO

	A RealvecSource takes a realvec and uses it as a source for your
	MarSystem network.  It is similar to a SoundFileSource except that
	as taking a sound file as the input, it takes a realvec that you
	create as input.

	It is very useful for getting numbers from external sources into
	your Marsystems, for example when reading external files of data.

	It is also very useful when writing tests for your Marsystems.
	You can load up a realvec with data, and then run it through a
	Marsystem to test it.

	When you feed in a realvec, the rows turn into observations and
	the columns turn into samples.

*/


class RealvecSource: public MarSystem
{
private:

  MarControlPtr ctrl_data_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_natural count_;
  mrs_natural samplesToUse_;

public:
  RealvecSource(std::string name);
  RealvecSource(const RealvecSource& a);
  ~RealvecSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

