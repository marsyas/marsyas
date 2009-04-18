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

#include "AuFileSource.h"

using namespace std;
using namespace Marsyas;

#define SND_MAGIC_NUM 0x2e736e64

/* types of .snd files */  
#define SND_FORMAT_UNSPECIFIED 0
#define SND_FORMAT_MULAW_8     1
#define SND_FORMAT_LINEAR_8    2
#define SND_FORMAT_LINEAR_16   3
#define SND_FORMAT_LINEAR_24   4
#define SND_FORMAT_LINEAR_32   5
#define SND_FORMAT_FLOAT       6

AuFileSource::AuFileSource(string name):AbsSoundFileSource("AuFileSource",name)
{
  //type_ = "SoundFileSource";//"AuFileSource"?!?
  //name_ = name;
  
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
  delete [] sdata_;
  delete [] cdata_;
  if (sfp_ != NULL)
    fclose(sfp_);
}

MarSystem* 
AuFileSource::clone() const
{
  return new AuFileSource(*this);
}


AuFileSource::AuFileSource(const AuFileSource& a): AbsSoundFileSource(a) 
{
  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_size_ = getctrl("mrs_natural/size");
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  ctrl_currentLabel_ = getctrl("mrs_natural/currentLabel");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
}




void
AuFileSource::addControls()
{
  addctrl("mrs_bool/notEmpty", true);  
  
  addctrl("mrs_natural/pos", (mrs_natural)0, ctrl_pos_);
  setctrlState("mrs_natural/pos", true);
  
  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/loopPos", true);
  
  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/size", (mrs_natural)0, ctrl_size_);
  addctrl("mrs_string/filetype", "au");

  addctrl("mrs_real/repetitions", 1.0);
  setctrlState("mrs_real/repetitions", true);

  addctrl("mrs_real/duration", -1.0);
  setctrlState("mrs_real/duration", true);

  addctrl("mrs_bool/advance", false);
  setctrlState("mrs_bool/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames", true);
  addctrl("mrs_natural/numFiles", 1);
      
  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
  addctrl("mrs_natural/currentLabel", 0, ctrl_currentLabel_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
  addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
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
    size_t n = fread(&hdr_, sizeof(snd_header), 1, sfp_);  
    if ((n != 1) ||((hdr_.pref[0] != '.') &&(hdr_.pref[1] != 's')))
		{
			MRSWARN("Filename " + filename + " is not correct .au file \n or has settings that are not supported in Marsyas");
			setctrl("mrs_natural/onObservations", (mrs_natural)1);
			setctrl("mrs_real/israte", (mrs_real)22050.0);
			setctrl("mrs_natural/size", (mrs_natural)0);
			notEmpty_ = false;
			setctrl("mrs_bool/notEmpty", false);
		}
    else 
		{ 
		#if defined(MARSYAS_BIGENDIAN)
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
			setctrl("mrs_natural/onObservations", (mrs_natural)hdr_.channels);
		    
			setctrl("mrs_real/israte", (mrs_real)hdr_.srate);
			setctrl("mrs_natural/size", size_);
			ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);
			ctrl_currentLabel_->setValue(0, NOUPDATE);
			ctrl_labelNames_->setValue(",", NOUPDATE);
			ctrl_nLabels_->setValue(0, NOUPDATE);
			setctrl("mrs_bool/notEmpty", true);
			notEmpty_ = true;
			samplesOut_ = 0;
			pos_ = 0;
			setctrl("mrs_natural/pos", 0);
		}
	}
  else
  {
    setctrl("mrs_natural/onObservations", (mrs_natural)1);
    setctrl("mrs_real/israte", (mrs_real)22050.0);
    setctrl("mrs_natural/size", (mrs_natural)0);
    notEmpty_ = false;
    setctrl("mrs_bool/notEmpty", false);
    pos_ = 0;
  }
  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  samplesRead_ = 0;
}

