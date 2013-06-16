/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_ADSR_H
#define MARSYAS_ADSR_H  

#include "MarSystem.h"  

namespace Marsyas
{
/** 
    \class ADSRenvelope
    \ingroup Processing
    \brief Multiply input realvec by Value of Envelope

   Simple MarSystem example. Just multiply the values of the input realvec
with current value of the ADSR envelope and put them in the output vector.

*/


  
class ADSR: public MarSystem
{
private: 
    void addControls();
    mrs_real target_;
    mrs_real rate_;
    mrs_real value_;
    mrs_real nton_;
    mrs_real ntoff_;
    mrs_real aRate_; 
    mrs_real aTime_;
    mrs_real aTarget_;
    mrs_real dRate_;
    mrs_real dTime_;
    mrs_real susLevel_; 
    mrs_real rRate_;
    mrs_real rTime_;
    mrs_real sampleRate_;
    mrs_natural state_;

    mrs_bool bypass_;
    mrs_bool noteon_;
    mrs_bool noteoff_;

public:
    ADSR(std::string name);
    ~ADSR();
    MarSystem* clone() const;  
    
    void myUpdate(MarControlPtr sender);
    void myProcess(realvec& in, realvec& out);
};

} // namespace Marsyas 

#endif

