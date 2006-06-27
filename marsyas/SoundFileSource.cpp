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


/**
   \class SoundFileSource
   \brief Interface for sound file sources 
   
   Abstract base class for any type of sound file input.
Specific IO classes for various formats like AuFileSource 
are children of this class. 
*/

#include "SoundFileSource.h"

using namespace std;
using namespace Marsyas;

//#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
//#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)

SoundFileSource::SoundFileSource(string name)
{
  type_ = "SoundFileSource";
  name_ = name;
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  src_ = NULL;
  addControls();
}


SoundFileSource::~SoundFileSource()
{
  delete src_;
}



MarSystem* 
SoundFileSource::clone() const
{
  return new SoundFileSource(*this);
}



SoundFileSource::SoundFileSource(const SoundFileSource& a)
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

  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  src_ = NULL;
}



void 
SoundFileSource::setName(string name)
{
  name_ = name;
  ncontrols_.clear();
  addControls();
}




void
SoundFileSource::addControls()
{
  addDefaultControls();
  addctrl("mrs_natural/nChannels",(mrs_natural)1);
  addctrl("mrs_bool/notEmpty", true);  
  addctrl("mrs_natural/pos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);
  

  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/loopPos", true);

  addctrl("mrs_string/filename", "defaultfile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames",true);
  addctrl("mrs_natural/numFiles", 0);
  
  
  addctrl("mrs_natural/size", (mrs_natural)0);
  addctrl("mrs_real/frequency", 0.0);
  setctrlState("mrs_real/frequency",true);
  addctrl("mrs_bool/noteon", false);
  setctrlState("mrs_bool/noteon", true);
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
  
  addctrl("mrs_string/currentlyPlaying", "daufile");
}


void
SoundFileSource::update()
{

  setctrl("mrs_string/onObsNames", "audio,");
  setctrl("mrs_string/inObsNames", "audio,");
  
  
  MRSDIAG("SoundFileSource::update");
  
  
  if (filename_ != getctrl("mrs_string/filename").toString())
    {
      if (checkType() == true)
	{
	  getHeader();
	  filename_ = getctrl("mrs_string/filename").toString();
	  
	  setctrl("mrs_natural/nChannels", src_->getctrl("mrs_natural/nChannels"));
	  setctrl("mrs_real/israte", src_->getctrl("mrs_real/israte"));
	  setctrl("mrs_real/osrate", src_->getctrl("mrs_real/israte"));
	  
	  setctrl("mrs_bool/notEmpty", (MarControlValue)true);

	  
	  if (src_->getctrl("mrs_natural/size").toNatural() != 0)
	    src_->notEmpty_ = true;
	}
      else
	{
	  setctrl("mrs_natural/nChannels", (mrs_natural)1);
	  setctrl("mrs_real/israte", (mrs_real)22050.0);
	  setctrl("mrs_bool/notEmpty", (MarControlValue)false);
	  src_ = NULL;
	}
    }
  
  
  if (src_ != NULL) 
    {
      src_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
      src_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
      src_->setctrl("mrs_real/repetitions", getctrl("mrs_real/repetitions"));
      src_->setctrl("mrs_real/duration", getctrl("mrs_real/duration"));
      src_->setctrl("mrs_bool/advance", getctrl("mrs_bool/advance"));
      src_->setctrl("mrs_natural/cindex", getctrl("mrs_natural/cindex"));
      src_->setctrl("mrs_bool/shuffle", getctrl("mrs_bool/shuffle"));
      
      advance_ = getctrl("mrs_bool/advance").toBool();
      cindex_ = getctrl("mrs_natural/cindex").toNatural();
      currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying").toString();
      shuffle_ = getctrl("mrs_bool/shuffle").toBool();
      
      
      src_->setctrl("mrs_bool/notEmpty", getctrl("mrs_bool/notEmpty"));
      src_->setctrl("mrs_natural/pos", getctrl("mrs_natural/pos"));
      src_->pos_ = getctrl("mrs_natural/pos").toNatural();
      

      src_->setctrl("mrs_natural/loopPos", getctrl("mrs_natural/loopPos"));
      src_->rewindpos_ = getctrl("mrs_natural/loopPos").toNatural();

      
      src_->update();

      setctrl("mrs_natural/onSamples", src_->getctrl("mrs_natural/onSamples"));
      setctrl("mrs_natural/onObservations", src_->getctrl("mrs_natural/onObservations"));
      setctrl("mrs_real/osrate", src_->getctrl("mrs_real/israte"));
      setctrl("mrs_natural/pos", src_->pos_);
      setctrl("mrs_natural/loopPos", src_->rewindpos_);
      
      setctrl("mrs_bool/notEmpty", (MarControlValue)src_->notEmpty_);
      setctrl("mrs_natural/size", src_->getctrl("mrs_natural/size"));
      setctrl("mrs_real/repetitions", src_->getctrl("mrs_real/repetitions"));
      setctrl("mrs_real/duration", src_->getctrl("mrs_real/duration"));
      setctrl("mrs_bool/advance", src_->getctrl("mrs_bool/advance"));
      setctrl("mrs_bool/shuffle", src_->getctrl("mrs_bool/shuffle"));
      setctrl("mrs_natural/cindex", src_->getctrl("mrs_natural/cindex"));
      setctrl("mrs_string/currentlyPlaying", src_->getctrl("mrs_string/currentlyPlaying"));
      
	      

      setctrl("mrs_string/allfilenames", src_->getctrl("mrs_string/allfilenames"));
      setctrl("mrs_natural/numFiles", src_->getctrl("mrs_natural/numFiles"));
      


      
      
      
      if (src_->getctrl("mrs_string/filetype").toString() == "raw")
	{
	  setctrl("mrs_real/frequency", src_->getctrl("mrs_real/frequency"));
	  setctrl("mrs_bool/noteon", src_->getctrl("mrs_bool/noteon"));
	}

      

    }
  

  defaultUpdate();
  
}



bool 
SoundFileSource::checkType()
{
  string filename = getctrl("mrs_string/filename").toString();
  // check if file exists
  if (filename != "defaultfile")
    {
      sfp_ = fopen(filename.c_str(), "r");
      if (sfp_ == NULL) 
	{
	  string wrn = "SoundFileSource::Problem opening file ";
	  wrn += filename;
	  MRSWARN(wrn);
	  filename_ = "defaultfile";
	  setctrl("mrs_string/filename", "defaultfile");
	  return false;
	}
      fclose(sfp_);
    }
  else 
    filename_ = "defaultfile";
  
  
  
  
  
  // try to open file with appropriate format 
  string::size_type pos = filename.rfind(".", filename.length());
  string ext;

  if (pos == string::npos) ext = "";
  else 
    ext = filename.substr(pos, filename.length());  
  if (ext == ".au")
    {
      delete src_;
      src_ = new AuFileSource(name_);
    }
  else if (ext == ".wav")
    {
      delete src_;
      src_ = new WavFileSource(name_);
    }
  else if (ext == ".raw") 
    {
      delete src_;
      src_ = new RawFileSource(name_);
    }	
  else if (ext == ".mf") 
    {
      delete src_;
      src_ = new CollectionFileSource(name_);
    }

#ifdef MAD_MP3
  else if (ext == ".mp3")
    {
      delete src_;
      src_ = new MP3FileSource(name_);
    }
#endif 

#ifdef OGG_VORBIS
  else if (ext == ".ogg")
  {
	  cout << "OGG" << endl;
	  delete src_;
	  src_ = new OggFileSource(name_);
  }
#endif 
  
  else 
    {
      
      if (filename != "defaultfile")
	{
	  string wrn = "Unsupported format for file ";
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
SoundFileSource::getHeader()
{
  string filename = getctrl("mrs_string/filename").toString();
  

  src_->getHeader(filename);
  setctrl("mrs_natural/pos", (MarControlValue)0);
  setctrl("mrs_natural/loopPos", (MarControlValue)0);
  
}


void
SoundFileSource::process(realvec& in, realvec &out)
{
 
  if (src_ != NULL)
    {
      src_->process(in,out);

      if (mute_) 
	out.setval(0.0);      

      setctrl("mrs_natural/pos", src_->pos_);
      setctrl("mrs_natural/loopPos", src_->rewindpos_);
      setctrl("mrs_bool/notEmpty", (MarControlValue)src_->notEmpty_);
    }


  
  if (advance_) 
    {
      updctrl("mrs_bool/advance", (MarControlValue)false);
    }
  

}

