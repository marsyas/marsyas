/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.cmu.edu>
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
   \class OggFileSource
   \brief OggFileSource read ogg files using vorbisfile
   \author Taras Glek
 
This class reads an ogg file using the vorbis ogg decoder library.
*/


#include "OggFileSource.h"
using namespace std;


OggFileSource::OggFileSource(string name)
{
  type_ = "OggFileSource";
  name_ = name;
  notEmpty_ = false;
  addControls();
}


OggFileSource::~OggFileSource()
{
  closeFile();
}

OggFileSource::OggFileSource(const OggFileSource& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_;

  inSamples_ = a.inSamples_;
  inObservations_ = a.inObservations_;
  onSamples_ = a.onSamples_;
  onObservations_ = a.onObservations_;
  dbg_ = a.dbg_;
  mute_ = a.mute_;
}

MarSystem*
OggFileSource::clone() const
{
  return new OggFileSource(*this);
}

void
OggFileSource::addControls()
{
  addDefaultControls();

  // nChannels is one for now
  addctrl("natural/nChannels",1);
  addctrl("natural/bitRate", 160000);
  setctrlState("natural/nChannels", true);
  addctrl("bool/init", false);
  setctrlState("bool/init", true);
  addctrl("bool/notEmpty", true);
  addctrl("natural/loopPos", (natural)0);
  setctrlState("natural/loopPos", true);
  addctrl("natural/pos", (natural)0);
  setctrlState("natural/pos", true);
  addctrl("string/filename", "daufile");
  setctrlState("string/filename", true);
  addctrl("natural/size", (natural)0);
  addctrl("string/filetype", "ogg");
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
  addctrl("natural/numFiles", 1);

  addctrl("string/currentlyPlaying", "daufile");
}


/**
 * Function: getHeader
 * Description: Opens the MP3 file and collects all the necessary
 *   information to update the MarSystem. 
 */
void
OggFileSource::getHeader(string filename)
{
  // if we have a file open already, close it
  closeFile();
  real duration = 0;
  real israte = 22050.0;
  natural nChannels = 1;
  natural size = 0;
  notEmpty_ = false;
  natural bitRate = 128*1024;
  
#ifdef OGG_VORBIS
  FILE* fp = fopen(filename.c_str(), "rb");

  if(fp && ov_open(fp, &vf, NULL, 0) == 0)
  {
    vi=ov_info(&vf,-1);
    size = ov_pcm_total(&vf,-1);
    duration = ov_time_total(&vf,-1);
    nChannels = vi->channels;
    israte = vi->rate;
    notEmpty_ = true;
    bitRate = ov_bitrate(&vf, -1);
  }
  else
#endif
  {
    MRSWARN(filename + " does not appear to be an Ogg bitstream.");
  }
  setctrl("natural/nChannels", nChannels);
  setctrl("real/israte", israte);
  setctrl("natural/size", size);
  setctrl("bool/notEmpty", (MarControlValue)notEmpty_);
  setctrl("natural/bitRate", bitRate);
  updctrl("real/duration", duration);
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
OggFileSource::update()
{
  MRSDIAG("OggFileSource::update");

  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));

  natural pos = getctrl("natural/pos").toNatural();
  natural size = getctrl("natural/size").toNatural();

#ifdef OGG_VORBIS
  // if the user has seeked somewhere in the file
  if ( pos < size && pos != ov_pcm_tell(&vf))
  {
    ov_pcm_seek(&vf, pos_);
  }
#endif

}

/**
 * Function: process
 * Description: Fills an output vector with samples.  In this case,
 *   getLinear16 does all the work.
 */
void OggFileSource::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  if (notEmpty_)
  {
#ifdef OGG_VORBIS
    /*real duration = getctrl("real/duration").toReal();
    real rate = getctrl("real/israte").toReal();
    */
    natural observations = getctrl("natural/inObservations").toNatural();
    natural samples = getctrl("natural/inSamples").toNatural();
    natural israte = (natural)getctrl("real/israte").toReal();

    //size_t size = (size_t)(duration * rate);
    size_t size = vi->channels*sizeof(short int)*((size_t)(observations * samples));
    char* buf = new char[size];
    int bitstream=0;
    size_t read = 0;
    long r = 0;
    bool eof = false; 
    do
    {
      r = ov_read(&vf, buf+read, size-read, 0, 2/*use 1 for 8bit samples..use 2 for 16*/, 1, &bitstream);
      if(r <= 0)
      {
        eof = true;
        break;
      }
      read += (size_t) r;
    }
    while(read < size);

    //   getLinear16(out);  for (o=0; o < inObservations_; o++) {
    const double peak = 1.0/32767; // normalize 24-bit sample
    short int* src = (short int*)buf;
    for (o=0; o < observations; o++)
    {
      for (t=0; t < samples; t++)
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
        
#endif
  }
  else
    out.setval(0.0);
  if (notEmpty_)
  {
    // notEmpty_ = (samplesOut_ < repetitions_ * csize_);
  }
  else
  {
    // if notEmpty_ was false already it got set in fillStream
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
#ifdef OGG_VORBIS

  if(notEmpty_)
    ov_clear(&vf);
#endif

  notEmpty_ = false;
}
