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
    \class MeddisHairCell
    \brief MeddisHairCell for auditory models 

    Directed port from the Auditory toolbox by Malcolm Slaney 
*/


#ifndef MARSYAS_MEDDISHAIRCELL_H
#define MARSYAS_MEDDISHAIRCELL_H

#include "MarSystem.h"





class MeddisHairCell: public MarSystem
{
private: 
  void addControls();
  
  natural numChannels;
  
  //parameters
  real M;
  real A;
  real B;
  real g;
  real y;
  real l;
  real r;
  real x;
  real h;
  
  //internal constants
  real dt;
  real gdt;
  real ydt;
  real ldt;
  real rdt;
  real xdt;
  
  //initial values
  real kt;
  real spont;
  
  realvec c;
  realvec q;
  realvec w;
  
public:
  MeddisHairCell(std::string name);
  ~MeddisHairCell();
  
  MarSystem* clone() const;
  void update();
  void process(realvec& in, realvec& out); 
};

#endif 

