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

#include <marsyas/common_source.h>
#include "OggFileSource.h"

#include <cstdio>

#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#include <vorbis/vorbisfile.h>


using std::ostringstream;
using namespace Marsyas;

OggFileSource::OggFileSource(mrs_string name):AbsSoundFileSource("OggFileSource", name)
{
  //type_ = "OggFileSource";
  //name_ = name;
  hasData_ = false;
  addControls();
}

OggFileSource::~OggFileSource()
{
  closeFile();
}

OggFileSource::OggFileSource(const OggFileSource& a):AbsSoundFileSource(a)
{
// 	type_ = a.type_;
// 	name_ = a.name_;
// 	ncontrols_ = a.ncontrols_;
//
// 	inSamples_ = a.inSamples_;
// 	inObservations_ = a.inObservations_;
// 	onSamples_ = a.onSamples_;
// 	onObservations_ = a.onObservations_;
// 	dbg_ = a.dbg_;
// 	mute_ = a.mute_;
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
}

MarSystem*
OggFileSource::clone() const
{
  return new OggFileSource(*this);
}

void
OggFileSource::addControls()
{
  // nChannels is one for now
  addctrl("mrs_natural/nChannels",1);
  addctrl("mrs_natural/bitRate", 160000);
  setctrlState("mrs_natural/nChannels", true);
  addctrl("mrs_bool/init", false);
  setctrlState("mrs_bool/init", true);
  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/loopPos", true);
  addctrl("mrs_natural/pos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);
  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/size", (mrs_natural)0);
  addctrl("mrs_string/filetype", "ogg");
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
  addctrl("mrs_natural/numFiles", 1);

  // addctrl("mrs_string/currentlyPlaying", "daufile");

  addctrl("mrs_string/currentlyPlaying", "doggfile", ctrl_currentlyPlaying_);
  addctrl("mrs_string/previouslyPlaying", "doggfile", ctrl_previouslyPlaying_);
  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
  addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
  addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);

  addctrl("mrs_bool/lastTickWithData", false);
  addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);
}


/**
 * Function: getHeader
 * Description: Opens the Ogg file and collects all the necessary
 *   information to update the MarSystem.
 */
void
OggFileSource::getHeader(mrs_string filename)
{
  // if we have a file open already, close it
  closeFile();
  mrs_real duration = 0;
  mrs_real israte = 22050.0;
  mrs_natural nChannels = 1;
  mrs_natural size = 0;
  hasData_ = false;
  mrs_natural bitRate = 128*1024;

  FILE* fp = fopen(filename.c_str(), "rb");
  vf = new OggVorbis_File;

  /* Using ov_open_callbacks because ov_open fails under windows. */
  if(fp && ov_open_callbacks(fp, vf, NULL, 0, OV_CALLBACKS_DEFAULT) == 0)
  {
    vi=ov_info(vf,-1);
    size = ov_pcm_total(vf,-1);
    duration = ov_time_total(vf,-1);
    nChannels = vi->channels;
    israte = vi->rate;
    hasData_ = true;
    bitRate = ov_bitrate(vf, -1);
  }
  else
  {
    (void) filename; // in case the macro is not expanded
    MRSWARN(filename + " does not appear to be an Ogg bitstream.");
  }
  setctrl("mrs_natural/nChannels", nChannels);
  setctrl("mrs_real/israte", israte);
  setctrl("mrs_natural/size", size);
  setctrl("mrs_bool/hasData", hasData_);
  setctrl("mrs_natural/bitRate", bitRate);
  updControl("mrs_real/duration", duration);
}

/**
 * Function: update
 *
 * Description: Performs the usual MarSystem update jobs. Additionally,
 *   it can update the position of the index in the file if
 *   the user seeks forward or backward.  Note that this
 *   is currently going to be slow as we have to refill the
 *   mad buffer each time somebody seeks in the file.
 *
 */
void
OggFileSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("OggFileSource::myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural pos = getctrl("mrs_natural/pos")->to<mrs_natural>();
  mrs_natural size = getctrl("mrs_natural/size")->to<mrs_natural>();

  // if the user has seeked somewhere in the file
  if ( pos < size && pos != ov_pcm_tell(vf))
  {
    ov_pcm_seek(vf, pos_);
  }

}

/**
 * Function: process
 * Description: Fills an output vector with samples.  In this case,
 *   getLinear16 does all the work.
 */
void OggFileSource::myProcess(realvec& in, realvec& out)
{
  (void) in;
  //checkFlow(in,out);

  if (hasData_)
  {
    /*mrs_real duration = getctrl("mrs_real/duration")->to<mrs_real>();
    mrs_real rate = getctrl("mrs_real/israte")->to<mrs_real>();
    */
    mrs_natural observations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    mrs_natural samples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
    mrs_natural israte = (mrs_natural)getctrl("mrs_real/israte")->to<mrs_real>();

    //mrs_natural size = (mrs_natural)(duration * rate);
    mrs_natural size = vi->channels*sizeof(short int)*((mrs_natural)(observations * samples));
    char* buf = new char[size];
    int bitstream=0;
    mrs_natural read = 0;
    long r = 0;
    bool eof = false;
    do
    {
      r = ov_read(vf, buf+read, size-read, 0, 2/*use 1 for 8bit samples..use 2 for 16*/, 1, &bitstream);
      if(r <= 0)
      {
        eof = true;
        break;
      }
      read += (mrs_natural) r;
    }
    while(read < size);

    //   getLinear16(out);  for (o=0; o < inObservations_; o++) {
    const double peak = 1.0/32768; // normalize 24-bit sample
    short int* src = (short int*)buf;
    for (mrs_natural o=0; o < observations; o++)
    {
      for (mrs_natural t=0; t < samples; t++)
      {
        const unsigned int i=vi->channels*t;
        switch(vi->channels)
        {
        case 2:
          out(0,t) = (src[i] + src[i+1])*peak/2;
          break;
        default:
          out(0,t) = src[i]*peak;
        }
      }
    }
    delete [] buf;
    if(eof)
      closeFile();

  }
  else
    out.setval(0.0);
  if (hasData_)
  {
    // hasData_ = (samplesOut_ < repetitions_ * csize_);
  }
  else
  {
    // if hasData_ was false already it got set in fillStream
    MRSWARN("OggFileSource: track ended.");
  }
}

/**
 * Function: closeFile()
 *
 * Description: Close the file if its open, release memory, and
 *   release mad structs.
 *
 */
void OggFileSource::closeFile()
{

  if(hasData_)
  {
    ov_clear(vf);
    delete vf;
  }

  hasData_ = false;
}
