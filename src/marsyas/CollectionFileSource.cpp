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

#include "CollectionFileSource.h"

using namespace std;
using namespace Marsyas;

CollectionFileSource::CollectionFileSource(string name):AbsSoundFileSource("SoundFileSource", name)
{
	addControls();
}

CollectionFileSource::CollectionFileSource(const CollectionFileSource& a):AbsSoundFileSource(a)
{
	ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
	ctrl_currentLabel_ = getctrl("mrs_natural/currentLabel");
	ctrl_labelNames_ = getctrl("mrs_string/labelNames");
	ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
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
	addctrl("mrs_bool/hasData", true);  
	hasData_ = true;
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

	addctrl("mrs_natural/advance", 0);
	setctrlState("mrs_natural/advance", true);

	addctrl("mrs_bool/shuffle", false);
	setctrlState("mrs_bool/shuffle", true);

	addctrl("mrs_natural/cindex", 0);
	setctrlState("mrs_natural/cindex", true);

	addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
	addctrl("mrs_natural/currentLabel", 0, ctrl_currentLabel_);
	addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
	addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
	mngCreated_ = false; 
}

void 
CollectionFileSource::getHeader(string filename)
{
	col_.clear();
	col_.read(filename);
	updControl("mrs_string/allfilenames", col_.toLongString());
	updControl("mrs_natural/numFiles", (mrs_natural)col_.getSize());  

	cindex_ = 0;
	setctrl("mrs_natural/cindex", 0);
	setctrl("mrs_bool/hasData", true);
	ctrl_currentlyPlaying_->setValue(col_.entry(0), NOUPDATE);

	

	if (col_.hasLabels())
	{
		ctrl_currentLabel_->setValue(col_.labelNum(col_.labelEntry(0)), NOUPDATE);
		ctrl_labelNames_->setValue(col_.getLabelNames(), NOUPDATE);
		ctrl_nLabels_->setValue(col_.getNumLabels(), NOUPDATE);
	}

	

	addctrl("mrs_natural/size", 1); // just so it's not zero 
	setctrl("mrs_natural/pos", 0);
	pos_ = 0;
}

void
CollectionFileSource::myUpdate(MarControlPtr sender)
{



	
	(void) sender;


	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

	filename_ = getctrl("mrs_string/filename")->to<mrs_string>();    
	pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();

	if (mngCreated_ == false) 
	{
		isrc_ = new SoundFileSource("isrc");
		mngCreated_ = true;
		downsampler_ = new DownSampler("downsampler_"); 
	}

	repetitions_ = getctrl("mrs_real/repetitions")->to<mrs_real>();
	duration_ = getctrl("mrs_real/duration")->to<mrs_real>();
	advance_ = getctrl("mrs_natural/advance")->to<mrs_natural>();
	cindex_ = getctrl("mrs_natural/cindex")->to<mrs_natural>();

	if (getctrl("mrs_bool/shuffle")->isTrue())
	{
		col_.shuffle();
		setctrl("mrs_bool/shuffle", false);
	}

	if (cindex_ < (mrs_natural)col_.size()) 
	{
		isrc_->updControl("mrs_string/filename", col_.entry(cindex_));
		isrc_->updControl("mrs_natural/pos", 0);
		ctrl_currentlyPlaying_->setValue(col_.entry(cindex_), NOUPDATE);

	if (col_.hasLabels())
		ctrl_currentLabel_->setValue(col_.labelNum(col_.labelEntry(cindex_)), NOUPDATE);
		ctrl_labelNames_->setValue(col_.getLabelNames(), NOUPDATE);
		ctrl_nLabels_->setValue(col_.getNumLabels(), NOUPDATE);
	}

	myIsrate_ = isrc_->getctrl("mrs_real/israte")->to<mrs_real>();//[!] why get an INPUT flow control?!
	onObservations_ = isrc_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

	setctrl("mrs_real/israte", myIsrate_);//[!] why set an INPUT flow control?!?
	setctrl("mrs_real/osrate", myIsrate_);
	setctrl("mrs_natural/onObservations", onObservations_);

	isrc_->updControl("mrs_natural/inSamples", inSamples_);
	setctrl("mrs_natural/onSamples", inSamples_);
	setctrl("mrs_real/israte", myIsrate_);//[!] why set an INPUT flow control?!?
	setctrl("mrs_real/osrate", myIsrate_);
	setctrl("mrs_natural/onObservations", onObservations_);
	temp_.create(inObservations_, inSamples_);

	isrc_->updControl("mrs_real/repetitions", repetitions_);
	isrc_->updControl("mrs_natural/pos", pos_);
	isrc_->updControl("mrs_real/duration", duration_);
	isrc_->updControl("mrs_natural/advance", advance_);
	isrc_->updControl("mrs_natural/cindex", cindex_);

	cindex_ = getctrl("mrs_natural/cindex")->to<mrs_natural>();  

	
	if (advance_)
	{
		
		setctrl("mrs_string/currentlyPlaying", col_.entry((cindex_+advance_) % col_.size()));

	if (col_.hasLabels())
	{
		setctrl("mrs_natural/currentLabel", col_.labelNum(col_.labelEntry((cindex_+advance_) % col_.size())));
		ctrl_currentLabel_->setValue(col_.labelNum(col_.labelEntry((cindex_+advance_) % col_.size())), NOUPDATE);
	}
	
		if (cindex_ + advance_ >= (mrs_natural)col_.size())
		{
			setctrl("mrs_bool/hasData", false);
			hasData_ = false;      
			advance_ = 0;
			cindex_ = 0;
		}
	}
	


}

