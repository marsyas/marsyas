/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "../common_source.h"







#include "AuFileSource.h"


#ifdef MARSYAS_WIN32
#ifndef MARSYAS_CYGWIN
typedef __int32 int32_t;
#endif
#else
#include <stdint.h>
#endif

#include <cstddef>

using std::size_t;

/********  NeXT/Sun Soundfile Header Struct   *******/
struct Marsyas::snd_header
{
  char pref[4];
  int32_t hdrLength;
  int32_t fileLength;
  int32_t mode;
  int32_t srate;
  int32_t channels;
  char comment[1024];
};



/* Array containing descriptions of
   the various formats for the samples
   of the Next .snd/ Sun .au format */


using std::ostringstream;
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

AuFileSource::AuFileSource(mrs_string name):AbsSoundFileSource("AuFileSource",name)
{
  //type_ = "SoundFileSource";//"AuFileSource"?!?
  //name_ = name

  sdata_ = 0;
  cdata_ = 0;
  sfp_ = 0;
  pos_ =0;
  hdr_ = new snd_header;
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
  delete hdr_;

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
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_currentHasData_ = getctrl("mrs_bool/currentHasData");
  hdr_ = new snd_header;

}




void
AuFileSource::addControls()
{
  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_bool/lastTickWithData", false);

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

  addctrl("mrs_natural/advance", 0);
  setctrlState("mrs_natural/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames", true);
  addctrl("mrs_natural/numFiles", 1);

  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
  addctrl("mrs_string/previouslyPlaying", "daufile", ctrl_previouslyPlaying_);
  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
  addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
  addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);

  addctrl("mrs_bool/currentHasData", true, ctrl_currentHasData_);
  addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);
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
  return (static_cast<unsigned short>((nValue & 0xff00) >> 8) |
          static_cast<unsigned short>((nValue & 0xff) << 8));
}

void
AuFileSource::getHeader(mrs_string filename)
{
  if (sfp_ != NULL)
    fclose(sfp_);
  sfp_ = fopen(filename.c_str(), "rb");
  if (sfp_)
  {
    size_t n = fread(hdr_, sizeof(snd_header), 1, sfp_);
    if ((n != 1) ||((hdr_->pref[0] != '.') &&(hdr_->pref[1] != 's')))
    {
      MRSWARN("Filename " + filename + " is not correct .au file \n or has settings that are not supported in Marsyas");
      setctrl("mrs_natural/onObservations", (mrs_natural)1);
      setctrl("mrs_real/israte", (mrs_real)22050.0);
      setctrl("mrs_natural/size", (mrs_natural)0);
      hasData_ = false;
      lastTickWithData_ = true;
      setctrl("mrs_bool/hasData", false);
      setctrl("mrs_bool/lastTickWithData", true);
    }
    else
    {
#if defined(MARSYAS_BIGENDIAN)
      hdr_->hdrLength = hdr_->hdrLength;
      hdr_->comment[hdr_->hdrLength-24] = '\0';
      hdr_->srate = hdr_->srate;
      hdr_->channels = hdr_->channels;
      hdr_->mode = hdr_->mode;
      hdr_->fileLength = hdr_->fileLength;
#else
      hdr_->hdrLength = ByteSwapLong(hdr_->hdrLength);
      hdr_->comment[hdr_->hdrLength-24] = '\0';
      hdr_->srate = ByteSwapLong(hdr_->srate);
      hdr_->channels = ByteSwapLong(hdr_->channels);
      hdr_->mode = ByteSwapLong(hdr_->mode);
      hdr_->fileLength = ByteSwapLong(hdr_->fileLength);
#endif

      sampleSize_ = 2;
      size_ = (hdr_->fileLength) / sndFormatSizes_[hdr_->mode] / hdr_->channels;
      // csize_ = size_ * hdr_->channels;
      csize_ = size_;

      fseek(sfp_, hdr_->hdrLength, 0);
      sfp_begin_ = ftell(sfp_);
      setctrl("mrs_natural/onObservations", (mrs_natural)hdr_->channels);

      setctrl("mrs_real/israte", (mrs_real)hdr_->srate);
      setctrl("mrs_natural/size", size_);
      ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);
      ctrl_previouslyPlaying_->setValue(filename, NOUPDATE);
      ctrl_currentLabel_->setValue(0.0, NOUPDATE);
      ctrl_previousLabel_->setValue(0.0, NOUPDATE);
      ctrl_labelNames_->setValue(",", NOUPDATE);
      ctrl_nLabels_->setValue(0, NOUPDATE);
      setctrl("mrs_bool/hasData", true);
      hasData_ = true;
      lastTickWithData_ = false;
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
    hasData_ = false;
    setctrl("mrs_bool/hasData", false);
    lastTickWithData_ = true;
    setctrl("mrs_bool/lastTickWithData", true);
    pos_ = 0;
  }
  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  samplesRead_ = 0;
}

mrs_natural
AuFileSource::getLinear16(realvec& slice)
{
  mrs_natural c,t =0;

  // read samples
  fseek(sfp_, 2 * pos_ * nChannels_ + sfp_begin_, SEEK_SET);
  samplesRead_ = (mrs_natural)fread(sdata_, sizeof(short), samplesToRead_, sfp_);


  // pad with zeros if necessary
  if ((samplesRead_ != samplesToRead_)&&(samplesRead_ != 0))
  {
    for (c=0; c < nChannels_; ++c)
      for (t=0; t < inSamples_; t++)
        slice(c, t) = 0.0;
    samplesToWrite_ = samplesRead_ / nChannels_;
  }
  else // default case - read enough samples or no samples in which case zero output
  {
    samplesToWrite_ = inSamples_;

    // if there are no more samples output zeros
    if (samplesRead_ == 0)
      for (t=0; t < inSamples_; t++)
      {
        nt_ = nChannels_ * t;
        for (c=0; c < nChannels_; ++c)
        {
          sdata_[nt_ + c] = 0;
        }
      }
  }

  // write the read samples to output slice once for each channel
  for (t=0; t < samplesToWrite_; t++)
  {
    sval_ = 0;
    nt_ = nChannels_ * t;

#if defined(MARSYAS_BIGENDIAN)
    for (c=0; c < nChannels_; ++c)
      slice(c, t) = ((mrs_real) sdata_[nt_ + c] / (PCM_FMAXSHRT));
#else
    for (c=0; c < nChannels_; ++c)
    {
      usval_ = sdata_[nt_ + c];
      usval_ = ByteSwapShort (usval_);
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
  (void) sender;  //suppress warning of unused parameter(s)
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
  advance_ = getctrl("mrs_natural/advance")->to<mrs_natural>();
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

    switch (hdr_->mode)
    {
    case SND_FORMAT_UNSPECIFIED:
    {
      MRSWARN("AuFileSource::Unspecified format");
      updControl("mrs_natural/pos", pos_);
      updControl("mrs_bool/hasData", (pos_ < size_ * nChannels_));
      break;
    }
    case SND_FORMAT_MULAW_8:
    {
      MRSWARN("MU_LAW for now not supported");
      updControl("mrs_natural/pos", pos_);
      updControl("mrs_bool/hasData", (pos_ < size_ * nChannels_));
      break;
    }
    case SND_FORMAT_LINEAR_8:
    {
      // pos_ = getLinear8(c, out);
      setctrl("mrs_natural/pos", pos_);
      setctrl("mrs_bool/hasData", pos_ < size_ * nChannels_);
      break;
    }
    case SND_FORMAT_LINEAR_16:
    {
      getLinear16(out);
      ctrl_pos_->setValue(pos_, NOUPDATE);

      if (pos_ >= rewindpos_ + csize_)
      {
        if (repetitions_ != 1)
        {
          pos_ = rewindpos_;
        }

      }

      samplesOut_ += onSamples_;

      if (repetitions_ != 1)
      {
        hasData_ = (samplesOut_ < repetitions_ * csize_);
        lastTickWithData_ = ((samplesOut_  + onSamples_>= repetitions_ * csize_) && hasData_);
      }
      else
      {
        hasData_ = pos_ < rewindpos_ + csize_;
        lastTickWithData_ = ((samplesOut_  + onSamples_>= repetitions_ * csize_) && hasData_);
      }


      if (repetitions_ == -1)
      {
        hasData_ = true;
        lastTickWithData_ = false;
      }

      break;
    }
    case SND_FORMAT_FLOAT:
    {
      // getfloat(win);
      break;
    }
    default:
    {
      mrs_string warn = "File mode";
      warn += sndFormats_[hdr_->mode];
      warn += "(";
      warn += (char) hdr_->mode;
      warn += ") is not supported for now";
      MRSWARN(warn);
    }
    }
  }
  ctrl_currentHasData_->setValue(hasData_);
  ctrl_currentLastTickWithData_->setValue(lastTickWithData_);
}
