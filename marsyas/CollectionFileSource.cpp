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


CollectionFileSource::CollectionFileSource(string name)
{
  type_ = "SoundFileSource";
  name_ = name;

  addControls();
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
  addDefaultControls();
  addctrl("natural/nChannels",(natural)1);
  addctrl("bool/notEmpty", true);  
  notEmpty_ = true;
  addctrl("natural/pos", (natural)0);
  setctrlState("natural/pos", true);
  
  addctrl("natural/loopPos", (natural)0);
  setctrlState("natural/pos", true);  

  addctrl("string/filename", "daufile");
  setctrlState("string/filename", true);
  addctrl("natural/size", (natural)0);
  addctrl("string/filetype", "mf");
  addctrl("natural/cindex", 0);
  setctrlState("natural/cindex", true);


  addctrl("string/allfilenames", "collectionFileSource");
  setctrlState("string/allfilenames", true);
  addctrl("natural/numFiles", 0);  

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
  
  mngCreated_ = false;
  
}

void 
CollectionFileSource::getHeader(string filename)
{
  
  col_.read(filename);
  updctrl("string/allfilenames", col_.toLongString());
  updctrl("natural/numFiles", col_.getSize());  

  cindex_ = 0;
  setctrl("natural/cindex", (natural)0);
  setctrl("bool/notEmpty", (MarControlValue)true);
  addctrl("natural/size", (natural)1); // just so it's not zero 
  setctrl("natural/pos", (natural)0);
  pos_ = 0;
}


void
CollectionFileSource::update()
{
  
  nChannels_ = getctrl("natural/nChannels").toNatural();  
  inSamples_ = getctrl("natural/inSamples").toNatural();
  inObservations_ = getctrl("natural/inObservations").toNatural();
  

  nChannels_ = getctrl("natural/nChannels").toNatural();
  filename_ = getctrl("string/filename").toString();    
  pos_ = getctrl("natural/pos").toNatural();
  
  if (mngCreated_ == false) 
    {
      isrc_ = new SoundFileSource("isrc");
      mngCreated_ = true;
      downsampler_ = new DownSampler("downsampler_"); 
    }
  
  repetitions_ = getctrl("real/repetitions").toReal();
  duration_ = getctrl("real/duration").toReal();
  advance_ = getctrl("bool/advance").toBool();
  cindex_ = getctrl("natural/cindex").toNatural();

  if (getctrl("bool/shuffle").toBool())
    {
      cout << "SHUFFLING" << endl;
      col_.shuffle();
      setctrl("bool/shuffle", (MarControlValue)false);
    }
  
     
  
  
  if (cindex_ < col_.size()) 
    {
      isrc_->updctrl("string/filename", col_.entry(cindex_));
      setctrl("string/currentlyPlaying", col_.entry(cindex_));
    }
  

  israte_ = isrc_->getctrl("real/israte").toReal();
  setctrl("real/israte", israte_);
  setctrl("real/osrate", israte_);



  if (israte_ == 44100.0) 
    {
      downsampler_->updctrl("natural/inSamples", 2* inSamples_);
      setctrl("natural/onSamples", inSamples_);
      setctrl("real/israte", israte_/2);
      setctrl("real/osrate", israte_/2);
      temp_.create(inObservations_, 2 * inSamples_);
      tempi_.create(inObservations_, 2 * inSamples_);
      isrc_->updctrl("natural/inSamples", 2 * inSamples_);
    }
  else
    {
      isrc_->updctrl("natural/inSamples", inSamples_);
      setctrl("natural/onSamples", inSamples_);
      setctrl("real/israte", israte_);
      setctrl("real/osrate", israte_);
      temp_.create(inObservations_, inSamples_);
    }
  

  setctrl("natural/onObservations", inObservations_);

  


  isrc_->updctrl("natural/inObservations", inObservations_);
  isrc_->updctrl("real/repetitions", repetitions_);
  isrc_->updctrl("natural/pos", pos_);
  
  isrc_->updctrl("real/duration", duration_);
  isrc_->updctrl("bool/advance", advance_);
  isrc_->updctrl("natural/cindex", cindex_);
  
  cindex_ = getctrl("natural/cindex").toNatural();  

  defaultUpdate();
}


void
CollectionFileSource::process(realvec& in, realvec &out)
{

  checkFlow(in,out);
  
  if (advance_) 
    {
      cindex_ = cindex_ + 1;
      if (cindex_ >= col_.size() -1)  
	{
	  
	  setctrl("bool/notEmpty", (MarControlValue)false);
	  notEmpty_ = false;      
	  advance_ = false;
	}

       setctrl("natural/cindex", cindex_);

       isrc_->updctrl("string/filename", col_.entry(cindex_));   
       updctrl("natural/pos", isrc_->getctrl("natural/pos"));   
       israte_ = isrc_->getctrl("real/israte").toReal();

       setctrl("real/israte", israte_);
       setctrl("real/osrate", israte_);

       if (israte_ == 44100)
	 {
	   isrc_->process(tempi_,temp_);
	   setctrl("natural/pos", isrc_->getctrl("natural/pos"));
	   setctrl("bool/notEmpty", (MarControlValue)isrc_->getctrl("bool/notEmpty"));
	   downsampler_->process(temp_,out);
	 }
       else 
	 {
	   isrc_->process(in,out);
	   setctrl("natural/pos", isrc_->getctrl("natural/pos"));
	   setctrl("bool/notEmpty", (MarControlValue)isrc_->getctrl("bool/notEmpty"));
	 }
       update();      

       return;
     }
   else
     {
       if (israte_ == 44100)
	 {
	   isrc_->process(tempi_,temp_);
	   setctrl("natural/pos", isrc_->getctrl("natural/pos"));
	   setctrl("bool/notEmpty", (MarControlValue)isrc_->getctrl("bool/notEmpty"));
	   downsampler_->process(temp_,out);
	 }
       else 
	 {
	   isrc_->process(in,out);
	   setctrl("natural/pos", isrc_->getctrl("natural/pos"));
	   setctrl("bool/notEmpty", (MarControlValue)isrc_->getctrl("bool/notEmpty"));
	 }


       if (isrc_->getctrl("bool/notEmpty").toBool() == false)
	 {
	   if (cindex_ < col_.size() -1)
	     {
	       cindex_ = cindex_ + 1;
	       setctrl("natural/cindex", cindex_);
	       isrc_->updctrl("string/filename", col_.entry(cindex_));      
	       isrc_->updctrl("natural/pos", 0);     
	       pos_ = 0;
	       israte_ = isrc_->getctrl("real/israte").toReal();
	       setctrl("real/israte", israte_);
	       setctrl("real/osrate", israte_);
	     }
	  else 
	    {
	      setctrl("bool/notEmpty", (MarControlValue)false);
	      notEmpty_ = false;
	    }
	}
    }
  
}  



  
  


	
