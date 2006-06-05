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
  addctrl("natural/nChannels",(natural)1);
  addctrl("bool/notEmpty", true);  
  addctrl("natural/pos", (natural)0);
  setctrlState("natural/pos", true);
  

  addctrl("natural/loopPos", (natural)0);
  setctrlState("natural/loopPos", true);

  addctrl("string/filename", "defaultfile");
  setctrlState("string/filename", true);
  addctrl("string/allfilenames", ",");
  setctrlState("string/allfilenames",true);
  addctrl("natural/numFiles", 0);
  
  
  addctrl("natural/size", (natural)0);
  addctrl("real/frequency", 0.0);
  setctrlState("real/frequency",true);
  addctrl("bool/noteon", false);
  setctrlState("bool/noteon", true);
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
  
  addctrl("string/currentlyPlaying", "daufile");
}


void
SoundFileSource::update()
{

  setctrl("string/onObsNames", "audio,");
  setctrl("string/inObsNames", "audio,");
  
  
  MRSDIAG("SoundFileSource::update");
  
  
  if (filename_ != getctrl("string/filename").toString())
    {
      if (checkType() == true)
	{
	  getHeader();
	  filename_ = getctrl("string/filename").toString();
	  
	  setctrl("natural/nChannels", src_->getctrl("natural/nChannels"));
	  setctrl("real/israte", src_->getctrl("real/israte"));
	  setctrl("real/osrate", src_->getctrl("real/israte"));
	  
	  setctrl("bool/notEmpty", (MarControlValue)true);

	  
	  if (src_->getctrl("natural/size").toNatural() != 0)
	    src_->notEmpty_ = true;
	}
      else
	{
	  setctrl("natural/nChannels", (natural)1);
	  setctrl("real/israte", (real)22050.0);
	  setctrl("bool/notEmpty", (MarControlValue)false);
	  src_ = NULL;
	}
    }
  
  
  if (src_ != NULL) 
    {
      src_->setctrl("natural/inSamples", getctrl("natural/inSamples"));
      src_->setctrl("natural/inObservations", getctrl("natural/inObservations"));
      src_->setctrl("real/repetitions", getctrl("real/repetitions"));
      src_->setctrl("real/duration", getctrl("real/duration"));
      src_->setctrl("bool/advance", getctrl("bool/advance"));
      src_->setctrl("natural/cindex", getctrl("natural/cindex"));
      src_->setctrl("bool/shuffle", getctrl("bool/shuffle"));
      
      advance_ = getctrl("bool/advance").toBool();
      cindex_ = getctrl("natural/cindex").toNatural();
      currentlyPlaying_ = getctrl("string/currentlyPlaying").toString();
      shuffle_ = getctrl("bool/shuffle").toBool();
      
      
      src_->setctrl("bool/notEmpty", getctrl("bool/notEmpty"));
      src_->setctrl("natural/pos", getctrl("natural/pos"));
      src_->pos_ = getctrl("natural/pos").toNatural();
      

      src_->setctrl("natural/loopPos", getctrl("natural/loopPos"));
      src_->rewindpos_ = getctrl("natural/loopPos").toNatural();

      
      src_->update();

      setctrl("natural/onSamples", src_->getctrl("natural/onSamples"));
      setctrl("natural/onObservations", src_->getctrl("natural/onObservations"));
      setctrl("real/osrate", src_->getctrl("real/israte"));
      setctrl("natural/pos", src_->pos_);
      setctrl("natural/loopPos", src_->rewindpos_);
      
      setctrl("bool/notEmpty", (MarControlValue)src_->notEmpty_);
      setctrl("natural/size", src_->getctrl("natural/size"));
      setctrl("real/repetitions", src_->getctrl("real/repetitions"));
      setctrl("real/duration", src_->getctrl("real/duration"));
      setctrl("bool/advance", src_->getctrl("bool/advance"));
      setctrl("bool/shuffle", src_->getctrl("bool/shuffle"));
      setctrl("natural/cindex", src_->getctrl("natural/cindex"));
      setctrl("string/currentlyPlaying", src_->getctrl("string/currentlyPlaying"));
      
	      

      setctrl("string/allfilenames", src_->getctrl("string/allfilenames"));
      setctrl("natural/numFiles", src_->getctrl("natural/numFiles"));
      


      
      
      
      if (src_->getctrl("string/filetype").toString() == "raw")
	{
	  setctrl("real/frequency", src_->getctrl("real/frequency"));
	  setctrl("bool/noteon", src_->getctrl("bool/noteon"));
	}

      

    }
  

  defaultUpdate();
  
}



bool 
SoundFileSource::checkType()
{
  string filename = getctrl("string/filename").toString();
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
	  setctrl("string/filename", "defaultfile");
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
  
  else 
    {
      
      if (filename != "defaultfile")
	{
	  string wrn = "Unsupported format for file ";
	  wrn += filename;
	  MRSWARN(wrn);
	  filename_ = "defaultfile";
	  setctrl("string/filename", "defaultfile");
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
  string filename = getctrl("string/filename").toString();
  

  src_->getHeader(filename);
  setctrl("natural/pos", (MarControlValue)0);
  setctrl("natural/loopPos", (MarControlValue)0);
  
}


void
SoundFileSource::process(realvec& in, realvec &out)
{
 
  if (src_ != NULL)
    {
      src_->process(in,out);

      if (mute_) 
	out.setval(0.0);      

      setctrl("natural/pos", src_->pos_);
      setctrl("natural/loopPos", src_->rewindpos_);
      setctrl("bool/notEmpty", (MarControlValue)src_->notEmpty_);
    }


  
  if (advance_) 
    {
      updctrl("bool/advance", (MarControlValue)false);
    }
  

}

