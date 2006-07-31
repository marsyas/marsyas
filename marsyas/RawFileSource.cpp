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
   \class RawFileSource
   \brief SoundFileSource for .raw wavetable files
   
   STK rawwave files have no header and are assumed to contain a
   monophonic stream of 16-bit signed integers in big-endian byte
   order with a sample rate of 22050 Hz.
   
*/

#include "RawFileSource.h"

using namespace std;
using namespace Marsyas;

RawFileSource::RawFileSource(string name):AbsSoundFileSource("RawFileSource", name)
{
	sfp_ = NULL;
	buffer_ = NULL;
  
	phaseOffset_ = 0.0;
  bufferSize_ = 0;
  time_ = 0.0;

	notEmpty_ = true;

	addControls();
}

RawFileSource::~RawFileSource() 
{
  if (sfp_ != NULL)
    fclose(sfp_);

  delete [] buffer_;
  
}

MarSystem* RawFileSource::clone() const
{
  return new RawFileSource(*this);//[?] copy constructor?!?
}

void
RawFileSource::addControls() 
{
  addctrl("mrs_natural/nChannels",1);
  addctrl("mrs_real/frequency",440.0);
  setctrlState("mrs_real/frequency",true);
  addctrl("mrs_natural/size", 0);
  addctrl("mrs_natural/pos", 0);
  setctrlState("mrs_natural/pos", true);
  addctrl("mrs_string/filename", "defaultfile.au");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_bool/notEmpty", true);
  addctrl("mrs_bool/noteon", false);
  setctrlState("mrs_bool/noteon", true);
  addctrl("mrs_string/filetype", "raw");
}	


void RawFileSource::openFile(string filename) 
{
  getHeader(filename);
  rate_ = fileSize_ * getctrl("mrs_real/frequency").toReal() / getctrl("mrs_real/israte").toReal();
}


// get file data info from the raw file
bool RawFileSource::getRawInfo( const char *fileName )
{
  // Use the system call "stat" to determine the file length.
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    return false;
  }
  
  // length in 2-byte samples
  fileSize_ = (mrs_natural) filestat.st_size / 2;  
  bufferSize_ = fileSize_;
  
  // assume little endian for now
  byteSwap_ = true;				
  
  return true;
}

//
// STK raw files don't have a header, so just open the file and get info
//
void RawFileSource::getHeader(string fileName)
{
  
  sfp_ = fopen(fileName.c_str(), "raw");
  if (sfp_ == NULL) {
    MRSWARN("Could not open file: " + fileName);
    return;
  } 
  
  bool result = getRawInfo( fileName.c_str() );
  if ( ! result ) {
    MRSWARN("Could not get raw data information from file: " + fileName);
    return;
  }
  
  // allocate storage for the buffer
  mrs_natural samples = (bufferSize_+1)* getctrl("mrs_natural/nChannels").toNatural();
  data_.create(samples);

	if(buffer_)
		delete [] buffer_;
  buffer_ = new short[bufferSize_];
  
  // now read the data into our buffer (data_[]).
  readData(0);
}


void RawFileSource::swap16(unsigned char *ptr)
{
  register unsigned char val;
  
  // Swap 1st and 2nd bytes
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}


//
// read all the raw data into data[] 
//
void RawFileSource::readData(unsigned long index)//[!]
{
  mrs_natural i;
  mrs_natural length = bufferSize_;
  
  // Read samples into data[].  
  if (fseek(sfp_, index, SEEK_SET) == -1) {
    MRSWARN("ERROR(fseek): could not read raw file data.");
    return;
  }
  
  if (int x = fread(buffer_, length, 2, sfp_) != 2 ) {
    MRSWARN("ERROR(fread): could not read raw file data");
    return;
  }
  
  // if we are on a little endian machine.
  byteSwap_ = true;
  if ( byteSwap_ ) {
    short* ptr = buffer_;
    for ( i=length; i>=0; i-- )       
      swap16((unsigned char *)(ptr++));
  }
  
  for ( i=length-1; i>=0; i-- ) {
    data_(i) = buffer_[i];
  }
  
  // repeat last sample frame for interpolation 
  data_(length) = data_(length-1);
  
  // find the peak
  mrs_real max = 0.0;
  for (i=0; i < length; i++) {
    if (fabs(data_(i)) > max)
      max = (mrs_real) fabs((double) data_(i));
  }
  
  // now normalize according to the peak.
  if (max > 0.0) {
    max = (mrs_real)(1.0 / max);
    max *= 1.0;					// constant factor for now.				
    for ( i=0; i <= length; i++ )  {
      data_(i) *= max;
    }
  }
  
}

void RawFileSource::localUpdate() 
{
 
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();  
  inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
  inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
  israte_ = getctrl("mrs_real/israte").toReal();
  
  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_natural/onObservations", inObservations_);
  setctrl("mrs_real/osrate", israte_);
  
  filename_ = getctrl("mrs_string/filename").toString();    
  pos_ = getctrl("mrs_natural/pos").toNatural();

  rate_ = fileSize_ * getctrl("mrs_real/frequency").toReal() / israte_;
}	


void RawFileSource::process(realvec& in,realvec &out)
{
  checkFlow(in,out);

  mrs_real alpha;
  mrs_natural i, index;
  
  if (getctrl("mrs_bool/noteon").toBool() == false) {
    return;
  }
  
  for (i = 0; i < inSamples_; i++ ) {
    
    // loop back to start of the wavetable
    if (time_ >= fileSize_) {
      time_ -= fileSize_;
    }
    
    // linear interpolation
    index = (mrs_natural) time_;
    alpha = time_ - (mrs_real) index;			// fractional part of time address
    out(0,i) = data_(index);
    out(0,i) += (alpha * (data_(index+1) - data_(index)));
    
    time_ += rate_;
    
  }	
}






