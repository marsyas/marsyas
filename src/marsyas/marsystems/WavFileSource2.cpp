/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "WavFileSource2.h"

using namespace std;
using namespace Marsyas;

WavFileSource2::WavFileSource2(mrs_string name):AbsSoundFileSource2("WavFileSource2",name)
{
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;

  addControls();
}

WavFileSource2::~WavFileSource2()
{
  delete [] sdata_;
  delete [] cdata_;
  if (sfp_ != NULL)
    fclose(sfp_);
}

WavFileSource2::WavFileSource2(const WavFileSource2& a):AbsSoundFileSource2(a) //[!][?]
{
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
}

MarSystem*
WavFileSource2::clone() const
{
  return new WavFileSource2(*this); //shouldn't this class have a copy constructor?!? [?][!]
}

void
WavFileSource2::addControls()
{
  setctrl("mrs_string/filetype", "wav");
}

void
WavFileSource2::hdrError()
{
  setctrl("mrs_natural/nChannels", (mrs_natural)1);
  setctrl("mrs_real/israte", MRS_DEFAULT_SLICE_SRATE);//(mrs_real)22050.0);
  setctrl("mrs_natural/size", (mrs_natural)0);
  setctrl("mrs_bool/hasData", false);
  setctrl("mrs_string/filename", string());
}

