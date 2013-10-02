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

#ifndef MARSYAS_BEATHISTOFEATURES_H
#define MARSYAS_BEATHISTOFEATURES_H

#include <marsyas/system/MarSystem.h>
#include "MaxArgMax.h"
#include "Peaker.h"

namespace Marsyas
{
/**
    \class BeatHistoFeatures
	\ingroup MachineLearning
    \brief Beat Histogram Features

    Beat Histogram Features numerical values summarizing
	the BH as described in the TSAP paper. Exact details
	keep changing.

*/

class BeatHistoFeatures: public MarSystem
{
private:
  MarSystem* mxr_;
  MarSystem* pkr_;
  MarSystem* pkr1_;

  MarControlPtr ctrl_mode_;

  realvec mxres_;
  realvec pkres_;
  realvec pkres1_;

  realvec flag_;

  void harm_prob(mrs_real& pmax, mrs_real factor,
                 mrs_real& s1, mrs_natural& t1,
                 mrs_real& s2, mrs_natural& t2,
                 mrs_natural tmx,
                 mrs_natural size,
                 const realvec& in);

  // helper function used by harm_prob
  mrs_real sum_nearby(mrs_natural index, mrs_natural radius,
                      mrs_natural size, const realvec& in);

  void myUpdate(MarControlPtr sender);
  void beatHistoFeatures(realvec& in, realvec& out);

  mrs_real NumMax (mrs_realvec& in);

  void addControls();
public:
  BeatHistoFeatures(std::string name);
  BeatHistoFeatures(const BeatHistoFeatures& a);

  ~BeatHistoFeatures();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


