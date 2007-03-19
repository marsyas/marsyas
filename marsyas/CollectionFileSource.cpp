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

/**
   \class CollectionFileSource
   \brief CollectionFileSource abstracts a collections of soundfiles as a SoundFileSource. 
   
   CollectionFileSource reads in a collection file consisting of a list of 
SoundFiles. The soundfiles are played one after the other with specifics 
such as offsets,durations as a single SoundFileSource. 
*/

#include "CollectionFileSource.h"

using namespace std;
using namespace Marsyas;

CollectionFileSource::CollectionFileSource(string name):AbsSoundFileSource("SoundFileSource", name)
{
  //type_ = "SoundFileSource";
  //name_ = name;

	addControls();
}

CollectionFileSource::CollectionFileSource(const CollectionFileSource& a):AbsSoundFileSource(a)
{
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
}


CollectionFileSource::~CollectionFileSource()
{
  delete isrc_;
  delete downsampler_;
}

MarSystem* 
CollectionFileSource::clone() const
{
  return new CollectionFileSource(*this);
}

void
CollectionFileSource::addControls()
{
  addctrl("mrs_natural/nChannels",(mrs_natural)1);
  addctrl("mrs_bool/notEmpty", true);  
  notEmpty_ = true;
  addctrl("mrs_natural/pos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);
  
  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);  

  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/size", (mrs_natural)0);
  addctrl("mrs_string/filetype", "mf");
  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", "collectionFileSource");
  setctrlState("mrs_string/allfilenames", true);
  addctrl("mrs_natural/numFiles", 0);  

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

  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
  
  mngCreated_ = false; 
}

void 
CollectionFileSource::getHeader(string filename)
{
	col_.read(filename);
  updctrl("mrs_string/allfilenames", col_.toLongString());
  updctrl("mrs_natural/numFiles", col_.getSize());  

  cindex_ = 0;
  setctrl("mrs_natural/cindex", 0);
  setctrl("mrs_bool/notEmpty", true);
  addctrl("mrs_natural/size", 1); // just so it's not zero 
  setctrl("mrs_natural/pos", 0);
  pos_ = 0;
}

void
CollectionFileSource::myUpdate(MarControlPtr sender)
{
	nChannels_ = getctrl("mrs_natural/nChannels")->toNatural();  
  inSamples_ = getctrl("mrs_natural/inSamples")->toNatural();
  inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
  
  nChannels_ = getctrl("mrs_natural/nChannels")->toNatural();
  filename_ = getctrl("mrs_string/filename")->toString();    
  pos_ = getctrl("mrs_natural/pos")->toNatural();
  
  if (mngCreated_ == false) 
  {
    isrc_ = new SoundFileSource("isrc");
    mngCreated_ = true;
    downsampler_ = new DownSampler("downsampler_"); 
  }
  
  repetitions_ = getctrl("mrs_real/repetitions")->toReal();
  duration_ = getctrl("mrs_real/duration")->toReal();
  advance_ = getctrl("mrs_bool/advance")->toBool();
  cindex_ = getctrl("mrs_natural/cindex")->toNatural();

  if (getctrl("mrs_bool/shuffle")->isTrue())
  {
    cout << "SHUFFLING" << endl;
    col_.shuffle();
    setctrl("mrs_bool/shuffle", false);
  }
  
  if (cindex_ < col_.size()) 
  {
    isrc_->updctrl("mrs_string/filename", col_.entry(cindex_));
    setctrl("mrs_string/currentlyPlaying", col_.entry(cindex_));
    ctrl_currentlyPlaying_->setValue(col_.entry(cindex_), NOUPDATE);
  }
  
  myIsrate_ = isrc_->getctrl("mrs_real/israte")->toReal();
  setctrl("mrs_real/israte", myIsrate_);
  setctrl("mrs_real/osrate", myIsrate_);

  if (myIsrate_ == 44100.0) 
  {
    downsampler_->updctrl("mrs_natural/inSamples", 2* inSamples_);
    setctrl("mrs_natural/onSamples", inSamples_);
    setctrl("mrs_real/israte", myIsrate_/2.0);
    setctrl("mrs_real/osrate", myIsrate_/2.0);
    temp_.create(inObservations_, 2 * inSamples_);
    tempi_.create(inObservations_, 2 * inSamples_);
    isrc_->updctrl("mrs_natural/inSamples", 2 * inSamples_);
  }
  else
  {
    isrc_->updctrl("mrs_natural/inSamples", inSamples_);
    setctrl("mrs_natural/onSamples", inSamples_);
    setctrl("mrs_real/israte", myIsrate_);
    setctrl("mrs_real/osrate", myIsrate_);
    temp_.create(inObservations_, inSamples_);
  }
  
	setctrl("mrs_natural/onObservations", inObservations_);

  isrc_->updctrl("mrs_natural/inObservations", inObservations_);
  isrc_->updctrl("mrs_real/repetitions", repetitions_);
  isrc_->updctrl("mrs_natural/pos", pos_);
  
  isrc_->updctrl("mrs_real/duration", duration_);
  isrc_->updctrl("mrs_bool/advance", advance_);
  isrc_->updctrl("mrs_natural/cindex", cindex_);
  
  cindex_ = getctrl("mrs_natural/cindex")->toNatural();  
}

