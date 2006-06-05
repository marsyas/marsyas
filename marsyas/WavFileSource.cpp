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
   \class WavFileSource
   \brief SoundFileSource for .wav soundfiles
   
   SoundFileSource reader for .wav sound files.
*/



#include "WavFileSource.h"
using namespace std;







WavFileSource::WavFileSource(string name)
{
  type_ = "SoundFileSource";
  name_ = name;
  

  sdata_ = 0;
  cdata_ = 0;
  sfp_ = 0;
  pos_ = 0;
  addControls();  
}




WavFileSource::~WavFileSource()
{
  delete sdata_;
  delete cdata_;
  if (sfp_ != NULL)
    fclose(sfp_);
}

MarSystem* 
WavFileSource::clone() const
{
  return new WavFileSource(*this);
}


void 
WavFileSource::addControls()
{
  addDefaultControls();
  addctrl("natural/nChannels",(natural)1);
  addctrl("bool/notEmpty", true);  
  addctrl("natural/pos", (natural)0);
  setctrlState("natural/pos", true);
  addctrl("natural/loopPos", (natural)0);
  setctrlState("natural/pos", true);
  addctrl("natural/size", (natural)0);
  addctrl("string/filename", "dwavfile");
  setctrlState("string/filename", true);
  addctrl("string/filetype", "wav");

  addctrl("real/repetitions", 1.0);
  setctrlState("real/repetitions", true);

  addctrl("real/duration", -1.0);
  setctrlState("real/duration", true);

  addctrl("bool/advance", false);
  setctrlState("bool/advance", true);

  addctrl("bool/shuffle", false);
  setctrlState("bool/shuffle", true);

  addctrl("natural/cindex", 0);
  setctrlState("natural/cindex", true);

  addctrl("string/allfilenames", ",");
  setctrlState("string/allfilenames", true);
  addctrl("natural/numFiles", 1);

  addctrl("string/currentlyPlaying", "daufile");
}


  







void 
WavFileSource::getHeader(string filename)
{
  sfp_ = fopen(filename.c_str(), "rb");
  if (sfp_)
    {
      char magic[5];
      
      fseek(sfp_, 8, SEEK_SET); // Locate wave id
      if (fread(magic, 4, 1, sfp_) == 0)
	{
	  MRSERR("WavFileSource: File " + filename + " is empty ");
	}
      magic[4] = '\0';

      if (strcmp(magic, "WAVE"))
	{
	  MRSWARN("Filename " + filename + " is not correct .au file \n or has settings that are not supported in Marsyas");
	  setctrl("natural/nChannels", (natural)1);
	  setctrl("real/israte", (real)22050.0);
	  setctrl("natural/size", (natural)0);
	  notEmpty_ = false;
	  setctrl("bool/notEmpty", (MarControlValue)false);	  
	}
      else
	{
	  char id[5];
	  int chunkSize;
	  fread(id, 4, 1, sfp_);
	  id[4] = '\0';
	  
	  while (strcmp(id, "fmt ")) 
	    {
	      fread(&chunkSize, 4, 1, sfp_);
#if defined(__BIG_ENDIAN__)	      
	      chunkSize = ByteSwapLong(chunkSize);
#else	      
	      chunkSize = chunkSize;
#endif 
	      fseek(sfp_, chunkSize, SEEK_CUR);
	      fread(id, 4, 1, sfp_);
	    }
	  fread(&chunkSize, 4, 1, sfp_);
#if defined(__BIG_ENDIAN__)	      
	      chunkSize = ByteSwapLong(chunkSize);
#else	      
	      chunkSize = chunkSize;
#endif 
	  
	  
	  unsigned short format_tag;
	  fread(&format_tag, 2, 1, sfp_);

#if defined(__BIG_ENDIAN__)	      
	  format_tag = ByteSwapShort(format_tag);
#else	      
	  format_tag = format_tag;
#endif 
	  

	  
	  if (format_tag != 1) 
	    {
	      fclose(sfp_);
	      MRSWARN("Non pcm(compressed) wave files are not supported");
	    }
	  
	  // Get number of channels
	  unsigned short channels;      
	  fread(&channels, 2,1, sfp_);

#if defined(__BIG_ENDIAN__)	      
	  channels = ByteSwapShort(channels);
#else	      
	  channels = channels;
#endif 
	  
	  // access directly controls to avoid update() recursion
	  setctrl("natural/nChannels", (natural)channels);
	  
	  unsigned short srate;
	  fread(&srate, 2,1,sfp_);
	  
#if defined(__BIG_ENDIAN__)	      
	  srate = ByteSwapShort(srate);
#else	      
	  srate = srate;
#endif 

	  setctrl("real/israte", (real)srate);
	  setctrl("real/osrate", (real)srate);
	  
	  fseek(sfp_,8,SEEK_CUR);
	  fread(&bits_, 2, 1, sfp_);

#if defined(__BIG_ENDIAN__)	      
	  bits_ = ByteSwapShort(bits_);
#else	      
	  bits_ = bits_;
#endif 
	  
	  if ((bits_ != 16)&&(bits_ != 8)) 
	    {
  	      MRSWARN("WavFileSource::Only linear 8-bit and 16-bit samples are supported ");
	    }
	  fseek(sfp_, chunkSize - 16, SEEK_CUR);
	  
	  
	  fread(id, 4, 1, sfp_);
	  id[4] = '\0';
	  while (strcmp(id, "data"))
	    {
	      fread(&chunkSize, 4, 1, sfp_);
#if defined(__BIG_ENDIAN__)	      
	      chunkSize = ByteSwapLong(chunkSize);
#else	      
	      chunkSize = chunkSize;
#endif 

	      fseek(sfp_,chunkSize,SEEK_CUR);
	      fread(&id,4,1,sfp_);	  
	    }
	  
	  int bytes;
	  fread(&bytes, 4, 1, sfp_);

#if defined(__BIG_ENDIAN__)	      
	  bytes = ByteSwapLong(bytes);
#else	      
	  bytes = bytes;
#endif 

	  
	  size_ = bytes / (bits_ / 8)/ (getctrl("natural/nChannels").toNatural());
	  csize_ = size_;
	  
	  
	  
	  setctrl("natural/size", size_);
	  sfp_begin_ = ftell(sfp_);
	  notEmpty_ = true;
	  pos_ = 0;
	  samplesOut_ = 0;
	}
    }
  else
    {
      setctrl("natural/nChannels", (natural)1);
      setctrl("real/israte", (real)22050.0);
      setctrl("natural/size", (natural)0);
      notEmpty_ = false;
      setctrl("bool/notEmpty", (MarControlValue)false);      
      pos_ = 0;
    }
  nChannels_ = getctrl("natural/nChannels").toNatural();  
}




