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
    \class Confidence
    \brief Confidence calculates classification confidence 

    Calculates classification confidence by majority voting. 
*/

#ifndef MARSYAS_CONFIDENCE_H
#define MARSYAS_CONFIDENCE_H

#include "MarSystem.h"	

namespace Marsyas
{

class Confidence: public MarSystem
{
private: 
  void addControls();
	void localUpdate();

  realvec confidences_;
  mrs_natural count_;
  std::vector<std::string> labelNames_;
  bool print_;
  bool forcePrint_;
  
  mrs_natural predictions_;

public:
  Confidence(std::string name);
  ~Confidence();
  MarSystem* clone() const;  
  
  void process(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	
