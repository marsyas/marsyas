/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.cmu.edu>
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

#include "AudioSink2.h"

using namespace std;
using namespace Marsyas;

AudioSink2::AudioSink2(string name):MarSystem("AudioSink2", name)
{
	bufferSize_ = 0;
	start_ = 0;
	end_ = 0;
	preservoirSize_ = 0;

	data_ = NULL;
#ifdef MARSYAS_AUDIOIO
	audio_ = NULL;
#endif 

	//isInitialized_ = false;
	//stopped_ = true;//lmartins

	addControls();
}

AudioSink2::~AudioSink2()
{
#ifdef MARSYAS_AUDIOIO
	delete audio_;
#endif 
	data_ = 0; // RtAudio deletes the buffer itself.
}

MarSystem* 
AudioSink2::clone() const
{
	return new AudioSink2(*this);
}

void 
AudioSink2::addControls()
{
	addctrl("mrs_bool/initialized", false);
	addctrl("mrs_bool/stopped", true);

	addctrl("mrs_natural/nChannels",1);
	setctrlState("mrs_natural/nChannels", true);

#ifdef __OS_MACOSX__
	addctrl("mrs_natural/bufferSize", 1024);
#else
	addctrl("mrs_natural/bufferSize", 512);
#endif
	setctrlState("mrs_natural/bufferSize", true);
}

void 
AudioSink2::myUpdate(MarControlPtr sender)
{
	MRSDIAG("AudioSink2::localUpdate");

	//bypass audio from input to output
	/*
	setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));  
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	*/
	MarSystem::myUpdate(sender);

	rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
	srate_ = rtSrate_;

	nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();

	bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();

#ifdef __OS_MACOSX__
	if (rtSrate_ == 22050) 
	{
		rtSrate_ = 44100;
		bufferSize_ = 2 * bufferSize_;
	}
#endif	

	//setup RtAudio (may change bufferSize_ !!)
	initRtAudio(); 

	//Resize reservoir if necessary
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	if (inSamples_ < bufferSize_) 
		reservoirSize_ = 2 * bufferSize_;
	else 
		reservoirSize_ = 2 * inSamples_;

	if (reservoirSize_ > preservoirSize_)
		reservoir_.stretch(reservoirSize_);

	preservoirSize_ = reservoirSize_;

#ifdef MARSYAS_AUDIOIO
	//if audio was playing (i.e.this was a re-init), keep it playing
	//if (!stopped_ && audio_)
	if(!getctrl("mrs_bool/stopped")->to<mrs_bool>() && getctrl("mrs_bool/initialized")->to<mrs_bool>())//thread safe!
		audio_->startStream();
#endif //MARSYAS_AUDIOIO
}

void 
AudioSink2::initRtAudio()
{
#ifdef MARSYAS_AUDIOIO
	//marsyas represents audio data as float numbers
	RtAudio3Format rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

	// hardwire channels to stereo playback even for mono
	int rtChannels = 2;

	//create new RtAudio object (delete any existing one)
	delete audio_;
	try 
	{
		audio_ = new RtAudio3(0, rtChannels, 0, 0, rtFormat,
			rtSrate_, &bufferSize_, 4);

		data_ = (mrs_real *) audio_->getStreamBuffer();
	}
	catch (RtError3 &error) 
	{
		error.printMessage();
	}

	//update bufferSize control which may have been changed
	//by RtAudio (see RtAudio documentation)
	setctrl("mrs_natural/bufferSize", (mrs_natural)bufferSize_);

	//isInitialized_ = true;
	setctrl("mrs_bool/initialized", true);//thread safe!
#endif 
}

void 
AudioSink2::start()
{
#ifdef MARSYAS_AUDIOIO
	//if ( stopped_ )
	if(getctrl("mrs_bool/stopped")->to<mrs_bool>())//thread safe!
	  {
		audio_->startStream();
		//stopped_ = false;
		setctrl("mrs_bool/stopped", false);//thread safe!
	}
#endif 
}

void 
AudioSink2::stop()
{
#ifdef MARSYAS_AUDIOIO
	//if ( !stopped_ ) 
	if(!getctrl("mrs_bool/stopped")->to<mrs_bool>())//thread safe!
	{
	  audio_->stopStream();
	  //stopped_ = true;
	  setctrl("mrs_bool/stopped", true);//thread safe!
	}
#endif 
}

void
AudioSink2::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}

void 
AudioSink2::myProcess(realvec& in, realvec& out)
{
	checkFlow(in,out);

	// copy to output and into reservoir
	for (t=0; t < inSamples_; t++)
	{
		reservoir_(end_) = in(0,t);
		end_ = (end_ + 1) % reservoirSize_;

		for (o=0; o < inObservations_; o++)
			out(o,t) = in(o,t);
	}

	//check if RtAudio is initialized
	//if (!isInitialized_)
	if(!getctrl("mrs_bool/initialized")->to<mrs_bool>())//thread safe!
		return;

	//check MUTE
	if(getctrl("mrs_bool/mute")->to<mrs_bool>())//thread safe!
		return;

	//assure that RtAudio thread is running
	//(this may be needed by if an explicit call to start()
	//is not done before ticking or calling process() )
	//if ( stopped_ )
	if(getctrl("mrs_bool/stopped")->to<mrs_bool>())//thread safe!
	{
		start();
	}

	//update reservoir pointers 
	rsize_ = bufferSize_;
#ifdef __OS_MACOSX__ 
	if (srate_ == 22050)
		rsize_ = bufferSize_/2;		// upsample to 44100
	else 
		rsize_ = bufferSize_;
#endif 

	if (end_ >= start_) 
		diff_ = end_ - start_;
	else 
		diff_ = reservoirSize_ - (start_ - end_);

	//send audio data in reservoir to RtAudio
	while (diff_ >= rsize_)  
	{
		for (t=0; t < rsize_; t++) 
		{
#ifndef __OS_MACOSX__
			data_[2*t] = reservoir_((start_+t)%reservoirSize_);
			data_[2*t+1] = reservoir_((start_+t)%reservoirSize_);
#else
			if (srate_ == 22050)
			{
				data_[4*t] = reservoir_((start_+t) % reservoirSize_);
				data_[4*t+1] = reservoir_((start_+t)%reservoirSize_);
				data_[4*t+2] = reservoir_((start_+t) % reservoirSize_);
				data_[4*t+3] = reservoir_((start_+t) % reservoirSize_);
			}
			else
			{
				data_[2*t] = reservoir_((start_+t)%reservoirSize_);
				data_[2*t+1] = reservoir_((start_+t)%reservoirSize_);
			}
#endif 
		}

#ifdef MARSYAS_AUDIOIO
		//tick RtAudio
		try 
		{
		  audio_->tickStream();
		}
		catch (RtError3 &error) 
		{
			error.printMessage();
		}
#endif 
		//update reservoir pointers
		start_ = (start_ + rsize_) % reservoirSize_;
		if (end_ >= start_) 
			diff_ = end_ - start_;
		else 
			diff_ = reservoirSize_ - (start_ - end_);
	}
}












