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

#ifndef MARSYAS_Biquad_H
#define MARSYAS_Biquad_H

#include <marsyas/system/MarSystem.h>
#include "Filter.h"

namespace Marsyas
{
/**
  \class Biquad
  \ingroup Processing
  \brief Filter a Signal

  \todo I don't know how Biquad works (inObs?!), if it works at all.
Investigate.

*/


class Biquad: public MarSystem
{
private:
  mrs_real q_;
  mrs_real freq_;

  mrs_real fs_;
  mrs_real alpha_;
  mrs_real w0_;

  realvec b;
  realvec a;

  Filter* filter;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Biquad(std::string name);
  Biquad(const Biquad& orig);
  ~Biquad();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif

