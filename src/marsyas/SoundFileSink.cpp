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

#include "common_source.h"
#include "SoundFileSink.h"

#ifdef MARSYAS_LAME
#include "MP3FileSink.h"
#endif

using namespace std;
using namespace Marsyas;

#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)
	


SoundFileSink::SoundFileSink(mrs_string name):MarSystem("SoundFileSink",name)
{

  
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  dest_ = NULL;

  addControls();
}


SoundFileSink::SoundFileSink(const SoundFileSink& a):MarSystem(a)
{
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  dest_ = NULL;
}


MarSystem* 
SoundFileSink::clone() const
{
  return new SoundFileSink(*this);
}

SoundFileSink::~SoundFileSink()
{
  delete dest_;
}

void
SoundFileSink::addControls()
{
  addctrl("mrs_string/filename", "defaultfile");
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
  mrs_string filename = getctrl("mrs_string/filename")->to<mrs_string>();
  dest_->putHeader(filename);
}



bool 
SoundFileSink::checkType()
{
  mrs_string filename = getctrl("mrs_string/filename")->to<mrs_string>();
  // check if file exists
	if (filename != "defaultfile")
    {
		sfp_ = fopen(filename.c_str(), "wb");
		if (sfp_ == NULL) 
		{
			mrs_string wrn = "SoundFileSink::checkType: Problem opening file ";
			wrn += filename;
			MRSWARN(wrn);
			filename = "defaultfile";
			return false;
		}
		fclose(sfp_);
    }
	
	// try to open file with appropriate format 
  mrs_string::size_type pos = filename.rfind(".", filename.length());
  mrs_string ext;
  if (pos == mrs_string::npos) ext = "";
  else 
    ext = filename.substr(pos, filename.length());  
  
  if (ext == ".au")
    {
      delete dest_;
			dest_ = new AuFileSink(getName());
    }
  else if (ext == ".wav")
    {
      delete dest_;
			dest_ = new WavFileSink(getName());
    }
#ifdef MARSYAS_LAME
  else if (ext == ".mp3")
    {
      delete dest_;
			dest_ = new MP3FileSink(getName());
    }
#endif
  else 
    {
      if (filename != "defaultfile")
	{
	  mrs_string wrn = "Unsupported format for file ";
	  wrn += filename;
	  MRSWARN(wrn);
	  filename_ = "defaultfile";
	  setctrl("mrs_string/filename", "defaultfile");
	  return false;
	}
      else 
	return false;
    }
  
  return true;
}

void 
SoundFileSink::myUpdate(MarControlPtr sender)
{
	(void) sender;  //suppress warning of unused parameter(s)
  
  if (filename_ != getctrl("mrs_string/filename")->to<mrs_string>())
    {
      if (checkType() == true)
	{
		dest_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
	  dest_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
	  dest_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
	  dest_->update();

	  putHeader();
	  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
	  
	  setctrl("mrs_real/israte", dest_->getctrl("mrs_real/israte"));
	}
      else 
	dest_ = NULL;
    }

  
  if (dest_ != NULL)
    {
      dest_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
      dest_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
	  dest_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
      // [ML] the filename is now propagated to the child 
		dest_->setctrl("mrs_string/filename", getctrl("mrs_string/filename"));
		dest_->setctrl("mrs_natural/bitrate", getctrl("mrs_natural/bitrate"));
		dest_->setctrl("mrs_natural/encodingQuality", getctrl("mrs_natural/encodingQuality"));
	 	dest_->setctrl("mrs_string/id3tags", getctrl("mrs_string/id3tags"));
		dest_->update();
      
      setctrl("mrs_natural/onSamples", dest_->getctrl("mrs_natural/onSamples"));
      setctrl("mrs_natural/onObservations", dest_->getctrl("mrs_natural/onObservations"));
      setctrl("mrs_real/osrate", dest_->getctrl("mrs_real/israte"));
      
      
    }
}
  

void 
SoundFileSink::myProcess(realvec& in, realvec& out)
{

	mrs_bool paused = getctrl("mrs_bool/pause")->to<mrs_bool>();

  if ((dest_ != NULL) &&  (paused==false))
    {
      dest_->process(in,out);
    }
 

}









void 
SoundFileSink::putFloat(mrs_natural c, realvec& win)
{
	(void) c;
	(void) win;
  MRSWARN("SoundFileSink::putFloat not implemented");
}




	
