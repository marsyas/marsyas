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
#include "WavFileSource.h"


using namespace std;
using namespace Marsyas;

WavFileSource::WavFileSource(mrs_string name):AbsSoundFileSource("WavFileSource",name)
{
  idata_ = 0;
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
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_currentHasData_ = getctrl("mrs_bool/currentHasData");
  ctrl_currentLastTickWithData_ = getctrl("mrs_bool/currentLastTickWithData");
}



WavFileSource::~WavFileSource()
{
  delete [] idata_;
  delete [] sdata_;
  delete [] cdata_;
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
  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_bool/lastTickWithData", false);

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

  addctrl("mrs_natural/advance", 0);
  setctrlState("mrs_natural/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames", true);

  addctrl("mrs_natural/numFiles", 1);
  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_); //"dwavfile" [?]
  addctrl("mrs_string/previouslyPlaying", "daufile", ctrl_previouslyPlaying_); //"dwavfile" [?]

  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
  addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
  addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);

  addctrl("mrs_bool/currentHasData", true, ctrl_currentHasData_);

  addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);

}

void
WavFileSource::getHeader(mrs_string filename)
{

  if (sfp_ != NULL)
    fclose(sfp_);

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
      setctrl("mrs_real/israte", (mrs_real)22050.0);
      setctrl("mrs_natural/size", 0);
      hasData_ = false;
      lastTickWithData_ = true;
      setctrl("mrs_bool/hasData", false);
      setctrl("mrs_bool/lastTickWithData", true);

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
#else
        //chunkSize = chunkSize;
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
#else
      //chunkSize = chunkSize;
#endif

      unsigned short format_tag;
      if (fread(&format_tag, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }

#if defined(MARSYAS_BIGENDIAN)
      format_tag = ByteSwapShort(format_tag);
#else
      //format_tag = format_tag;
#endif

      if (format_tag != 1)
      {
        fclose(sfp_);
        MRSWARN("Non pcm(compressed) wave files are not supported");
        exit(1);
      }

      // Get number of channels
      unsigned short channels;
      if (fread(&channels, 2,1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }

#if defined(MARSYAS_BIGENDIAN)
      channels = ByteSwapShort(channels);
#else
      //channels = channels;
#endif

      setctrl("mrs_natural/onObservations", (mrs_natural)channels);

      unsigned int srate;
      if (fread(&srate, 4,1,sfp_) != 1) {
        MRSERR("Error reading wav file");
      }

#if defined(MARSYAS_BIGENDIAN)
      srate = ByteSwapLong(srate);
#else
      //srate = srate;
#endif


      setctrl("mrs_real/israte", (mrs_real)srate);
      setctrl("mrs_real/osrate", (mrs_real)srate);

      fseek(sfp_,6,SEEK_CUR);
      if (fread(&bits_, 2, 1, sfp_) != 1) {
        MRSERR("Error reading wav file");
      }

#if defined(MARSYAS_BIGENDIAN)
      bits_ = ByteSwapShort(bits_);
#else
      //bits_ = bits_;
#endif

      if ((bits_ != 16)&&(bits_ != 8)&&(bits_!=32))
      {
        MRSWARN("WavFileSource::Only linear 8-bit, 16-bit, and 32-bit samples are supported ");
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
#else
        //chunkSize = chunkSize;
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
#else
      //bytes = bytes;
#endif

      //size in number of samples per channel
      size_ = bytes / (bits_ / 8)/ (getctrl("mrs_natural/onObservations")->to<mrs_natural>());
      csize_ = size_;

      setctrl("mrs_natural/size", size_);
      ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);
      ctrl_previouslyPlaying_->setValue(filename, NOUPDATE);
      ctrl_currentLabel_->setValue((mrs_real)0.0, NOUPDATE);
      ctrl_previousLabel_->setValue((mrs_real)0.0, NOUPDATE);

      ctrl_labelNames_->setValue(",", NOUPDATE);
      ctrl_nLabels_->setValue(0, NOUPDATE);
      sfp_begin_ = ftell(sfp_);
      hasData_ = true;
      lastTickWithData_ = false;
      pos_ = 0;
      samplesOut_ = 0;
      MRSDIAG("WavFileSource: "
              << filename
              << " has the following properties: ");
      mrs_real temprate = getControl("mrs_real/israte")->to<mrs_real>();
      mrs_natural numInSamples = getControl("mrs_natural/inSamples")->to<mrs_natural>();
      MRSDIAG("israte == " << temprate);
      MRSDIAG("inSamples == " << numInSamples);
      (void) temprate;
      (void) numInSamples; // in case the macro is not expanded

    }
  }
  else
  {
    MRSWARN("couldn't open file: " << filename);
    // setctrl("mrs_natural/nChannels", 1);
    setctrl("mrs_real/israte", (mrs_real)22050.0);
    setctrl("mrs_natural/onObservations", 1);
    setctrl("mrs_natural/size", 0);
    hasData_ = false;
    lastTickWithData_ = true;
    setctrl("mrs_bool/hasData", false);
    setctrl("mrs_bool/lastTickWithData", true);
    pos_ = 0;
  }

  nChannels_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
}

