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

#ifndef MARSYAS_SOUNDFILESOURCE_H
#define MARSYAS_SOUNDFILESOURCE_H

#include "realvec.h"
#include "AuFileSource.h"
#include "WavFileSource.h"
#include "RawFileSource.h"


#ifdef MARSYAS_MAD
#include "MP3FileSource.h"
#endif

#ifdef MARSYAS_VORBIS
#include "OggFileSource.h"
#endif

#include "CollectionFileSource.h"

#include <cstdio>
#include <cmath>

namespace Marsyas
{
/**
   \class SoundFileSource
   \ingroup IO
   \brief Interface for sound file sources 
   
   Abstract base class for any type of sound file input.
   Specific IO classes for various formats like AuFileSource 
   are children of this class. 

   Controls:
   - \b mrs_string/filename [w] : name of the sound file to read
   - \b mrs_bool/notEmpty [r] : is there any data left?
   - \b mrs_natural/pos [rw] : sample position currently read, in samples
   - \b mrs_natural/loopPos [rw] : sample position at which to loop
   - \b mrs_string/allfilenames [w] : a long string containing every sound file to read.
   - \b mrs_natural/numFiles [r] : how many files to read
   - \b mrs_natural/size [r] : number of samples in the current sound file
   - \b mrs_real/frequency [r] : sample rate of the current sound file
   - \b mrs_bool/noteon [rw] : used as a wavetable on receiving a
     noteon.
   - \b mrs_real/repetitions [rw] : number of times to repeat the sound file
   - \b mrs_real/duration [r] : number of seconds in the current sound file
   - \b mrs_bool/advance [w] : go to the next sound file (collection
     only)
   - \b mrs_bool/shuffle [w] : randomize the order of reading multiple sound files
   - \b mrs_natural/cindex [rw] : number of the current sound file in the
     collection
   - \b mrs_string/currentlyPlaying [r] : filename of the current sound file.
   - \b mrs_natural/currentLabel [r] : current label of file played 
   - \b mrs_natural/nLabels [r] : number of labels 
   - \b mrs_string/labelNames [r] : labels (for collections) 
   
*/


class SoundFileSource: public MarSystem
{
private:
	std::string filename_;
  mrs_natural sampleSize_; //in bytes
  mrs_natural samplesRead_;
  mrs_natural samplesToRead_;
  mrs_natural nChannels_;

  MarControlPtr ctrl_pos_;
  MarControlPtr ctrl_loop_;
  MarControlPtr ctrl_notEmpty_;
  MarControlPtr ctrl_mute_;
  MarControlPtr ctrl_advance_; 
  MarControlPtr ctrl_filename_;
  MarControlPtr ctrl_currentlyPlaying_;
  MarControlPtr ctrl_currentLabel_;
  MarControlPtr ctrl_nLabels_;
  MarControlPtr ctrl_labelNames_;
  
  AbsSoundFileSource* src_;

	bool advance_;

	void addControls();
	void myUpdate(MarControlPtr sender);
  
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
