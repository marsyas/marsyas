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

#ifndef MARSYAS_COMPEXP_H
#define MARSYAS_COMPEXP_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class CompExp
	\ingroup Processing
    \brief Multiply input realvec with CompExp

   Simple MarSystem example. Just multiply the values of the input realvec
with CompExp and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

/* How to use the Compressor/Expander Marsystem

	The system can be setup using the marsystem manager
		series->addMarSystem(mng.create("CompExp", "compexp"));

	Options: Threshold can be set to any value between -1.0 and 1
	Compressor  -1.0 <= thresh < 0
	Expander	0 <= thresh <= 1.0	
		series->updctrl("NoiseGate/noisegate/mrs_real/thresh", thresh);
		
	Attack time can be calculated using the following formula: at = 1 - exp(-2.2*T/t_AT)
	where at = attack time, T = sampling period, t_AT = time parameter 0.00016 < t_AT < 2.6 sec
		series->updctrl("NoiseGate/noisegate/mrs_real/rolloff", t_AT);
	
	Release time can be calculated similar to at time: rt = 1 - exp(-2.2*T/t_RT)
	where rt = release time, T = sampling period, t_RT = time parameter 0.001 < t_RT < 5.0 msec
		series->updctrl("NoiseGate/noisegate/mrs_real/rt", t_RT);
		
	Slope factor: 0 < slope <= 1.0
		series->updctrl("NoiseGate/noisegate/mrs_real/rt", slope);	
*/


class CompExp: public MarSystem
{
private: 
  void addControls();
	void myUpdate(MarControlPtr sender);

  mrs_real xdprev_;
  realvec xd_;
  realvec gains_;
  mrs_real alpha_;
  
public:
  CompExp(std::string name);
  ~CompExp();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