void
WavFileSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
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

  delete [] idata_;
  delete [] sdata_;
  delete [] cdata_;

  idata_ = new int[inSamples_ * nChannels_];
  sdata_ = new short[inSamples_ * nChannels_];
  cdata_ = new unsigned char[inSamples_ * nChannels_];

  repetitions_ = getctrl("mrs_real/repetitions")->to<mrs_real>();
  duration_ = getctrl("mrs_real/duration")->to<mrs_real>();

  if (duration_ != -1.0)
  {
    csize_ = (mrs_natural)(duration_ * israte_);
  }

  samplesToRead_ = inSamples_ * nChannels_;
  MRSDIAG("WavFileSource::myUpdate "
          << " has the following properties: ");
  mrs_real temprate = getControl("mrs_real/israte")->to<mrs_real>();
  mrs_natural numInSamples = getControl("mrs_natural/inSamples")->to<mrs_natural>();
  MRSDIAG("israte == " << temprate);
  MRSDIAG("inSamples == " << numInSamples);
  (void) temprate;
  (void) numInSamples; // in case the macro is not expanded

}

mrs_natural
WavFileSource::getLinear8(realvec& slice)
{
  mrs_natural t;
  mrs_natural c = 0;
  fseek(sfp_, pos_ * nChannels_ + sfp_begin_, SEEK_SET);

  samplesToRead_ = inSamples_ * nChannels_;
  samplesRead_ = (mrs_natural)fread(cdata_, sizeof(unsigned char), samplesToRead_, sfp_);


  if (samplesRead_ != samplesToRead_)
  {
    for (c=0; c < nChannels_; ++c)
      for (t=0; t < inSamples_; t++)
      {
        slice(c,t) = 0.0;
      }
    samplesToWrite_ = samplesRead_ / nChannels_;
  }
  else
    samplesToWrite_ = inSamples_;

  for (t=0; t < samplesToWrite_; t++)
  {
    for (c=0; c < nChannels_; ++c)
    {
      slice(c, t) = (mrs_real)-1.0 + (mrs_real) cdata_[nChannels_ * t + c] / 127;
    }
  }

  pos_ += samplesToWrite_;
  return pos_;
}

// oh dear.  "long" here means 32 bits
unsigned long
WavFileSource::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
          ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

unsigned int
WavFileSource::ByteSwapInt(unsigned int nInt)
{
  return (((nInt&0x000000FF)<<24)+((nInt&0x0000FF00)<<8)+
          ((nInt&0x00FF0000)>>8)+((nInt&0xFF000000)>>24));
}


unsigned short
WavFileSource::ByteSwapShort (unsigned short nValue)
{
  return (static_cast<unsigned short>((nValue & 0xff00) >> 8) |
          static_cast<unsigned short>((nValue & 0xff) << 8));
}

