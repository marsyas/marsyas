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
   \class AuFileSource
   \brief SoundFileSource for .au (.snd) soundfiles
   
   SoundFileSource reader for .au (.snd) sound files.
   (Next, Sun audio format). 
*/




#ifndef MARSYAS_AUFILESOURCE_H
#define MARSYAS_AUFILESOURCE_H





#include "AbsSoundFileSource.h"

#include <string> 
#include <vector> 


#define SND_MAGIC_NUM 0x2e736e64

/********  NeXT/Sun Soundfile Header Struct   *******/

struct snd_header 
{
  char pref[4];
  long hdrLength;
  long fileLength;
  long mode;
  long srate;
  long channels;
  char comment[1024];
};


/* Array containing descriptions of
the various formats for the samples
of the Next .snd/ Sun .au format */


/* types of .snd files */  
#define SND_FORMAT_UNSPECIFIED 0
#define SND_FORMAT_MULAW_8     1
#define SND_FORMAT_LINEAR_8    2
#define SND_FORMAT_LINEAR_16   3
#define SND_FORMAT_LINEAR_24   4
#define SND_FORMAT_LINEAR_32   5
#define SND_FORMAT_FLOAT       6




class AuFileSource: public AbsSoundFileSource
{
private:
  snd_header hdr_;
  std::string tfname_;
  std::string filename_;
  
  short *sdata_;
  short sval_;
  unsigned short usval_;
  
  unsigned char *cdata_;
  FILE *sfp_;
  long sfp_begin_;
  
  natural sampleSize_;			// in bytes
  natural size_;  
  natural csize_;

  natural samplesOut_;
  
  

  void addControls();
  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);
  std::vector<std::string> sndFormats_;
  std::vector<int>    sndFormatSizes_;
  
  natural nChannels_;
  natural samplesToRead_;
  natural samplesToWrite_;
  natural samplesRead_;
  

  real duration_;
  bool advance_;
  natural cindex_;
  
  real repetitions_;
  
 
public:
  AuFileSource(std::string name);
  ~AuFileSource();
  MarSystem* clone() const;    

  void update();
  void getHeader(std::string filename);
  void process(realvec& in,realvec &out);
  

  natural getLinear16(realvec& win);

  
};




#endif    /* !MARSYAS_AUFILESOURCE_H */ 

	
