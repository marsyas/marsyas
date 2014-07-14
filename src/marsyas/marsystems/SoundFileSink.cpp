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

#include "../common_source.h"
#include "SoundFileSink.h"

#ifdef MARSYAS_LAME
#include "MP3FileSink.h"
#endif

using namespace std;
using namespace Marsyas;

#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)



SoundFileSink::SoundFileSink(mrs_string name):
  MarSystem("SoundFileSink",name),
  backend_(0)
{
  addControls();
}


SoundFileSink::SoundFileSink(const SoundFileSink& a):
  MarSystem(a),
  backend_(0)
{
}


MarSystem*
SoundFileSink::clone() const
{
  return new SoundFileSink(*this);
}

SoundFileSink::~SoundFileSink()
{
  delete backend_;
}

void
SoundFileSink::addControls()
{
  addctrl("mrs_string/filename", string());
  setctrlState("mrs_string/filename", true);
  // lossy encoding specific controls
  addctrl("mrs_natural/bitrate", 128);
  setctrlState("mrs_natural/bitrate", true);
  addctrl("mrs_natural/encodingQuality", 2);
  setctrlState("mrs_natural/encodingQuality", true);
  addctrl("mrs_string/id3tags", "noTitle|noArtist|noAlbum|1978|noComment|1|0");  // 1: track one O Blues genreopen
  setctrlState("mrs_string/id3tags", true);

  // pauses the output.
  addctrl("mrs_bool/pause", false);
}
void
SoundFileSink::putHeader()
{
  assert(!filename_.empty());
  assert(backend_ != 0);

  backend_->putHeader(filename_);
}



bool
SoundFileSink::updateBackend()
{
  delete backend_;
  backend_ = 0;

  if (filename_.empty())
    return false;

  // check if file exists
  bool file_exists;
  {
    FILE *file = fopen(filename_.c_str(), "wb");
    file_exists = file != 0;
    if (file)
      fclose(file);
  }
  if (!file_exists)
  {
    MRSWARN("SoundFileSink: Failed to open file for writing: " << filename_);
    return false;
  }

  // try to open file with appropriate format
  mrs_string::size_type pos = filename_.rfind(".", filename_.length());
  mrs_string ext;
  if (pos != mrs_string::npos)
    ext = filename_.substr(pos);

  if (ext == ".au")
  {
    backend_ = new AuFileSink(getName());
  }
  else if (ext == ".wav")
  {
    backend_ = new WavFileSink(getName());
  }
#ifdef MARSYAS_LAME
  else if (ext == ".mp3")
  {
    backend_ = new MP3FileSink(getName());
  }
#endif
  else
  {
    MRSWARN("Unknown file extension: " << filename_);
    return false;
  }

  return true;
}

void
SoundFileSink::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  const string &new_filename = getctrl("mrs_string/filename")->to<mrs_string>();

  if (new_filename != filename_)
  {
    filename_ = new_filename;

    if (updateBackend())
    {
      backend_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
      backend_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
      backend_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
      backend_->update();

      putHeader();
      filename_ = getctrl("mrs_string/filename")->to<mrs_string>();

      setctrl("mrs_real/israte", backend_->getctrl("mrs_real/israte"));
    }
  }

  if (backend_)
  {
    backend_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
    backend_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
    backend_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
    // [ML] the filename is now propagated to the child
    backend_->setctrl("mrs_string/filename", getctrl("mrs_string/filename"));
    backend_->setctrl("mrs_natural/bitrate", getctrl("mrs_natural/bitrate"));
    backend_->setctrl("mrs_natural/encodingQuality", getctrl("mrs_natural/encodingQuality"));
    backend_->setctrl("mrs_string/id3tags", getctrl("mrs_string/id3tags"));
    backend_->update();

    setctrl("mrs_natural/onSamples", backend_->getctrl("mrs_natural/onSamples"));
    setctrl("mrs_natural/onObservations", backend_->getctrl("mrs_natural/onObservations"));
    setctrl("mrs_real/osrate", backend_->getctrl("mrs_real/israte"));
  }
}


void
SoundFileSink::myProcess(realvec& in, realvec& out)
{

  mrs_bool paused = getctrl("mrs_bool/pause")->to<mrs_bool>();

  if ((backend_ != NULL) &&  (paused==false))
  {
    backend_->process(in,out);
  }


}









void
SoundFileSink::putFloat(mrs_natural c, realvec& win)
{
  (void) c;
  (void) win;
  MRSWARN("SoundFileSink::putFloat not implemented");
}





