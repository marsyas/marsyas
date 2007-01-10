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
    \class DeviBot
    \brief Control MahaDeviBot Midi Commands. 
*/

#ifndef MARSYAS_DEVIBOT_H
#define MARSYAS_DEVIBOT_H

#include "MarSystem.h"	

#ifdef MRSMIDI
#include "RtMidi.h"
#endif 

namespace Marsyas
{
  
  class DeviBot: public MarSystem
    {
    private: 
      void myUpdate();
      
      std::vector<unsigned char> message;
      
      
#ifdef MRSMIDI
      RtMidiOut* midiout;
#endif 
      int byte3, byte2, type;  
      
    public:
      DeviBot(std::string name);
      DeviBot(const DeviBot& a);
      ~DeviBot();
      
      MarSystem* clone() const;  
      
      int rval;
      
      // Robot Control
      int dha;
      int tu;  
      int ge;
      int na;  

      MarControlPtr ctrl_arm_; 
      MarControlPtr ctrl_velocity_;
      MarControlPtr ctrl_strike_;


      void addControls();
      void myProcess(realvec& in, realvec& out);
    };
  
}//namespace Marsyas

#endif
