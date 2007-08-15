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


#ifndef MARSYAS_ENVELOPE_H
#define MARSYAS_ENVELOPE_H	

#include "MarSystem.h"	


namespace Marsyas 
{
/** 
    \class Envelope
	\ingroup Processing
    \brief Multiply input realvec by Value of Envelope

   Simple MarSystem example. Just multiply the values of the input realvec
with current value of the envelope and put them in the output vector.
*/


  
class Envelope: public MarSystem
{
private: 
  void addControls();
  mrs_real target_;
  mrs_real rate_;
  mrs_real time_;
  mrs_real value_;
  mrs_real noteon_;
  mrs_real noteoff_;
  mrs_real sampleRate_;
  
  mrs_natural state_;
 

public:
  Envelope(std::string name);
  ~Envelope();
  MarSystem* clone() const;  
  
  void update();
  void myProcess(realvec& in, realvec& out);
};
 
} // namespace Marsyas

#endif
  