mrs_natural
WavFileSource::getLinear32(realvec& slice)
{
  mrs_natural c,t;

  fseek(sfp_, 4 * pos_ * nChannels_ + sfp_begin_, SEEK_SET);
  samplesRead_ = (mrs_natural)fread(idata_, sizeof(int), samplesToRead_, sfp_);

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
          idata_[nt_ + c] = 0;
        }
      }
  }

  // write the read samples to output slice once for each channel
  for (t=0; t < samplesToWrite_; t++)
  {
    ival_ = 0;
    nt_ = nChannels_ * t;
#if defined(MARSYAS_BIGENDIAN)
    for (c=0; c < nChannels_; ++c)
    {
      // oh dear.  "long" here means 32 bits
      ival_ = ByteSwapInt(idata_[nt_ + c]);
      slice(c, t) = (mrs_real) ival_ / (PCM_FMAXINT + 1);
    }
#else
    for (c=0; c < nChannels_; ++c)
    {
      ival_ = idata_[nt_ + c];
      slice(c, t) = ((mrs_real) ival_ / (PCM_FMAXINT + 1));
    }
#endif
  }

  pos_ += samplesToWrite_;
  return pos_;
}


mrs_natural
WavFileSource::getLinear16(realvec& slice)
{
  mrs_natural c,t;

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
    {
      sval_ = ByteSwapShort(sdata_[nt_ + c]);
      slice(c, t) = (mrs_real) sval_ / (PCM_FMAXSHRT + 1);
    }
#else
    for (c=0; c < nChannels_; ++c)
    {
      sval_ = sdata_[nt_ + c];
      slice(c, t) = ((mrs_real) sval_ / (PCM_FMAXSHRT + 1));
    }
#endif
  }

  pos_ += samplesToWrite_;
  return pos_;
}

void
WavFileSource::myProcess(realvec& in, realvec& out)
{
  (void) in;
  switch(bits_)
  {
  case 32:
  {
    getLinear32(out);
    ctrl_pos_->setValue(pos_, NOUPDATE);

    if (pos_ >= rewindpos_ + csize_)
    {
      if (repetitions_ != 1)
        pos_ = rewindpos_;
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
      lastTickWithData_ = ((pos_ + onSamples_ >= rewindpos_ + csize_) && hasData_);
    }

    if (repetitions_ == -1)
    {
      hasData_ = true;
      lastTickWithData_ = false;
    }
    break;
  }
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

    if (repetitions_ != 1)
    {
      hasData_ = (samplesOut_ < repetitions_ * csize_);

      lastTickWithData_ = ((samplesOut_  + onSamples_>= repetitions_ * csize_) && hasData_);
    }

    else
    {
      hasData_ = pos_ < rewindpos_ + csize_;
      lastTickWithData_ = ((pos_ + onSamples_ >= rewindpos_ + csize_) && hasData_);
    }

    if (repetitions_ == -1)
    {
      hasData_ = true;
      lastTickWithData_ = false;
    }
    break;
  }
  case 8:
  {
    getLinear8(out);
    ctrl_pos_->setValue(pos_, NOUPDATE);

    if (pos_ >= rewindpos_ + csize_)
    {
      if (repetitions_ != 1)
        pos_ = rewindpos_;
    }
    samplesOut_ += onSamples_;

    if (repetitions_ != 1)
    {
      hasData_ = (samplesOut_ < repetitions_ * csize_);
      lastTickWithData_ = ((samplesOut_ + onSamples_ >= repetitions_ * csize_) && hasData_);
    }
    else
    {
      hasData_ = pos_ < rewindpos_ + csize_;
      lastTickWithData_ = ((pos_ + onSamples_ >= rewindpos_ + csize_) && hasData_);
    }

    if (repetitions_ == -1)
    {
      hasData_ = true;
      lastTickWithData_ = false;
    }
    break;


  }
  }
  ctrl_currentHasData_->setValue(hasData_);

  ctrl_currentLastTickWithData_->setValue(lastTickWithData_);
}
