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

/** 
    \class BeatHistoFeatures
    \brief Beat Histogram Features

    Beat Histogram Features numerical values summarizing 
the BH as described in the TSAP paper. Exact details 
keep changing. 

*/

#ifndef MARSYAS_BEATHISTOFEATURES_H
#define MARSYAS_BEATHISTOFEATURES_H

#include "MarSystem.h"	
#include "MaxArgMax.h" 




class BeatHistoFeatures: public MarSystem
{
private: 
  void addControls();
  MarSystem* mxr_;
  realvec mxres_;
  realvec flag_;
  
  void harm_prob(real& pmax, real factor, 
			    real& s1, natural& t1, 
			    real& s2, natural& t2, 
			    natural tmx,
			    natural size, 
		 realvec& in);
  
public:
  BeatHistoFeatures(std::string name);
  BeatHistoFeatures(const BeatHistoFeatures& a);
  
  ~BeatHistoFeatures();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

#endif

	
