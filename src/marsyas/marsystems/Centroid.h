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


#ifndef MARSYAS_CENTROID_H
#define MARSYAS_CENTROID_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Analysis
\brief Centroid of observations within each sample.

Centroid computes the centroid of the observations for each
time sample. The center is defined as the normalized first moment
(center of gravity) of the observation vector.
*/


class Centroid: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  mrs_real m0_;
  mrs_real m1_;

public:
  Centroid(std::string name);

  ~Centroid();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


