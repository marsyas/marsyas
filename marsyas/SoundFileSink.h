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

/**
   \class SoundFileSink
   \brief Interface for sound file sink
   
   Abstract base class for any type of sound file input.
Specific IO classes for various formats like AuFileSink 
are children of this class. 
*/

	

#ifndef MARSYAS_SOUNDFILESINK_H
#define MARSYAS_SOUNDFILESINK_H


#include "realvec.h"
#include "AuFileSink.h" 
#include "WavFileSink.h"




#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)


# define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)



class SoundFileSink: public MarSystem
{
private: 
  virtual void addControls();
  
protected:
  short *sdata_;
  unsigned char *cdata_;
  long *ldata_;
  void ConvertToIeeeExtended(double num,char *bytes);
  double ieee_ext2double(unsigned char *bytes);
  std::string filename_;
  
  FILE *sfp_;  
  long sfp_begin_;

  AbsSoundFileSink* dest_;

  

  
  
public:
  SoundFileSink(std::string name);
  SoundFileSink(const SoundFileSink& a);
  ~SoundFileSink();
  MarSystem* clone() const;    
  bool checkType();
  
  
  
  virtual void process(realvec& in, realvec& out);
  virtual void update();
  virtual void putHeader();
  
  
  // void putLinear16(natural c, realvec& win);
  // void putLinear16Swap(natural c, realvec& win);
  void putFloat(natural c, realvec& win);
  
};



#endif /* !MARSYAS_SOUNDFILESINK_H */ 

	
