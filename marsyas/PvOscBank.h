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
    \class PvOscBank
    \brief PvOscBank

    A bank of sine wave oscillators used for phasevocoding. 

*/


#ifndef MARSYAS_OSCBANK_H
#define MARSYAS_OSCBANK_H

#include "MarSystem.h"	

class PvOscBank: public MarSystem
{
private:
  realvec lastamp_;
  realvec lastfreq_;
  realvec index_;
  realvec table_;
  natural L_;
  natural N_;
  natural NP_;
  real P_;
  real Iinv_;
  real Pinc_;
  real R_;
  natural I_;
  real S_;
  natural size_, psize_;
  real address_,paddress_, ppaddress_,f_, finc_, a_, ainc_;
  natural naddress_;
  real tableval_;
  

  void addControls();
  
public:
  PvOscBank(std::string name);
  
  ~PvOscBank();
  MarSystem* clone() const;    
  
  void update();
  void process1(realvec& in, realvec& out);
  void process(realvec& in, realvec& out);
  
};

#endif

	

	

	
