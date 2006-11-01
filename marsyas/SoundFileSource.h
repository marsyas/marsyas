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

#include <cstdio>
#include <cmath>

namespace Marsyas
{

class SoundFileSource: public MarSystem
{
private:
	std::string filename_;
  mrs_natural sampleSize_; //in bytes
  mrs_natural samplesRead_;
  mrs_natural samplesToRead_;
  mrs_natural nChannels_;
  
  AbsSoundFileSource* src_;

	bool advance_;

	void addControls();
	void myUpdate();
  
public:
  SoundFileSource(std::string name);
  SoundFileSource(const SoundFileSource& a);
  
  ~SoundFileSource();
  MarSystem* clone() const;    
 
  virtual void myProcess(realvec& in,realvec& out);
  virtual bool checkType();
  virtual void getHeader();

};

}//namespace Marsyas

#endif /* !MARSYAS_SOUNDFILESOURCE_H */ 