void
CollectionFileSource::myProcess(realvec& in, realvec &out)
{
	//checkFlow(in,out);
  
  if (advance_) 
  {
    cindex_ = cindex_ + 1;
    if (cindex_ >= col_.size() -1)  
		{
			setctrl("mrs_bool/notEmpty", false);
			notEmpty_ = false;      
			advance_ = false;
		}

		setctrl("mrs_natural/cindex", cindex_);

		isrc_->updctrl("mrs_string/filename", col_.entry(cindex_));   
		updctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));   
		myIsrate_ = isrc_->getctrl("mrs_real/israte")->toReal();

		setctrl("mrs_real/israte", myIsrate_);
		setctrl("mrs_real/osrate", myIsrate_);

		if (myIsrate_ == 44100.0)
		{
		 isrc_->process(tempi_,temp_);
		 setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
		 setctrl("mrs_bool/notEmpty", isrc_->getctrl("mrs_bool/notEmpty"));
		 downsampler_->process(temp_,out);
		}
		else 
		{
		 isrc_->process(in,out);
		 setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
		 setctrl("mrs_bool/notEmpty", isrc_->getctrl("mrs_bool/notEmpty"));
		}
    
		update();      
		return;
  }
  
  else
  {
    if (myIsrate_ == 44100.0)
		{
			isrc_->process(tempi_,temp_);
			setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
			setctrl("mrs_bool/notEmpty", isrc_->getctrl("mrs_bool/notEmpty"));
			downsampler_->process(temp_,out);
		}
    else 
		{
			isrc_->process(in,out);
			setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
			setctrl("mrs_bool/notEmpty", isrc_->getctrl("mrs_bool/notEmpty"));
		}

    if (!isrc_->getctrl("mrs_bool/notEmpty")->isTrue())
		{
		  cout << "SWITCHING " << endl;
		  
		  if (cindex_ < col_.size() -1)
		    {
		      cindex_ = cindex_ + 1;
		      setctrl("mrs_natural/cindex", cindex_);
		      isrc_->updctrl("mrs_string/filename", col_.entry(cindex_));      
		      isrc_->updctrl("mrs_natural/pos", 0);     
		      pos_ = 0;
		      myIsrate_ = isrc_->getctrl("mrs_real/israte")->toReal();
		      setctrl("mrs_real/israte", myIsrate_);
		      setctrl("mrs_real/osrate", myIsrate_);
		      setctrl("mrs_string/currentlyPlaying", col_.entry(cindex_));
		      ctrl_currentlyPlaying_->setValue(col_.entry(cindex_), NOUPDATE);
		      
		      cout << "Just updated currentlyPlaying to" << col_.entry(cindex_) << endl;
		      
		    }
		  else 
		    {
		      setctrl("mrs_bool/notEmpty", false);
		      notEmpty_ = false;
		    }
		}
  } 
}  



  
  


	
