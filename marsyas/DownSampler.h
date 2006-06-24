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
    \class DownSampler
    \brief Downsample the input signal

    Downsample the signal by an integer factor.
*/

#ifndef MARSYAS_DOWNSAMPLER_H
#define MARSYAS_DOWNSAMPLER_H

#include "MarSystem.h"	

namespace Marsyas
{

class DownSampler: public MarSystem
{
private: 
  void addControls();
public:
  DownSampler(std::string name);
  ~DownSampler();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
	