bool
WavFileSource2::getHeader()
{
  unsigned short channels, srate;
  mrs_natural size;

  mrs_string filename = getctrl("mrs_string/filename")->to<mrs_string>();
  //if an empty filename, return error and default configuration
  if(filename.empty())
  {
    MRSERR("WavFileSource2::getHeader: empty FileName");
    hdrError();
    return false;
  }

  sfp_ = fopen(filename.c_str(), "rb");
  if (sfp_)
  {
    char magic[5];

    fseek(sfp_, 8, SEEK_SET); // Locate wave id
    if (fread(magic, 4, 1, sfp_) == 0)
    {
      MRSERR("WavFileSource2::getHeader: File " + filename + " is empty ");
      hdrError();
      return false;
    }
    magic[4] = '\0';

    if (strcmp(magic, "WAVE"))
    {
      MRSWARN("WavFileSource2::getHeader: Filename " + filename + " is not correct .wav file \n or has settings that are not supported in Marsyas");
      hdrError();
      return false;
    }
    else
    {
      char id[5];
      int chunkSize;
      if (fread(id, 4, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
      id[4] = '\0';

      while (strcmp(id, "fmt "))
      {
        if (fread(&chunkSize, 4, 1, sfp_) != 1) {
          MRSERR("Error reading wav file");
        }
#if defined(MARSYAS_BIGENDIAN)
        chunkSize = ByteSwapLong(chunkSize);
#endif
        fseek(sfp_, chunkSize, SEEK_CUR);
        if (fread(id, 4, 1, sfp_) != 1) {
          MRSERR("Error reading wav file");
        }
      }

      if (fread(&chunkSize, 4, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      chunkSize = ByteSwapLong(chunkSize);
#endif

      unsigned short format_tag;
      if (fread(&format_tag, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      format_tag = ByteSwapShort(format_tag);
#endif
      if (format_tag != 1)
      {
        fclose(sfp_);
        MRSWARN("WavFileSource2::getHeader: Non pcm(compressed) wave files are not supported");
        hdrError();
        return false;
      }

      // Get number of channels
      if (fread(&channels, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      channels = ByteSwapShort(channels);
#endif
      // access directly controls to avoid update() recursion
      setctrl("mrs_natural/nChannels", (mrs_natural)channels);
      //nChannels_ = channels; //getctrl("mrs_natural/nChannels")->to<mrs_natural>();//[!]

      if (fread(&srate, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      srate = ByteSwapShort(srate);
#endif
      setctrl("mrs_real/israte", (mrs_real)srate);
      //israte_ = (mrs_real)srate;

      fseek(sfp_,8,SEEK_CUR);
      if (fread(&bits_, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      bits_ = ByteSwapShort(bits_);
#endif
      if ((bits_ != 16))//&&(bits_ != 8))
      {
        fclose(sfp_);
        //MRSWARN("WavFileSource2::getHeader: WavFileSource2::Only linear 8-bit and 16-bit samples are supported ");
        MRSWARN("WavFileSource2::getHeader: WavFileSource2::Only linear 16-bit samples are supported for now...");
        hdrError();
        return false;
      }

      fseek(sfp_, chunkSize - 16, SEEK_CUR);
      if (fread(id, 4, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
      id[4] = '\0';
      while (strcmp(id, "data"))
      {
        if (fread(&chunkSize, 4, 1, sfp_) != 1) {
          MRSERR("Error reading wav file");
        }
#if defined(MARSYAS_BIGENDIAN)
        chunkSize = ByteSwapLong(chunkSize);
#endif
        fseek(sfp_,chunkSize,SEEK_CUR);
        if (fread(&id,4,1,sfp_) != 1) {
          MRSERR("Error reading wav file");
        }
      }

      int bytes;
      if (fread(&bytes, 4, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }
#if defined(MARSYAS_BIGENDIAN)
      bytes = ByteSwapLong(bytes);
#endif
      //size in number of samples per channel
      size = bytes / (bits_ / 8)/ channels;
      setctrl("mrs_natural/size", size);

      sfp_begin_ = ftell(sfp_);

      //check if there is in fact any audio data in the soundfile
      if(size > 0)
        setctrl("mrs_bool/hasData", true);
      else
        setctrl("mrs_bool/hasData", false);
    }
  }
  else
  {
    hdrError();
    return false;
  }

  return true;
}

void
WavFileSource2::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  //if not a new audiofile, no need to read header again
  mrs_string filename = getctrl("mrs_string/filename")->to<mrs_string>();
  if(filename_ != filename)
  {
    getHeader();//sets controls filename, nChannels, israte and size
    filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
    nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
    israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
    size_ = getctrl("mrs_natural/size")->to<mrs_natural>();
  }

  //update internal vars
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  osrate_ = israte_;

  //update output pin config
  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_natural/onObservations", nChannels_);
  setctrl("mrs_real/osrate", osrate_);

  //update internal buffers
  delete [] sdata_;
  delete [] cdata_;
  sdata_ = new short[inSamples_ * nChannels_];
  cdata_ = new unsigned char[inSamples_ * nChannels_];

  samplesToRead_ = inSamples_ * nChannels_;

  //observation's names
  ostringstream oss;
  for (mrs_natural i = 0; i < nChannels_; ++i)
    oss << "audio_ch_" << i+1 << ",";
  setctrl("mrs_string/onObsNames", oss.str());
}

// mrs_natural
// WavFileSource2::getLinear8(mrs_natural c, realvec& slice)//this does not seem to be working!!! [!][?]
// {
// 	mrs_natural nChannels = getctrl("nChannels")->to<mrs_natural>();
// 	mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
//
// 	samplesToRead_ = inSamples * nChannels;
//
// 	// only read data when called for first channel
// 	if (c == 0)
// 	{
// 		samplesRead_ = (mrs_natural)fread(cdata_, sizeof(unsigned char), samplesToRead_, sfp_);
// 	}
// 	if (samplesRead_ != samplesToRead_)
// 	{
// 		for (t=0; t < inSamples; t++)
// 		{
// 			slice(0,t) = 0.0;
// 		}
// 	}
// 	for (t=0; t < inSamples; t++)
// 	{
// 		slice(0, t) = (mrs_real)-1.0 + (mrs_real) cdata_[nChannels * t + c] / 128;
// 	}
//
// 	pos_ += samplesToRead_;
// 	return pos_;
// }

unsigned long
WavFileSource2::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
          ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

unsigned short
WavFileSource2::ByteSwapShort (unsigned short nValue)
{
  return (static_cast<unsigned short>((nValue & 0xff00) >> 8) |
          static_cast<unsigned short>((nValue & 0xff) << 8));
}

mrs_natural
WavFileSource2::getLinear16(realvec& slice)
{
  mrs_natural c,t;
  mrs_natural pos = getctrl("mrs_natural/pos")->to<mrs_natural>();

  fseek(sfp_, 2 * pos * nChannels_ + sfp_begin_, SEEK_SET);

  samplesToRead_ = inSamples_ * nChannels_;

  samplesRead_ = (mrs_natural)fread(sdata_, sizeof(short), samplesToRead_, sfp_);

  if (samplesRead_ != samplesToRead_)
  {
    samplesToWrite_ = samplesRead_ / nChannels_;
    for (c=0; c < nChannels_; ++c)
      //only fill remaining space with zeros => faster
      for(t = samplesToWrite_; t < inSamples_; t++)
      {
        slice(c, t) = 0.0;
      }
  }
  else
    samplesToWrite_ = inSamples_;

  for (t=0; t < samplesToWrite_; t++)
  {
    sval_ = 0;
#if defined(MARSYAS_BIGENDIAN)
    for (c=0; c < nChannels_; ++c)
    {
      sval_ = ByteSwapShort(sdata_[nChannels_*t + c]);
      slice(c, t) = (mrs_real) sval_ / (PCM_FMAXSHRT);
    }
#else
    for (c=0; c < nChannels_; ++c)
    {
      sval_ = sdata_[nChannels_ *t + c];
      slice(c, t) = ((mrs_real) sval_ / (PCM_FMAXSHRT));
    }
#endif
  }

  //update play position
  pos += samplesToWrite_;
  setctrl("mrs_natural/pos", pos);
  return pos;
}

realvec&
WavFileSource2::getAudioRegion(mrs_natural startSample, mrs_natural endSample)
{
  (void) startSample; // FIXME Unused parameters
  (void) endSample;

  //fill audioRegion_ with corresponding audio data!
  // ...
  // ...

  return audioRegion_;
}

void
WavFileSource2::myProcess(realvec& in, realvec& out)
{
  (void) in;
  //in case of problems opening the .wav file,
  //or no audiodata available to read, just send silence
  if(!getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    out.setval(0.0);
    return;
  }

  switch(bits_)
  {
  case 16:
  {
    //read audio samples from .wav file
    getLinear16(out);
    break;
  }
  case 8:
  {
    //getLinear8(out); [!]
    break;
  }
  }

  //if reached end of file, signal it!
  if(getctrl("mrs_natural/pos")->to<mrs_natural>() >= size_)
    setctrl("mrs_bool/hasData", false);
}

















