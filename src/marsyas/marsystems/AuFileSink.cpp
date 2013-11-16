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
#include "../FileName.h"
#include "AuFileSink.h"
#include "AuFileSource.h"

#ifdef MARSYAS_WIN32
#ifndef MARSYAS_CYGWIN
typedef __int32 int32_t;
#endif
#else
#include <stdint.h>
#endif

#include <cstddef>

using std::ostringstream;
using std::size_t;
using namespace Marsyas;

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






#define SND_MAGIC_NUM 0x2e736e64


/* Array containing descriptions of
the various formats for the samples
of the Next .snd/ Sun .au format */

/* types of .snd files */
#define SND_FORMAT_UNSPECIFIED 0
#define SND_FORMAT_MULAW_8     1
#define SND_FORMAT_LINEAR_8    2
#define SND_FORMAT_LINEAR_16   3
#define SND_FORMAT_LINEAR_24   4
#define SND_FORMAT_LINEAR_32   5
#define SND_FORMAT_FLOAT       6

AuFileSink::AuFileSink(mrs_string name):AbsSoundFileSink("AuFileSink",name)
{
  //type_ = "AuFileSink";
  //name_ = name;

  sfp_ = NULL;
  cdata_ = NULL;
  sdata_ = NULL;
  hdr_ = new snd_header;

  addControls();
}

AuFileSink::~AuFileSink()
{
  delete [] sdata_;
  delete [] cdata_;
  delete hdr_;
  if (sfp_) fclose(sfp_);
}


AuFileSink::AuFileSink(const AuFileSink& a): AbsSoundFileSink(a)
{
  hdr_ = new snd_header;
}



MarSystem*
AuFileSink::clone() const
{
  return new AuFileSink(*this);
}

void
AuFileSink::addControls()
{
  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);

  // lossy encoding specific controls
  addctrl("mrs_natural/bitrate", 128);
  setctrlState("mrs_natural/bitrate", true);
  addctrl("mrs_natural/encodingQuality", 2);
  setctrlState("mrs_natural/encodingQuality", true);
  addctrl("mrs_string/id3tags", "noTitle|noArtist|noAlbum|1978|noComment|1|0");  // 1: track one O Blues genreopen
  setctrlState("mrs_string/id3tags", true);

}

bool
AuFileSink::checkExtension(mrs_string filename)
{
  FileName fn(filename);
  mrs_string auext  = "au";
  mrs_string sndext = "snd";

  if ((fn.ext() == auext) || (fn.ext() == sndext))
    return true;
  else
    return false;
}

void
AuFileSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AudioFileSink::myUpdate");
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
AuFileSink::putHeader(mrs_string filename)
{
  mrs_natural nChannels = (mrs_natural)getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  written_ = 0;
  const char *comment = "MARSYAS 2001, George Tzanetakis.\n";
  size_t commentSize = strlen(comment);
  sfp_ = fopen(filename.c_str(), "wb");
  hdr_->pref[0] = '.';
  hdr_->pref[1] = 's';
  hdr_->pref[2] = 'n';
  hdr_->pref[3] = 'd';

#if defined(MARSYAS_BIGENDIAN)
  hdr_->hdrLength = 24 + commentSize;
  hdr_->fileLength = 0;
  hdr_->mode = SND_FORMAT_LINEAR_16;
  hdr_->srate = (mrs_natural)getctrl("mrs_real/israte")->to<mrs_real>();
  hdr_->channels = nChannels;
#else
  hdr_->hdrLength = ByteSwapLong(24 + (unsigned long)commentSize);
  hdr_->fileLength = ByteSwapLong(0);
  hdr_->mode = ByteSwapLong(SND_FORMAT_LINEAR_16);
  hdr_->srate = ByteSwapLong((mrs_natural)getctrl("mrs_real/israte")->to<mrs_real>());
  hdr_->channels = ByteSwapLong(nChannels);
#endif

  fwrite(hdr_, 24, 1, sfp_);
  // Write comment part of header
  fwrite(comment, commentSize, 1, sfp_);
  sfp_begin_ = ftell(sfp_);
}

unsigned long
AuFileSink::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
          ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

unsigned short
AuFileSink::ByteSwapShort (unsigned short nValue)
{
  return (static_cast<unsigned short>((nValue & 0xff00) >> 8) |
          static_cast<unsigned short>((nValue & 0xff) << 8));
}

void
AuFileSink::putLinear16(realvec& slice)
{
  mrs_natural c,t;
  for (c=0; c < nChannels_; ++c)
    for (t=0; t < inSamples_; t++)
    {
#if defined(MARSYAS_BIGENDIAN)
      sdata_[t*nChannels_ + c] = (short)(slice(c,t) * PCM_MAXSHRT);
#else
      sdata_[t*nChannels_ + c] = ByteSwapShort((short)(slice(c,t) * PCM_MAXSHRT));
#endif
    }

  if ((mrs_natural)fwrite(sdata_, sizeof(short), nChannels_ * inSamples_, sfp_) != nChannels_ * inSamples_)
  {
    MRSWARN("Problem: could not write window to file" + filename_);
  }
}

void
AuFileSink::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //checkFlow(in,out);

  // copy input to output
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
    {
      if (in(o,t) > 1.0)
      {
        MRSWARN("AuFileSink::Value out of range > 1.0");
      }
      if (in(o,t) < -1.0)
      {
        MRSWARN("AuFileSink::Value out of range < -1.0");
      }
      out(o,t) = in(o,t);
    }

  long fileSize;
  fpos_ = ftell(sfp_);
  fseek(sfp_, 8, SEEK_SET);
  written_ += inSamples_;
#if defined(MARSYAS_BIGENDIAN)
  fileSize = (written_ * 2 * nChannels_);
#else
  fileSize = ByteSwapLong(written_ * 2 * nChannels_);
#endif

  fwrite(&fileSize, 4, 1, sfp_);
  fseek(sfp_, fpos_, SEEK_SET);

  putLinear16(in);
}
















