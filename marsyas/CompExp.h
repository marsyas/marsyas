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
    \class Limiter
    \brief Multiply input realvec with Limiter

   Simple MarSystem example. Just multiply the values of the input realvec
with Limiter and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

#ifndef MARSYAS_COMPEXP_H
#define MARSYAS_COMPEXP_H


#include "MarSystem.h"	


class CompExp: public MarSystem
{
private: 
  void addControls();
  real xdprev_;
  realvec xd_;
  realvec gains_;
  float alpha_;
  
public:
  CompExp(std::string name);
  ~CompExp();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

#endif
