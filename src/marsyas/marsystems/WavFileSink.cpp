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

#include "../common_source.h"
#include "WavFileSink.h"

using namespace std;
using namespace Marsyas;

WavFileSink::WavFileSink(mrs_string name):AbsSoundFileSink("WavFileSink",name)
{
  //type_ = "WavFileSink";
  //name_ = name;

  sfp_ = NULL;
  sdata_ = NULL;
  cdata_ = NULL;

  addControls();
}

WavFileSink::~WavFileSink()
{
  delete [] sdata_;
  delete [] cdata_;
  if (sfp_) fclose(sfp_);
}

MarSystem*
WavFileSink::clone() const
{
  return new WavFileSink(*this);
}

void
WavFileSink::addControls()
{
  addctrl("mrs_string/filename", "default");
  setctrlState("mrs_string/filename", true);

  // lossy encoding specific controls
  addctrl("mrs_natural/bitrate", 128);
  setctrlState("mrs_natural/bitrate", true);
  addctrl("mrs_natural/encodingQuality", 2);
  setctrlState("mrs_natural/encodingQuality", true);
  addctrl("mrs_string/id3tags", "id3tags");  // 1: track one O Blues genreopen
  setctrlState("mrs_string/id3tags", true);

}

bool
WavFileSink::checkExtension(mrs_string filename)
{
  FileName fn(filename);
  mrs_string wavext  = "wav";
  mrs_string sndext = "snd";

  if (fn.ext() == wavext)
    return true;
  else
    return false;
}

void
WavFileSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("WavFileSink::myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  nChannels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  delete [] sdata_;
  delete [] cdata_;

  sdata_ = new short[getctrl("mrs_natural/inSamples")->to<mrs_natural>() * nChannels_];
  cdata_ = new unsigned char[getctrl("mrs_natural/inSamples")->to<mrs_natural>() * nChannels_];

  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
}

void
WavFileSink::putHeader(mrs_string filename)
{
  mrs_natural nChannels = (mrs_natural)getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  sfp_ = fopen(filename.c_str(), "wb");

  written_ = 0;

  /* prepare header */
  hdr_.riff[0] = 'R';
  hdr_.riff[1] = 'I';
  hdr_.riff[2] = 'F';
  hdr_.riff[3] = 'F';

  hdr_.file_size = 44;

  hdr_.wave[0] = 'W';
  hdr_.wave[1] = 'A';
  hdr_.wave[2] = 'V';
  hdr_.wave[3] = 'E';

  hdr_.fmt[0] = 'f';
  hdr_.fmt[1] = 'm';
  hdr_.fmt[2] = 't';
  hdr_.fmt[3] = ' ';

#if defined(MARSYAS_BIGENDIAN)
  hdr_.chunk_size = ByteSwapLong(16);
  hdr_.format_tag = ByteSwapShort(1);
  hdr_.num_chans = ByteSwapShort((signed short)nChannels);
  hdr_.sample_rate = ByteSwapLong((mrs_natural)getctrl("mrs_real/israte")->to<mrs_real>());
  hdr_.bytes_per_sec = ByteSwapLong(hdr_.sample_rate * 2);
  hdr_.bytes_per_samp = ByteSwapShort(2);
  hdr_.bits_per_samp = ByteSwapShort(16);
  hdr_.data_length = ByteSwapLong(0);
#else
  hdr_.chunk_size = 16;
  hdr_.format_tag = 1;
  hdr_.num_chans = (signed short)nChannels;
  hdr_.sample_rate = (mrs_natural)getctrl("mrs_real/israte")->to<mrs_real>();
  hdr_.bytes_per_sec = hdr_.sample_rate * 2;
  hdr_.bytes_per_samp = 2;
  hdr_.bits_per_samp = 16;
  hdr_.data_length = 0;
#endif

  hdr_.data[0] = 'd';
  hdr_.data[1] = 'a';
  hdr_.data[2] = 't';
  hdr_.data[3] = 'a';

  fwrite(&hdr_, 4, 11, sfp_);

  sfp_begin_ = ftell(sfp_);
}

unsigned long
WavFileSink::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
          ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

unsigned short
WavFileSink::ByteSwapShort (unsigned short nValue)
{
  return (static_cast<unsigned short>((nValue & 0xff00) >> 8) |
          static_cast<unsigned short>((nValue & 0xff) << 8));
}

void
WavFileSink::putLinear16Swap(realvec& slice)
{
  for (mrs_natural c=0; c < nChannels_; ++c)
    for (mrs_natural t=0; t < inSamples_; t++)
    {
#if defined(MARSYAS_BIGENDIAN)
      sdata_[t*nChannels_ + c] = ByteSwapShort((short)(slice(c,t) * PCM_MAXSHRT));
#else
      sdata_[t*nChannels_ + c] = (short)(slice(c,t) * PCM_MAXSHRT);
#endif
    }

  if ((mrs_natural)fwrite(sdata_, sizeof(short), nChannels_ * inSamples_, sfp_) != nChannels_ * inSamples_)
  {
    MRSERR("Problem: could not write window to file " + filename_);
  }
}

void
WavFileSink::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  // copy input to output
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
    {
      out(o,t) = in(o,t);
    }

  long fileSize;
  fpos_ = ftell(sfp_);

  // jump to start and write data size
  fseek(sfp_, 40, SEEK_SET);
  written_ += inSamples_;
  fileSize = (written_ * 2 * nChannels_);
#if defined(MARSYAS_BIGENDIAN)
  fileSize =    ByteSwapLong(fileSize);
#endif

  fwrite(&fileSize, 4, 1, sfp_);
  fseek(sfp_, fpos_, SEEK_SET);

  putLinear16Swap(in);
}


















