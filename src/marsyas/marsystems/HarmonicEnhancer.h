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

#ifndef MARSYAS_HARMONICENHANCER_H
#define MARSYAS_HARMONICENHANCER_H

#include <marsyas/system/MarSystem.h>
#include <cfloat>

namespace Marsyas
{
/**
    \class HarmonicEnhancer
	\ingroup MachineLearning
    \brief Used to strengthen harmonic peaks in Beat Histograms

*/


class HarmonicEnhancer: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  void harm_prob(mrs_real& pmax, mrs_real factor,
                 mrs_real& s1, mrs_natural& t1,
                 mrs_real& s2, mrs_natural& t2,
                 mrs_natural tmx,
                 mrs_natural size,
                 realvec& in);

  realvec flag_;


public:
  HarmonicEnhancer(std::string name);
  ~HarmonicEnhancer();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

