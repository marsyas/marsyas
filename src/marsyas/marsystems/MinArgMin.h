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

#ifndef MARSYAS_MINARGMIN_H
#define MARSYAS_MINARGMIN_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class MinArgMin
    \ingroup Analysis
    \brief Calculate k minimums and their positions

    The output is : min1, argmin1, min2, argmin2, ....
*/


class MinArgMin: public MarSystem
{
private:
  mrs_natural ki;
  realvec maximums_;
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  MinArgMin(std::string name);
  ~MinArgMin();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif





