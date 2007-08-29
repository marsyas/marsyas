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

#include "WavFileSource.h"

using namespace std;
using namespace Marsyas;

WavFileSource::WavFileSource(string name):AbsSoundFileSource("WavFileSource",name)
{
  sdata_ = 0;
  cdata_ = 0;
  sfp_ = 0;
  pos_ = 0;

  addControls();
}


WavFileSource::WavFileSource(const WavFileSource& a): AbsSoundFileSource(a) 
{
  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  
}



WavFileSource::~WavFileSource()
{
  delete [] sdata_;
  delete [] cdata_;
  if (sfp_ != NULL)
    fclose(sfp_);
}

MarSystem* 
WavFileSource::clone() const
{
  return new WavFileSource(*this); //shouldn't this class have a copy constructor?!? [?][!]
}

void 
WavFileSource::addControls()
{
  //addctrl("mrs_natural/nChannels",(mrs_natural)1);
  addctrl("mrs_bool/notEmpty", true);  
  
	addctrl("mrs_natural/pos", (mrs_natural)0, ctrl_pos_);
  setctrlState("mrs_natural/pos", true);
  
	addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);
  
	addctrl("mrs_natural/size", (mrs_natural)0);
  
	addctrl("mrs_string/filename", "dwavfile");
  setctrlState("mrs_string/filename", true);
  
	addctrl("mrs_string/filetype", "wav");

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
  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_); //"dwavfile" [?]
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
			MRSWARN("Filename " + filename + " is not correct .wav file \n or has settings that are not supported in Marsyas");
			// setctrl("mrs_natural/nChannels", 1);
			setctrl("mrs_real/israte", 22050.0);
			setctrl("mrs_natural/size", 0);
			notEmpty_ = false;
			setctrl("mrs_bool/notEmpty", false);	  
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
				#if defined(MARSYAS_BIGENDIAN)	      
				chunkSize = ByteSwapLong(chunkSize);
				#else	      
				chunkSize = chunkSize;
				#endif 
				fseek(sfp_, chunkSize, SEEK_CUR);
				fread(id, 4, 1, sfp_);
			}
			
			fread(&chunkSize, 4, 1, sfp_);
			#if defined(MARSYAS_BIGENDIAN)	      
			chunkSize = ByteSwapLong(chunkSize);
			#else	      
			chunkSize = chunkSize;//[?]
			#endif 
	  
			unsigned short format_tag;
			fread(&format_tag, 2, 1, sfp_);

			#if defined(MARSYAS_BIGENDIAN)	      
			format_tag = ByteSwapShort(format_tag);
			#else	      
			format_tag = format_tag;//[?]
			#endif 
		  			  
			if (format_tag != 1) 
			{
				fclose(sfp_);
				MRSWARN("Non pcm(compressed) wave files are not supported");
			}
		  
			// Get number of channels
			unsigned short channels;      
			fread(&channels, 2,1, sfp_);

			#if defined(MARSYAS_BIGENDIAN)	      
			channels = ByteSwapShort(channels);
			#else	      
			channels = channels; //[?]
			#endif 
		  
			// access directly controls to avoid update() recursion
			// setctrl("mrs_natural/nChannels", (mrs_natural)channels);
			setctrl("mrs_natural/onObservations", (mrs_natural)channels);

			unsigned short srate;
			fread(&srate, 2,1,sfp_);
		  
			#if defined(MARSYAS_BIGENDIAN)	      
			srate = ByteSwapShort(srate);
			#else	      
			srate = srate; //[?]
			#endif 

			setctrl("mrs_real/israte", (mrs_real)srate);
			setctrl("mrs_real/osrate", (mrs_real)srate);
		  
			fseek(sfp_,8,SEEK_CUR);
			fread(&bits_, 2, 1, sfp_);

			#if defined(MARSYAS_BIGENDIAN)	      
			bits_ = ByteSwapShort(bits_);
			#else	      
			bits_ = bits_;//[?]
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
				#if defined(MARSYAS_BIGENDIAN)	      
				chunkSize = ByteSwapLong(chunkSize);
				#else	      
				chunkSize = chunkSize;
				#endif 

				fseek(sfp_,chunkSize,SEEK_CUR);
				fread(&id,4,1,sfp_);	  
			}
		  
			int bytes;
			fread(&bytes, 4, 1, sfp_);

			#if defined(MARSYAS_BIGENDIAN)	      
			bytes = ByteSwapLong(bytes);
			#else	      
			bytes = bytes;//[?]
			#endif 
		  
			//size in number of samples per channel
			size_ = bytes / (bits_ / 8)/ (getctrl("mrs_natural/onObservations")->to<mrs_natural>());
			csize_ = size_;
			setctrl("mrs_natural/size", size_);
			ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);

			sfp_begin_ = ftell(sfp_);
			notEmpty_ = true;
			pos_ = 0;
			samplesOut_ = 0;
		}
	}
  else
  {
    // setctrl("mrs_natural/nChannels", 1);
    setctrl("mrs_real/israte", 22050.0);
    setctrl("mrs_natural/onObservations", 1);
    setctrl("mrs_natural/size", 0);
    notEmpty_ = false;
    setctrl("mrs_bool/notEmpty", false);      
    pos_ = 0;
  }
  
  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();  
}

void
WavFileSource::myUpdate(MarControlPtr sender)
{
	(void) sender;
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
	osrate_ = getctrl("mrs_real/osrate")->to<mrs_real>();
  
  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_natural/onObservations", nChannels_);
  //setctrl("mrs_real/osrate", osrate_);
  //osrate_ = getctrl("mrs_real/osrate")->to<mrs_real>();
  pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();
  rewindpos_ = getctrl("mrs_natural/loopPos")->to<mrs_natural>();
  
  delete [] sdata_;
  delete [] cdata_;
  
  sdata_ = new short[inSamples_ * nChannels_];
  cdata_ = new unsigned char[inSamples_ * nChannels_];   
  
  repetitions_ = getctrl("mrs_real/repetitions")->to<mrs_real>();
  duration_ = getctrl("mrs_real/duration")->to<mrs_real>();

  if (duration_ != -1.0)
    {
      csize_ = (mrs_natural)(duration_ * israte_);
    }

	samplesToRead_ = inSamples_ * nChannels_;
}

mrs_natural 
WavFileSource::getLinear8(mrs_natural c, realvec& slice)
{
  mrs_natural nChannels = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  
  samplesToRead_ = inSamples * nChannels;

  // only read data when called for first channel
  if (c == 0)				
    {
      samplesRead_ = (mrs_natural)fread(cdata_, sizeof(unsigned char), samplesToRead_, sfp_);
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
      slice(0, t) = (mrs_real)-1.0 + (mrs_real) cdata_[nChannels * t + c] / 128;
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

mrs_natural
WavFileSource::getLinear16(realvec& slice)
{
  mrs_natural c = 0;

  fseek(sfp_, 2 * pos_ * nChannels_ + sfp_begin_, SEEK_SET);

  samplesToRead_ = inSamples_ * nChannels_;
  
  samplesRead_ = (mrs_natural)fread(sdata_, sizeof(short), samplesToRead_, sfp_);
  
  if (samplesRead_ != samplesToRead_)
    {
      for (c=0; c < nChannels_; c++)
				for (t=0; t < inSamples_; t++)
					{
						slice(c, t) = 0.0;
					}
      samplesToWrite_ = samplesRead_ / nChannels_;
    }
  else 
    samplesToWrite_ = inSamples_;
  
  for (t=0; t < samplesToWrite_; t++)
    {
      sval_ = 0;
			#if defined(MARSYAS_BIGENDIAN)
      for (c=0; c < nChannels_; c++)
			{
				sval_ = ByteSwapShort(sdata_[nChannels_*t + c]);
				slice(c, t) = (mrs_real) sval_ / (PCM_FMAXSHRT);
			}
      #else
      for (c=0; c < nChannels_; c++)
			{
				sval_ = sdata_[nChannels_ *t + c];
				slice(c, t) = ((mrs_real) sval_ / (PCM_FMAXSHRT));
			}
			#endif  
    }
  
  pos_ += samplesToWrite_;  
  return pos_;
}
 
void
WavFileSource::myProcess(realvec& in, realvec& out)
{
  switch(bits_) 
  {
    case 16: 
    {
			getLinear16(out);
			ctrl_pos_->setValue(pos_, NOUPDATE);
			
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




  
  


