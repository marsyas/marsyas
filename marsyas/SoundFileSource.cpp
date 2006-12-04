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

SoundFileSource::SoundFileSource(string name):MarSystem("SoundFileSource",name)
{
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

SoundFileSource::SoundFileSource(const SoundFileSource& a):MarSystem(a)
{
	src_ = NULL;
}

void
SoundFileSource::addControls()
{
  addctrl("mrs_natural/nChannels", 1);
  addctrl("mrs_bool/notEmpty", true);  
  
  addctrl("mrs_natural/pos", 0);
  setctrlState("mrs_natural/pos", true);

  addctrl("mrs_natural/loopPos", 0);
  setctrlState("mrs_natural/loopPos", true);

  addctrl("mrs_string/filename", "defaultfile");
  setctrlState("mrs_string/filename", true);
  
	addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames",true);
  
	addctrl("mrs_natural/numFiles", 0);
  
	addctrl("mrs_natural/size", 0);
  
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
SoundFileSource::myUpdate()
{
	MRSDIAG("SoundFileSource::myUpdate");

	setctrl("mrs_string/onObsNames", "audio,");
  setctrl("mrs_string/inObsNames", "audio,");

  if (filename_ != getctrl("mrs_string/filename")->toString())
  {
    if (checkType() == true)
		{
			getHeader();
			filename_ = getctrl("mrs_string/filename")->toString();
		  
			setctrl("mrs_natural/nChannels", src_->getctrl("mrs_natural/nChannels"));
			setctrl("mrs_real/israte", src_->getctrl("mrs_real/israte"));
			setctrl("mrs_real/osrate", src_->getctrl("mrs_real/osrate"));
		  
			setctrl("mrs_bool/notEmpty", true);
		  
			if (src_->getctrl("mrs_natural/size")->toNatural() != 0)
				src_->notEmpty_ = true; //[!]
		}
    else
		{
			setctrl("mrs_natural/nChannels", 1);
			setctrl("mrs_real/israte", 22050.0);
			setctrl("mrs_bool/notEmpty", false);
			src_ = NULL;
		}
  }
  if (src_ != NULL) 
  {
		//pass configuration to audio source object and update it 
		src_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
    src_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
    src_->setctrl("mrs_real/repetitions", getctrl("mrs_real/repetitions"));
    src_->setctrl("mrs_real/duration", getctrl("mrs_real/duration"));
    src_->setctrl("mrs_bool/advance", getctrl("mrs_bool/advance"));
    src_->setctrl("mrs_natural/cindex", getctrl("mrs_natural/cindex"));
    src_->setctrl("mrs_bool/shuffle", getctrl("mrs_bool/shuffle"));
    src_->setctrl("mrs_bool/notEmpty", getctrl("mrs_bool/notEmpty"));
    src_->setctrl("mrs_natural/pos", getctrl("mrs_natural/pos"));
    src_->pos_ = getctrl("mrs_natural/pos")->toNatural();//[!]
    src_->setctrl("mrs_natural/loopPos", getctrl("mrs_natural/loopPos"));
    src_->rewindpos_ = getctrl("mrs_natural/loopPos")->toNatural();//[!]
 		src_->update();

    //sync local controls with the controls from the audio source object 
		setctrl("mrs_natural/onSamples", src_->getctrl("mrs_natural/onSamples"));
    setctrl("mrs_natural/onObservations", src_->getctrl("mrs_natural/onObservations"));
    
		mrs_real temp = src_->getctrl("mrs_real/osrate")->toReal();
		setctrl("mrs_real/osrate",temp ); //israte[?]

    setctrl("mrs_natural/pos", src_->pos_);//[!]
    setctrl("mrs_natural/loopPos", src_->rewindpos_);//[!]
    setctrl("mrs_bool/notEmpty", src_->notEmpty_);//[!]
    setctrl("mrs_natural/size", src_->getctrl("mrs_natural/size"));
    setctrl("mrs_real/repetitions", src_->getctrl("mrs_real/repetitions"));
    setctrl("mrs_real/duration", src_->getctrl("mrs_real/duration"));
    
		advance_ = getctrl("mrs_bool/advance")->toBool();//?!?!!? [!][?]
		setctrl("mrs_bool/advance", src_->getctrl("mrs_bool/advance"));
    
		setctrl("mrs_bool/shuffle", src_->getctrl("mrs_bool/shuffle"));
    setctrl("mrs_natural/cindex", src_->getctrl("mrs_natural/cindex"));
    setctrl("mrs_string/currentlyPlaying", src_->getctrl("mrs_string/currentlyPlaying"));
		setctrl("mrs_string/allfilenames", src_->getctrl("mrs_string/allfilenames"));
    setctrl("mrs_natural/numFiles", src_->getctrl("mrs_natural/numFiles"));
        
    if (src_->getctrl("mrs_string/filetype")->toString() == "raw")
		{
			setctrl("mrs_real/frequency", src_->getctrl("mrs_real/frequency"));
			setctrl("mrs_bool/noteon", src_->getctrl("mrs_bool/noteon"));
		}
  }  
}

bool 
SoundFileSource::checkType()
{
  string filename = getctrl("mrs_string/filename")->toString();
  // check if file exists
  if (filename != "defaultfile")
  {
    FILE * sfp = fopen(filename.c_str(), "r");
    if (sfp == NULL) 
		{
			string wrn = "SoundFileSource::Problem opening file ";
			wrn += filename;
			MRSWARN(wrn);
			filename_ = "defaultfile";
			setctrl("mrs_string/filename", "defaultfile");
			return false;
		}
    fclose(sfp);
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
    src_ = new AuFileSource(getName());
  }
  else if (ext == ".wav")
  {
    delete src_;
    src_ = new WavFileSource(getName());
  }
  else if (ext == ".raw") 
  {
    delete src_;
    src_ = new RawFileSource(getName());
  }	
  else if (ext == ".mf") 
  {
    delete src_;
    src_ = new CollectionFileSource(getName());
  }
	#ifdef MAD_MP3
  else if (ext == ".mp3")
    {
      delete src_;
      src_ = new MP3FileSource(getName());
    }
	#endif 
	#ifdef OGG_VORBIS
  else if (ext == ".ogg")
  {
	  cout << "OGG" << endl;
	  delete src_;
	  src_ = new OggFileSource(getName());
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
  string filename = getctrl("mrs_string/filename")->toString();
 
	src_->getHeader(filename);
	setctrl("mrs_natural/pos", 0);
	setctrl("mrs_natural/loopPos", 0); 
}

void
SoundFileSource::myProcess(realvec& in, realvec &out)
{
  if (src_ != NULL)
  {
    src_->process(in,out);

    if(getctrl("mrs_bool/mute")->isTrue())
      out.setval(0.0);      

    setctrl("mrs_natural/pos", src_->pos_); //[!]
    setctrl("mrs_natural/loopPos", src_->rewindpos_);//[!]
    setctrl("mrs_bool/notEmpty", src_->notEmpty_);//[!]
  }
  
  if (advance_) 
  {
    updctrl("mrs_bool/advance", false);
  }

	//MATLAB_PUT(out, "SoundFileSource_out");
	//MATLAB_EVAL("plot(SoundFileSource_out)");
}

