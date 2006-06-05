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
    \class Fanin
    \brief Fanin composite of MarSystem objects
    
    Executes the systems in paraellel for each 
observation stream and sums the results. 
*/

#ifndef MARSYAS_FANIN_H
#define MARSYAS_FANIN_H


#include "Composite.h"

class Fanin: public Composite
{
private:
  // vector<realvec*> slices_;
  void addControls();
  void deleteSlices();
  
public:
  Fanin();
  Fanin(std::string name);
  Fanin(const Fanin& a);  
  ~Fanin();
  MarSystem* clone() const;  

  void update();
  void process(realvec& in, realvec& out);
};



#endif

	

	
