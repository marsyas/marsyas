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
   \class SoundFileSource
   \brief Interface for sound file sources 
   
   Abstract base class for any type of sound file input.
Specific IO classes for various formats like AuFileSource 
are children of this class. 
*/


	

#ifndef MARSYAS_SOUNDFILESOURCE_H
#define MARSYAS_SOUNDFILESOURCE_H


#include "realvec.h"
#include "AuFileSource.h"
#include "WavFileSource.h"
#include "RawFileSource.h"



#ifdef WIN32

#ifdef CYGWIN 
#include "config.h" 
#else 
#include "win_config.h"
#endif 


#else 
#include "config.h" 
#endif 



#ifdef MAD_MP3
#include "MP3FileSource.h"
#endif

#ifdef OGG_VORBIS
#include "OggFileSource.h"
#endif

#include "CollectionFileSource.h"

#include <stdio.h>
#include <math.h>



#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)


# define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)



class SoundFileSource: public MarSystem
{
private:
  virtual void addControls();


protected:
  short *sdata_;
  unsigned char *cdata_;
  FILE *sfp_;
  

  long sfp_begin_;
  std::string filename_;
  natural sampleSize_;			// in bytes
  natural samplesRead_;
  natural samplesToRead_;
  natural nChannels_;
  natural inSamples_;
  natural inObservations_;
  real israte_;
  
  AbsSoundFileSource* src_;
  bool advance_;
  bool shuffle_;
  
  natural cindex_;
  std::string currentlyPlaying_;
  
  
  
public:
  SoundFileSource(std::string name);
  SoundFileSource(const SoundFileSource&);
  
  ~SoundFileSource();
  MarSystem* clone() const;    
  void setName(std::string name);
  
  virtual void update();
  virtual void process(realvec& in,realvec& out);
  virtual bool checkType();
  virtual void getHeader();

};



#endif /* !MARSYAS_SOUNDFILESOURCE_H */ 