mrs_natural
AuFileSource::getLinear16(realvec& slice)
{
  mrs_natural c = 0;
  fseek(sfp_, 2 * pos_ * nChannels_ + sfp_begin_, SEEK_SET);
  
  samplesRead_ = (mrs_natural)fread(sdata_, sizeof(short), samplesToRead_, sfp_);

  // pad with zeros if necessary 
  if (samplesRead_ != samplesToRead_)
  {
    for (c=0; c < nChannels_; c++)
			for (t=0; t < inSamples_; t++)
				slice(c, t) = 0.0;
    samplesToWrite_ = samplesRead_ / nChannels_;
  }
  else 
    samplesToWrite_ = inSamples_;
  
  // write the read samples to output slice once for each channel 
  for (t=0; t < samplesToWrite_; t++)
  {
    sval_ = 0;
    nt_ = nChannels_ * t;
    
		#if defined(MARSYAS_BIGENDIAN)
    for (c=0; c < nChannels_; c++)
			slice(c, t) = ((mrs_real) sdata_[nt_ + c] / (PCM_FMAXSHRT));
		#else
    for (c=0; c < nChannels_; c++)
		{
			usval_ = sdata_[nt_ + c];
			usval_ = ((usval_ >> 8) | (usval_ << 8));
			sval_ = usval_;
		  
			slice(c, t) = (mrs_real) sval_ / (PCM_FMAXSHRT);
		}
		#endif 
  }
  pos_ += samplesToWrite_;  
  return pos_;
}

void
AuFileSource::myUpdate(MarControlPtr sender)
{
	(void) sender;
  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();  
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
  
  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_natural/onObservations", nChannels_);
  
  setctrl("mrs_real/osrate", israte_);
   
  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();    
  pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();
  rewindpos_ = getctrl("mrs_natural/loopPos")->to<mrs_natural>();
  
  delete [] sdata_;
  delete [] cdata_;
  
  sdata_ = new short[inSamples_ * nChannels_];
  cdata_ = new unsigned char[inSamples_ * nChannels_];   

  repetitions_ = getctrl("mrs_real/repetitions")->to<mrs_real>();

  duration_ = getctrl("mrs_real/duration")->to<mrs_real>();
  advance_ = getctrl("mrs_bool/advance")->to<mrs_bool>();
  cindex_ = getctrl("mrs_natural/cindex")->to<mrs_natural>();
 
  if (duration_ != -1.0)
  {
    csize_ = (mrs_natural)(duration_ * israte_ );
  }
  
  //defaultUpdate(); [!]

	samplesToRead_ = inSamples_ * nChannels_;  
}

void
AuFileSource::myProcess(realvec& in, realvec &out)
{
	(void) in;
  if (ctrl_size_->to<mrs_natural>() != 0)
    {
      //checkFlow(in,out);
      
      switch (hdr_.mode)
	{
	case SND_FORMAT_UNSPECIFIED:
	  {
	    MRSWARN("AuFileSource::Unspecified format");
	    updctrl("mrs_natural/pos", pos_);
	    updctrl("mrs_bool/notEmpty", (pos_ < size_ * nChannels_));
	    break;
	  }
	case SND_FORMAT_MULAW_8:
	  {
	    MRSWARN("MU_LAW for now not supported");
	    updctrl("mrs_natural/pos", pos_);
	    updctrl("mrs_bool/notEmpty", (pos_ < size_ * nChannels_));
	    break;
	  }
	case SND_FORMAT_LINEAR_8:
	  {
	    // pos_ = getLinear8(c, out);
	    setctrl("mrs_natural/pos", pos_);
	    setctrl("mrs_bool/notEmpty", pos_ < size_ * nChannels_);
	    break;
	  }
	case SND_FORMAT_LINEAR_16:
	  {
	    getLinear16(out);
	    ctrl_pos_->setValue(pos_, NOUPDATE);
	    
	    if (pos_ >= rewindpos_ + csize_) 
	      {
			  if (repetitions_ != 1)
				  pos_ = rewindpos_;
	      }
	    samplesOut_ += onSamples_;
		
		if (repetitions_ != 1) 
			notEmpty_ = (samplesOut_ < repetitions_ * csize_);
		else 
			notEmpty_ = pos_ < csize_;
		
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



  
  


