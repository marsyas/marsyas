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

#ifndef MARSYAS_ANNOTATOR_H
#define MARSYAS_ANNOTATOR_H

#include "MarSystem.h"

#include <vector>

namespace Marsyas
{
/** 
    \class Annotator
	\ingroup MachineLearning
    \brief Annotate feature stream with additional row of labels.
    
*/


class Annotator: public MarSystem
{
private: 
  void addControls();
	void myUpdate(MarControlPtr sender);
  std::vector<mrs_natural> labels_;
  std::string labels_str_;
  mrs_natural labels_index_;
  
  MarControlPtr ctrl_label_;

public:
  Annotator(std::string name);
  Annotator(const Annotator& a);
  ~Annotator();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
	
