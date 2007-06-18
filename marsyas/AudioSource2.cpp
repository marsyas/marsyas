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

/**
\class AudioSource2
\ingroup Experimental
\brief Real-time audio source 

Real-time Audio Source based on RtAudio
*/

#include "AudioSource2.h"

using namespace std;
using namespace Marsyas;

AudioSource2::AudioSource2(string name):MarSystem("AudioSource2", name)
{
	data_ = NULL;
	audio_ = NULL;

	ri_ = 0;
	preservoirSize_ = 0;

	isInitialized_ = false;
	stopped_ = true;

	addControls();
}

AudioSource2::~AudioSource2()
{
	delete audio_;
	data_ = 0; // RtAudio deletes the buffer itself.
}


MarSystem* 
AudioSource2::clone() const
{
	return new AudioSource2(*this);
}

void 
AudioSource2::addControls()
{
	addctrl("mrs_natural/nChannels",1);
	setctrlState("mrs_natural/nChannels", true);

#ifdef MARSYAS_MACOSX
	addctrl("mrs_natural/bufferSize", 2048);
#else
	addctrl("mrs_natural/bufferSize", 256);
#endif
	setctrlState("mrs_natural/bufferSize", true);

	addctrl("mrs_real/gain", 1.0); // 1.05) [!];
}

void 
AudioSource2::myUpdate(MarControlPtr sender)
{
	MRSDIAG("AudioSource2::localUpdate");

	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	nChannels_ = getctrl("mrs_natural/nChannels")->toNatural();
	setctrl("mrs_natural/inObservations", nChannels_);
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));

	bufferSize_ = (mrs_natural)getctrl("mrs_natural/bufferSize")->toNatural();
#ifdef MARSYAS_MACOSX
	bufferSize_ = 8 * bufferSize_; //[!]
#endif	

	initRtAudio(); 

	//resize reservoir if necessary
	inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
	inSamples_ = getctrl("mrs_natural/inSamples")->toNatural();

	if (inSamples_ * inObservations_ < bufferSize_) 
		reservoirSize_ = 2 * inObservations_ * bufferSize_;
	else 
		reservoirSize_ = 2 * inSamples_ * inObservations_;

	if (reservoirSize_ > preservoirSize_)
	{
		reservoir_.stretch(reservoirSize_);
	}
	preservoirSize_ = reservoirSize_;
}

void 
AudioSource2::initRtAudio()
{
	//marsyas represents audio data as float numbers
	RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

	int rtSrate = (int)getctrl("mrs_real/israte")->toReal();
	int rtChannels = (int)getctrl("mrs_natural/nChannels")->toNatural();

	//create new RtAudio object (delete any existing one)
	delete audio_;
	try {
		audio_ = new RtAudio(0, 0, 0, rtChannels, rtFormat,
			rtSrate, &bufferSize_, 4);

		data_ = (mrs_real *) audio_->getStreamBuffer();
	}
	catch (RtError &error) 
	{
		error.printMessage();
	}

	//update bufferSize control which may have been changed
	//by RtAudio (see RtAudio documentation)
	setctrl("mrs_natural/bufferSize", (mrs_natural)bufferSize_);

	isInitialized_ = true;
	//stopped_ = true;
}

void 
AudioSource2::start()
{
	if ( stopped_ ) {
		audio_->startStream();
		stopped_ = false;
	}
}


void 
AudioSource2::stop()
{
	if ( !stopped_ ) {
		audio_->stopStream();
		stopped_ = true;
	}
}

void
AudioSource2::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}

void 
AudioSource2::myProcess(realvec& in, realvec& out)
{
	checkFlow(in,out);

	//check if RtAudio is initialized
	if (!isInitialized_)
		return;

	//check MUTE
	if(getctrl("mrs_bool/mute")->toBool()) return;

	//assure that RtAudio thread is running
	//(this may be needed by if an explicit call to start()
	//is not done before ticking or calling process() )
	if ( stopped_ )
		start();

	//send audio to output
	while (ri_ < inSamples_ * inObservations_)
	{
		try 
		{
			audio_->tickStream();
		}
		catch (RtError &error) 
		{
			error.printMessage();
		}

		for (t=0; t < inObservations_ * bufferSize_; t++)
		{
			reservoir_(ri_) = data_[t];
			ri_++;
		}
	}

	for (o=0; o < inObservations_; o++)
		for (t=0; t < inSamples_; t++)
		{
			out(o,t) = getctrl("mrs_real/gain")->toReal() * reservoir_(inObservations_ * t + o);
		}

		for (t=inSamples_*inObservations_; t < ri_; t++)
			reservoir_(t-inSamples_ * inObservations_) = reservoir_(t);

		ri_ = ri_ - inSamples_ * inObservations_;
}










