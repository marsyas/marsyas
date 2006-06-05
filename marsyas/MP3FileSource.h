/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.cmu.edu>
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
   \class MP3FileSource
   \brief MP3FileSource read mp3 files using libmad
   \author Stuart Bray

This class reads an mp3 file using the mad mp3 decoder library.  Some
of this code was inspired from Bertrand Petit's madlld example.  The
code to resize the buffers was borrowed from Marsyas AudioSource.
	  

*/


#ifndef MARSYAS_MP3SOURCE_H
#define MARSYAS_MP3SOURCE_H

#include <stdio.h>

#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif 

#include "common.h"
#include "AbsSoundFileSource.h"

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
extern "C" {
#include "mad.h"
}
#endif 

#define INPUT_BUFFER_SIZE (5*8192)



class MP3FileSource: public AbsSoundFileSource
{
private:
 
  void addControls();
  natural getLinear16(realvec& slice);

#ifdef MAD_MP3

  inline signed int scale(mad_fixed_t sample);
  void madStructInitialize();
  void madStructFinish();
#endif

  void fillStream( natural offset = 0 );
  void closeFile(); 
  
  // MAD stuff
#ifdef MAD_MP3
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;
#endif

  natural fileSize_; 
  natural frameSamples_;
  natural totalFrames_;
  natural frameCount_;
  unsigned char* ptr_;
  int fd;
  FILE* fp;
  struct stat myStat;	 
  
  // variables for buffer balancing
  int bufferSize_;
  natural currentPos_;
  realvec reservoir_;
  natural reservoirSize_;
  natural preservoirSize_;
  natural ri_;
  natural offset;

  bool advance_;
  natural cindex_;
  
  real duration_;
  natural csize_;
  natural size_;
  natural samplesOut_;
  std::string filename_;
  real repetitions_;
  
public:

  MP3FileSource(std::string name);
  MP3FileSource(const MP3FileSource& a);
  ~MP3FileSource();
  MarSystem* clone() const;  
  void update();
  void process(realvec& in, realvec& out);
  void getHeader(std::string filename);
  
};

#endif	
