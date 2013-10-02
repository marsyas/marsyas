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
#include "MP3FileSink.h"

#include "lame/lame.h"

using std::ostringstream;
using namespace Marsyas;

MP3FileSink::MP3FileSink(mrs_string name):AbsSoundFileSink("MP3FileSink",name)
{
  //type_ = "MP3FileSink";
  //name_ = name;
  gfp_ = NULL;
  sfp_ = NULL;
  leftpcm_ = NULL;
  rightpcm_ = NULL;

  mp3Buffer_ = NULL;
  addControls();
}

MP3FileSink::~MP3FileSink()
{


  if (sfp_)
  {
    mrs_natural encodeCheck = lame_encode_flush(gfp_, mp3Buffer_, 0);
    mrs_natural owrite = (int) fwrite(mp3Buffer_, 1, encodeCheck, sfp_);
    if (owrite != encodeCheck) {
      MRSWARN("Writing MP3 data to "+filename_+" failed.");
    }
    fclose(sfp_);
  }


  delete [] mp3Buffer_;
  delete [] leftpcm_;
  delete [] rightpcm_;
}

MarSystem*
MP3FileSink::clone() const
{
  return new MP3FileSink(*this);
}

void
MP3FileSink::addControls()
{
  addctrl("mrs_string/filename", "default");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/bitrate", 128);
  setctrlState("mrs_natural/bitrate", true);
  addctrl("mrs_natural/encodingQuality", 2);
  setctrlState("mrs_natural/encodingQuality", true);
  addctrl("mrs_string/id3tags", "noTitle|noArtist|noAlbum|1978|noComment|1|0");  // 1: track one O Blues genreopen
  setctrlState("mrs_string/filename", true);
}

bool
MP3FileSink::checkExtension(mrs_string filename)
{
  FileName fn(filename);
  mrs_string mp3ext  = "mp3";

  if (fn.ext() == mp3ext)
    return true;
  else
    return false;
}

void
MP3FileSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("MP3FileSink::myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  nChannels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  // initialize to default encoding parameters
  gfp_ = lame_init();

  mrs_natural lameNbChannels = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  delete [] mp3Buffer_;
  mrs_natural mp3BufferSize = (int) ceil(1.25*inSamples_ + 7200);
  mp3Buffer_ = new unsigned char[mp3BufferSize]; //[LAME_MAXMP3BUFFER];
  delete [] leftpcm_;
  delete [] rightpcm_;
  leftpcm_ = new short[getctrl("mrs_natural/inSamples")->to<mrs_natural>()];
  rightpcm_ = new short[getctrl("mrs_natural/inSamples")->to<mrs_natural>()];

  if (lameNbChannels>2) {
    MRSWARN("Lame can handle up to 2 channels. Only the first 2 observation vectors will be written.");
    lameNbChannels = 2;
  }
  if (lameNbChannels==1)
    lame_set_mode(gfp_, (MPEG_mode) 3); // mode = 0,1,2,3 = stereo, jstereo, dual channel (not supported), mono
  else
    lame_set_mode(gfp_, (MPEG_mode) 1);

  lame_set_num_channels(gfp_,lameNbChannels);
  lame_set_in_samplerate(gfp_,(int) ceil(getctrl("mrs_real/israte")->to<mrs_real>()));
  lame_set_brate(gfp_,128); // ,(int) ceil(getctrl("mrs_natural/bitrate")->to<mrs_natural>())
  lame_set_quality(gfp_,2);   /* 2=high  5 = medium  7=low */ // ,(int) ceil(getctrl("mrs_natural/encodingQuality")->to<mrs_natural>())

  // split id3 string
  mrs_string id3tags = getctrl("mrs_string/id3tags")->to<mrs_string>();
  mrs_natural pos=0;
  pos = id3tags.find("|", 0);
  id3tag_set_title(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_artist(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_album(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_year(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_comment(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_track(gfp_, id3tags.substr(0, pos).c_str());
  id3tags = id3tags.substr(pos+1);
  pos = id3tags.find("|", 0);
  id3tag_set_genre(gfp_, id3tags.substr(0, pos).c_str());

  int ret_code = lame_init_params(gfp_);
  if (ret_code == -1)
    MRSWARN("Initialization of the lame encoder failed.");



  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
}

void
MP3FileSink::putHeader(mrs_string filename)
{
  sfp_ = fopen(filename.c_str(), "wb");
}

void
MP3FileSink::myProcess(realvec& in, realvec& out)
{


  mrs_natural t,o;

  // copy input to output
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
    {
      out(o,t) = in(o,t);
    }

  mrs_natural encodeCheck=-1;

  // fill left buffer
  for (t=0; t < inSamples_; t++)
    leftpcm_[t] = (short)(in(0,t) * PCM_MAXSHRT);
  if (inObservations_>1) {
// fill right buffer
    for (t=0; t < inSamples_; t++)
      rightpcm_[t] = (short)(in(1,t) * PCM_MAXSHRT);

    encodeCheck = lame_encode_buffer(gfp_,
                                     leftpcm_, rightpcm_,
                                     inSamples_, mp3Buffer_, 0);
  }
  else
    encodeCheck = lame_encode_buffer(gfp_,
                                     leftpcm_, leftpcm_,
                                     inSamples_, mp3Buffer_,0); // [ML] 0 do not check for buffer size, should be sizeof(mp3Buffer_) but check fails


  /* was our output buffer big enough? */
  if (encodeCheck < 0) {
    if (encodeCheck == -1) {
      MRSWARN("mp3 buffer is not big enough... \n");
    }
    else
      MRSWARN("mp3 internal error");
  }
  mrs_natural owrite = (int) fwrite(mp3Buffer_, 1, encodeCheck, sfp_);
  if (owrite != encodeCheck) {
    MRSWARN("Writing MP3 data to "+filename_+" failed.");
  }



}


