void
CollectionFileSource::myProcess(realvec& in, realvec &out)
{
	if (advance_) 
	{
		cindex_ = (cindex_ + advance_) % col_.size();
		
		setctrl("mrs_natural/cindex", cindex_);
		isrc_->updControl("mrs_string/filename", col_.entry(cindex_));   
		isrc_->updControl("mrs_natural/pos", 0);

		updControl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));   
		myIsrate_ = isrc_->getctrl("mrs_real/israte")->to<mrs_real>();
		onObservations_ = isrc_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

		setctrl("mrs_real/israte", myIsrate_);
		setctrl("mrs_real/osrate", myIsrate_);
		setctrl("mrs_natural/onObservations", onObservations_);
		setctrl("mrs_string/currentlyPlaying", col_.entry(cindex_));
		ctrl_currentlyPlaying_->setValue(col_.entry(cindex_), NOUPDATE);
		
		if (col_.hasLabels())
		{
			
		setctrl("mrs_natural/currentLabel", col_.labelNum(col_.labelEntry(cindex_)));
		ctrl_currentLabel_->setValue(col_.labelNum(col_.labelEntry(cindex_)), NOUPDATE);
		}
		
		ctrl_labelNames_->setValue(col_.getLabelNames(), NOUPDATE);
		ctrl_nLabels_->setValue(col_.getNumLabels(), NOUPDATE);
		
		update();   

		isrc_->process(in,out);
		setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
		setctrl("mrs_bool/hasData", isrc_->getctrl("mrs_bool/hasData"));

 		if (cindex_ > (mrs_natural)col_.size()-2)  
 		{
			setctrl("mrs_bool/hasData", false);
			hasData_ = false;      
			advance_ = 0;
 			return;
 		}

		
		setctrl("mrs_natural/advance", 0);
		advance_ = 0;
		
		return;
	}
	else
	{
		//finished current file. Advance to next one in collection (if any)
		if (!isrc_->getctrl("mrs_bool/hasData")->isTrue())
		{
			//check if there a following file ion the collection
			if (cindex_ < (mrs_natural)col_.size() -1)
			{
				cindex_ = cindex_ + 1;
				setctrl("mrs_natural/cindex", cindex_);
				
				isrc_->updControl("mrs_string/filename", col_.entry(cindex_));      
				isrc_->updControl("mrs_natural/pos", 0);     
				pos_ = 0;
				myIsrate_ = isrc_->getctrl("mrs_real/israte")->to<mrs_real>();
				onObservations_ = isrc_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

				setctrl("mrs_real/israte", myIsrate_);
				setctrl("mrs_real/osrate", myIsrate_);
				setctrl("mrs_natural/onObservations", onObservations_);
				setctrl("mrs_string/currentlyPlaying", col_.entry(cindex_));
				ctrl_currentlyPlaying_->setValue(col_.entry(cindex_), NOUPDATE);

		
				if (col_.hasLabels())
				{
					setctrl("mrs_natural/currentLabel", col_.labelNum(col_.labelEntry(cindex_)));
					ctrl_currentLabel_->setValue(col_.labelNum(col_.labelEntry(cindex_)), NOUPDATE);
				}
				
				ctrl_labelNames_->setValue(col_.getLabelNames(), NOUPDATE);
				ctrl_nLabels_->setValue(col_.getNumLabels(), NOUPDATE);
			}
			else //no more files in collection
			{
				setctrl("mrs_bool/hasData", false);
				hasData_ = false;
			}
		}

		isrc_->process(in,out);
		setctrl("mrs_natural/pos", isrc_->getctrl("mrs_natural/pos"));
		setctrl("mrs_bool/hasData", isrc_->getctrl("mrs_bool/hasData"));
	} 


	
}  








