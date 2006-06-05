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
    \class ERB
    \brief ERB Filterbank

    ERB (Equivalent rectangular bandwith) filterbank. Based on 
the code from the Auditory Toolbox by Malcolm Slaney. 
*/



#ifndef MARSYAS_ERB_H
#define MARSYAS_ERB_H

#include "MarSystem.h"
#include "Fanout.h"


class ERB: public MarSystem
{
private: 
  void addControls();
  
  real lowFreq;
  real highFreq;
  real fs;
  real EarQ;
  real minBW;
  
  real A0, A2, B0;
  
  natural numChannels;
  natural order;
  
  realvec fcoefs;
  realvec centerFreqs;
  
  real E(real);
  real B(real);
  real B1(real, real);
  real B2(real);
  real A11(real, real);
  real A12(real, real);
  real A13(real, real);
  real A14(real, real);
  real gain(real, real);
  real abs(real, real);
  
  Fanout *filterBank;
  
public:
  ERB(std::string name);
  ~ERB();
  
  MarSystem* clone() const;
  void update();
  void process(realvec& in, realvec& out); 
};

#endif

