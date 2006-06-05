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


#include "AuFileSource.h"
using namespace std;



AuFileSource::AuFileSource(string name)
{
  type_ = "SoundFileSource";
  name_ = name;
  sdata_ = 0;
  cdata_ = 0;
  sfp_ = 0;
  pos_ =0;
  
  sndFormats_.push_back("Unspecified format");
  sndFormatSizes_.push_back(0);
  sndFormats_.push_back("Mulaw 8-bit");
  sndFormatSizes_.push_back(1);
  sndFormats_.push_back("Linear 8-bit");
  sndFormatSizes_.push_back(1);
  sndFormats_.push_back("Linear 16-bit");
  sndFormatSizes_.push_back(2);
  sndFormats_.push_back("Linear 24-bit");
  sndFormatSizes_.push_back(3);
  sndFormats_.push_back("Linear 32-bit");
  sndFormatSizes_.push_back(4);
  sndFormats_.push_back("Float");
  sndFormatSizes_.push_back(4);
  sndFormats_.push_back("Double");
  sndFormatSizes_.push_back(8);

  addControls();
}



AuFileSource::~AuFileSource()
{

  delete sdata_;
  delete cdata_;
  if (sfp_ != NULL)
    fclose(sfp_);
}


MarSystem* 
AuFileSource::clone() const
{
  return new AuFileSource(*this);
}



void
AuFileSource::addControls()
{
  addDefaultControls();
  addctrl("natural/nChannels",(natural)1);
  addctrl("bool/notEmpty", true);  
  addctrl("natural/pos", (natural)0);
  setctrlState("natural/pos", true);
  addctrl("natural/loopPos", (natural)0);
  setctrlState("natural/pos", true);
  addctrl("string/filename", "daufile");
  setctrlState("string/filename", true);
  addctrl("natural/size", (natural)0);
  addctrl("string/filetype", "au");

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



unsigned long 
AuFileSource::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
	  ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}
 


unsigned short 
AuFileSource::ByteSwapShort (unsigned short nValue)
{
  return (((nValue>> 8)) | (nValue << 8));
}



void 
AuFileSource::getHeader(string filename)
{
  sfp_ = fopen(filename.c_str(), "rb");
  if (sfp_)
    {
      int n = fread(&hdr_, sizeof(snd_header), 1, sfp_);  
      if ((n != 1) ||
	  ((hdr_.pref[0] != '.') &&
	   (hdr_.pref[1] != 's')))
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
	  
	  
#if defined(__BIG_ENDIAN__)
	  
	  hdr_.hdrLength = hdr_.hdrLength;
	  hdr_.comment[hdr_.hdrLength-24] = '\0';
	  hdr_.srate = hdr_.srate;
	  hdr_.channels = hdr_.channels;
	  hdr_.mode = hdr_.mode;
	  hdr_.fileLength = hdr_.fileLength;
#else
	  hdr_.hdrLength = ByteSwapLong(hdr_.hdrLength);
	  hdr_.comment[hdr_.hdrLength-24] = '\0';
	  hdr_.srate = ByteSwapLong(hdr_.srate);
	  hdr_.channels = ByteSwapLong(hdr_.channels);
	  hdr_.mode = ByteSwapLong(hdr_.mode);
	  hdr_.fileLength = ByteSwapLong(hdr_.fileLength);
#endif 
	  
	  
	  
	  sampleSize_ = 2;
	  
	  size_ = (hdr_.fileLength) / sndFormatSizes_[hdr_.mode] / hdr_.channels;
	  // csize_ = size_ * hdr_.channels;
	  csize_ = size_;

	  fseek(sfp_, hdr_.hdrLength, 0);
	  sfp_begin_ = ftell(sfp_);
	  setctrl("natural/nChannels", hdr_.channels);
	  setctrl("real/israte", (real)hdr_.srate);
	  setctrl("natural/size", size_);
	  setctrl("bool/notEmpty", (MarControlValue)true);
	  notEmpty_ = true;
	  samplesOut_ = 0;
	  pos_ = 0;
	  setctrl("natural/pos", (MarControlValue)0);
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
  samplesRead_ = 0;
  
  
  
}




natural
AuFileSource::getLinear16(realvec& slice)
{
  natural c = 0;
  fseek(sfp_, 2 * pos_ * nChannels_ + sfp_begin_, SEEK_SET);
  


  samplesRead_ = fread(sdata_, sizeof(short), samplesToRead_, sfp_);

  
  // pad with zeros if necessary 
  if (samplesRead_ != samplesToRead_)
    {
      for (t=0; t < inSamples_; t++)
	slice(0, t) = 0.0;
      samplesToWrite_ = samplesRead_ / nChannels_;
    }
  else 
    samplesToWrite_ = inSamples_;
  
  // write the read samples to output slice once for each channel 
  
  for (t=0; t < samplesToWrite_; t++)
    {
      sval_ = 0;
      
#if defined(__BIG_ENDIAN__)
      slice(0,t) = 0.0;
      for (c=0; c < nChannels_; c++)
	slice(0, t) += ((real) sdata_[nChannels_*t + c] / (FMAXSHRT));
      slice(0,t) /= nChannels_;      
      
#else
      
      slice(0,t) = 0.0;
      
      for (c=0; c < nChannels_; c++)
	{
	  usval_ = sdata_[nChannels_*t + c];
	  usval_ = ((usval_ >> 8) | (usval_ << 8));
	  sval_ = usval_;
	  
	  slice(0, t) += (real) sval_ / (FMAXSHRT);
	}
      slice(0,t) /= nChannels_;      
      
#endif 


    }

  pos_ += samplesToWrite_;  
  return pos_;
}


void
AuFileSource::update()
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
      csize_ = (natural)(duration_ * israte_ );
    }
  
  
  defaultUpdate();
  samplesToRead_ = inSamples_ * nChannels_;  

  
}




void
AuFileSource::process(realvec& in, realvec &out)
{
  
  
  if (getctrl("natural/size").toNatural() != 0)
    {
  
      checkFlow(in,out);
      
      switch (hdr_.mode)
	{
	case SND_FORMAT_UNSPECIFIED:
	  {
	    MRSWARN("AuFileSource::Unspecified format");
	    updctrl("natural/pos", pos_);
	    updctrl("bool/notEmpty", (pos_ < size_ * nChannels_));

	    
	    break;
	  }
	case SND_FORMAT_MULAW_8:
	  {
	    MRSWARN("MU_LAW for now not supported");
	    updctrl("natural/pos", pos_);
	    updctrl("bool/notEmpty", (pos_ < size_ * nChannels_));
	    break;
	  }
	case SND_FORMAT_LINEAR_8:
	  {
	    // pos_ = getLinear8(c, out);
	    setctrl("natural/pos", pos_);
	    setctrl("bool/notEmpty", (MarControlValue)(pos_ < size_ * nChannels_));
	    break;
	  }
	case SND_FORMAT_LINEAR_16:
	  {
	    getLinear16(out);
	    setctrl("natural/pos", pos_);	    
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
	case SND_FORMAT_FLOAT:
	  {
	    // getfloat(win);
	    break;
	  }
	default:
	  {
	    string warn = "File mode";
	    warn += sndFormats_[hdr_.mode];
	    warn += "(";
	    warn += hdr_.mode;
	    warn += ") is not supported for now";
	    MRSWARN(warn);
	  }
	}
      
      
    }
}



  
  


