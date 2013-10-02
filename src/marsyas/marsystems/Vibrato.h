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

#ifndef MARSYAS_VIBRATO_H
#define MARSYAS_VIBRATO_H


#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Applies vibrato using time-varying delay
*/


class Vibrato: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec delayline_;
  mrs_natural delaylineSize_;
  mrs_real delay_;
  mrs_real width_;
  mrs_natural rp_, wp_, rpp_;
  mrs_natural tmod_;

public:
  Vibrato(std::string name);
  ~Vibrato();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