void
WavFileSource::update()
{

  nChannels_ = getctrl("natural/nChannels").toNatural();  
  inSamples_ = getctrl("natural/inSamples").toNatural();
  inObservations_ = getctrl("natural/inObservations").toNatural();
  israte_ = getctrl("real/israte").toReal();
  
  nChannels_ = getctrl("natural/nChannels").toNatural();

  setctrl("natural/onSamples", inSamples_);
  setctrl("natural/onObservations", inObservations_);
  setctrl("real/osrate", israte_);
  
  filename_ = getctrl("string/filename").toString();    


  pos_ = getctrl("natural/pos").toNatural();
  rewindpos_ = getctrl("natural/loopPos").toNatural();
  

 
  delete sdata_;
  delete cdata_;
  
  sdata_ = new short[inSamples_ * nChannels_];
  cdata_ = new unsigned char[inSamples_ * nChannels_];   
  
  repetitions_ = getctrl("real/repetitions").toReal();

  duration_ = getctrl("real/duration").toReal();
  advance_ = getctrl("bool/advance").toBool();
  cindex_ = getctrl("natural/cindex").toNatural();
  

  if (duration_ != -1.0)
    {
      csize_ = (natural)(duration_ * israte_);
    }


  defaultUpdate();
  samplesToRead_ = inSamples_ * nChannels_;
}




natural 
WavFileSource::getLinear8(natural c, realvec& slice)
{
  natural nChannels = getctrl("nChannels").toNatural();
  natural inSamples = getctrl("natural/inSamples").toNatural();
  
  
  samplesToRead_ = inSamples * nChannels;

  // only read data when called for first channel
  if (c == 0)				
    {
      samplesRead_ = (natural)fread(cdata_, sizeof(unsigned char), samplesToRead_, sfp_);
    }
  if (samplesRead_ != samplesToRead_)
    {
      for (t=0; t < inSamples; t++)
	{
	  slice(0,t) = 0.0;
	}
    }
  for (t=0; t < inSamples; t++)
    {
      slice(0, t) = (real)-1.0 + (real) cdata_[nChannels * t + c] / 128;
    }

  pos_ += samplesToRead_;
  return pos_;
}



unsigned long 
WavFileSource::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
	  ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}


unsigned short 
WavFileSource::ByteSwapShort (unsigned short nValue)
{
  return (((nValue>> 8)) | (nValue << 8));
}



natural
WavFileSource::getLinear16(realvec& slice)
{
  natural c = 0;
  natural nChannels = getctrl("natural/nChannels").toNatural();
  natural inSamples = getctrl("natural/inSamples").toNatural();

  fseek(sfp_, 2 * pos_ * nChannels + sfp_begin_, SEEK_SET);




  samplesToRead_ = inSamples * nChannels;
  
  samplesRead_ = (natural)fread(sdata_, sizeof(short), samplesToRead_, sfp_);
  
  if (samplesRead_ != samplesToRead_)
    {
      for (t=0; t < inSamples; t++)
	{
	  slice(0, t) = 0.0;
	}
      samplesToWrite_ = samplesRead_ / nChannels;
    }
  else 
    samplesToWrite_ = inSamples_;
  
  
  for (t=0; t < samplesToWrite_; t++)
    {
      sval_ = 0;
      
      
#if defined(__BIG_ENDIAN__)
      slice(0,t) = 0.0;
      
      for (c=0; c < nChannels_; c++)
	{
	  sval_ = ByteSwapShort(sdata_[nChannels_*t + c]);
	  slice(0, t) += (real) sval_ / (FMAXSHRT);
	}
      slice(0,t) /= nChannels_;      
      
#else


      slice(0,t) = 0.0;
      for (c=0; c < nChannels_; c++)
	{
	  sval_ = sdata_[nChannels_ *t + c];
	  slice(0, t) += ((real) sval_ / (FMAXSHRT));
	}
      slice(0,t) /= nChannels_;      
#endif  
    }
  
  pos_ += samplesToWrite_;  
  return pos_;
}





 
void
WavFileSource::process(realvec& in, realvec& out)
{
  
  switch(bits_) 
    {
    case 16: 
      {
	getLinear16(out);
	if (pos_ >= rewindpos_ + csize_) 
	  {
	    if (repetitions_ != 1)
	      pos_ = rewindpos_;
	  }
	samplesOut_ += onSamples_;
	notEmpty_ = samplesOut_ < repetitions_ * csize_;
	    if (repetitions_ == -1) 
	      notEmpty_ = true;
	
	
	break;
      }
    case 8:
      {
	getLinear8(c, in);
	break;
      }
    }
}




  
  


