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

#include "SoundFileSource2.h"

using namespace std;
using namespace Marsyas;

SoundFileSource2::SoundFileSource2(string name):MarSystem("SoundFileSource2",name)
{
	src_ = NULL;
	//filename_ = "defaultfile";

	addControls();

	//create a default file source object
	fileReady(false);
}

SoundFileSource2::~SoundFileSource2()
{
	delete src_;
}

SoundFileSource2::SoundFileSource2(const SoundFileSource2& a):MarSystem(a)//[!]
{
	src_ = new AbsSoundFileSource2("AbsSoundFileSource2", name_);
	filename_ = "defaultfile"; 
}

MarSystem* 
SoundFileSource2::clone() const
{
	return new SoundFileSource2(*this);
}

void
SoundFileSource2::fileReady(bool ready)
{
	if(ready)
	{
		//...read and setup audio file at src_ level
		src_->updctrl("mrs_string/filename", getctrl("mrs_string/filename"));

		//check if audio file header was read  without errors
		//(if an error occurred, the filename is set to "defaultfile" by src_ )
		string filename = src_->getctrl("mrs_string/filename")->toString();
		if (filename == "defaultfile")
		{
			fileReady(false);
			return;
		}
		setctrl("mrs_string/filename", filename);
		filename_ = filename;

		setctrl("mrs_bool/notEmpty", src_->getctrl("mrs_bool/notEmpty"));

		//rewind 
		setctrl("mrs_natural/pos", 0);
	}
	else
	{
		//set audio file object to a dummy audio file reader
		delete src_;
		src_ = new AbsSoundFileSource2("AbsSoundFileSource2", name_);

		//...set controls to a default state
		setctrl("mrs_string/filename", "defaultfile");
		filename_ = "defaultfile";

		setctrl("mrs_natural/nChannels", (mrs_natural)1);
		setctrl("mrs_real/israte", MRS_DEFAULT_SLICE_SRATE);
		setctrl("mrs_bool/notEmpty", false);

 		//rewind
		setctrl("mrs_natural/pos", 0);
	}
}

void
SoundFileSource2::addControls()
{
	//read/write controls (controls with state)
	addctrl("mrs_string/filename", "defaultfile");
	setctrlState("mrs_string/filename", true);
	addctrl("mrs_natural/pos", (mrs_natural)0);
	setctrlState("mrs_natural/pos", true);

	//read-only controls (stateless)
	addctrl("mrs_bool/notEmpty", true); 
	addctrl("mrs_natural/nChannels",(mrs_natural)1);
	addctrl("mrs_natural/size", (mrs_natural)0);
}

void
SoundFileSource2::myUpdate(MarControlPtr sender)
{
	MRSDIAG("SoundFileSource2::localUpdate");

	//if file name changed, open new audio file
	string filename = getctrl("mrs_string/filename")->toString();
	if (filename_ != filename)
	{
		//if valid audio file format, create corresponding file object (src_)...
		if (checkType() == true)
		{
			//read audio file header and configure objects accordingly
			fileReady(true);
		}
		else //if empty or invalid filename...
		{
			//...go to a default (dummy) state
			fileReady(false);
		}
	}

	//pass configuration to file source object (src_) and update it 
	src_->setctrl("mrs_natural/pos", getctrl("mrs_natural/pos"));
	src_->setctrl("mrs_bool/notEmpty", getctrl("mrs_bool/notEmpty"));
	//avoid calling src_->update unless it's really necessary
	if(getctrl("mrs_natural/inSamples") != src_->getctrl("mrs_natural/inSamples") ||
		 getctrl("mrs_natural/inObservations") != src_->getctrl("mrs_natural/inObservations"))
	{
		src_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
		src_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
		src_->update();
	}

	//sync controls with file source object 
	setctrl("mrs_natural/onSamples", src_->getctrl("mrs_natural/onSamples"));
	setctrl("mrs_natural/onObservations", src_->getctrl("mrs_natural/onObservations"));
	//setctrl("mrs_real/israte", src_->getctrl("mrs_real/israte"));
	setctrl("mrs_real/osrate", src_->getctrl("mrs_real/osrate"));
	setctrl("mrs_string/onObsNames", src_->getctrl("mrs_string/onObsNames"));
	
	setctrl("mrs_natural/nChannels", src_->getctrl("mrs_natural/nChannels"));
	setctrl("mrs_bool/notEmpty", src_->getctrl("mrs_bool/notEmpty"));
	setctrl("mrs_natural/size", src_->getctrl("mrs_natural/size"));
	setctrl("mrs_natural/pos", src_->getctrl("mrs_natural/pos"));
}

bool 
SoundFileSource2::checkType()
{	
	string filename = getctrl("mrs_string/filename")->toString();

	if (filename == "defaultfile")
	{
		MRSWARN("SoundFileSource2::checkType: empty filename");
		return false;
	}
			
	//check if file exists
	FILE * sfp = fopen(filename.c_str(), "r");
	if (sfp == NULL) 
	{
		string wrn = "SoundFileSource2::checkType: Problem opening file ";
		wrn += filename;
		MRSWARN(wrn);
		return false;
	}
	fclose(sfp);

	// try to open file with appropriate format 
	string::size_type pos = filename.rfind(".", filename.length());
	string ext;

	if (pos == string::npos) 
		ext = "";
	else 
		ext = filename.substr(pos, filename.length());  
/*if (ext == ".au")
	{
		delete src_;
		src_ = new AuFileSource(name_);
	}
	else
*/ 
	if (ext == ".wav")
	{
		delete src_;
		src_ = new WavFileSource2(name_);
	}
/*
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
*/
	else 
	{
		if (filename != "defaultfile")
		{
			string wrn = "Unsupported format for file ";
			wrn += filename;
			MRSWARN(wrn);
			return false;
		}
		else 
			return false;
	}
	return true;
}


realvec&
SoundFileSource2::getAudioRegion(mrs_natural startSample, mrs_natural endSample)
{
		return src_->getAudioRegion(startSample, endSample);
}

void
SoundFileSource2::myProcess(realvec& in, realvec &out)
{
	//read audio data from audio file
	src_->process(in,out);

  //sync play controls (no update needed!)
	setctrl("mrs_natural/pos", src_->getctrl("mrs_natural/pos"));
	setctrl("mrs_bool/notEmpty", src_->getctrl("mrs_bool/notEmpty"));

	//if muted, replace output data with silence
	if(getctrl("mrs_bool/mute")->toBool())
		out.setval(0.0); 
}

