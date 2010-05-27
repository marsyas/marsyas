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

#include "common.h"
#include "AudioSinkCallback.h"

using namespace std;
using namespace Marsyas;


#ifdef MARSYAS_AUDIOIO
#include "RtAudio.h"
#endif 



AudioSinkCallback::AudioSinkCallback(string name):MarSystem("AudioSinkCallback", name)
{
#ifdef MARSYAS_AUDIOIO
	audio_ = NULL;
#endif
  
	pringBufferSize_ = 0;
	pnChannels_ = 1;


	rtSrate_ = 0;
  
	isInitialized_ = false;
	stopped_ = true;//lmartins
  
	rtSrate_ = 0;
	bufferSize_ = 0;
	rtChannels_ = 0;
	rtDevice_ = -1;
  


	addControls();
}

AudioSinkCallback::~AudioSinkCallback()
{
#ifdef MARSYAS_AUDIOIO
	delete audio_;
#endif 

}

MarSystem* 
AudioSinkCallback::clone() const
{
	return new AudioSinkCallback(*this);
}

void 
AudioSinkCallback::addControls()
{
  
#ifdef MARSYAS_MACOSX
	addctrl("mrs_natural/bufferSize", 256);
#else
	addctrl("mrs_natural/bufferSize", 256);
#endif

	addctrl("mrs_bool/initAudio", false);
	setctrlState("mrs_bool/initAudio", true);
  
	addctrl("mrs_natural/device", -1);
  
}

void 
AudioSinkCallback::myUpdate(MarControlPtr sender)
{
	MRSDIAG("AudioSinkCallback::myUpdate");

	MarSystem::myUpdate(sender);
	//Set ringBuffer size
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

	if (inSamples_ < bufferSize_) 
		ringBufferSize_ = 8 * bufferSize_;
	else 
    {
		ringBufferSize_ = 8 * inSamples_;
    }
	odata.ringBufferSize = ringBufferSize_;
	odata.high_watermark = ringBufferSize_ / 4;
	odata.low_watermark =  ringBufferSize_ /8;
	
	// resize if necessary 
	nChannels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();	
	if ((ringBufferSize_ > pringBufferSize_)||(nChannels_ != pnChannels_))
    {
		ringBuffer_.stretch(nChannels_, ringBufferSize_);
    }
	pringBufferSize_ = ringBufferSize_;
	pnChannels_ = nChannels_;

	

	// initialize RtAudio
	if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
		initRtAudio();
}


void 
AudioSinkCallback::initRtAudio()
{
	
	rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
	srate_ = rtSrate_;
	bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
	rtDevice_= (int)getctrl("mrs_natural/device")->to<mrs_natural>();

#ifdef MARSYAS_MACOSX
	if (rtSrate_ == 22050) 
  {
		rtSrate_ = 44100;
		bufferSize_ = 2 * bufferSize_;
  }
#endif	

#ifdef MARSYAS_AUDIOIO
	if (audio_ == NULL)
		audio_ = new RtAudio();

	unsigned int bufferFrames;
	bufferFrames = bufferSize_;

	// hardwire channels to stereo playback even for mono
	rtChannels_ = 2;
	
	RtAudio::StreamParameters oParams;
  if (rtDevice_ < 0)
    rtDevice_ = audio_->getDefaultOutputDevice();

	oParams.deviceId = rtDevice_;
	oParams.nChannels = rtChannels_;
	oParams.firstChannel = 0; 

	odata.ringBuffer = &ringBuffer_;
	odata.wp = 0;
	odata.rp = 0;
	odata.ringBufferSize = ringBufferSize_;
	odata.inchannels = inObservations_;
	odata.myself = this;
	

	//marsyas represents audio data as float numbers
	RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  
  if (rtDevice_ != audio_->getDefaultOutputDevice())
  {
      RtAudio::DeviceInfo info;
      info = audio_->getDeviceInfo(rtDevice_);
      cout << "Using output device: " << info.name << endl;
  }

	try 
	{
		audio_->openStream(&oParams, NULL, rtFormat, rtSrate_, 
						  &bufferFrames, &playCallback, (void *)&odata, NULL);
	}
	catch (RtError& e) 
	{
		e.printMessage();
		exit(0);
	}

	
  
	//update bufferSize control which may have been changed
	//by RtAudio (see RtAudio documentation)
	setctrl("mrs_natural/bufferSize", (mrs_natural)bufferFrames);
  
#endif
	isInitialized_ = true;
	setctrl("mrs_bool/initAudio", false);

}

void 
AudioSinkCallback::start()
{
#ifdef MARSYAS_AUDIOIO
	if ( stopped_) {
		audio_->startStream();
		stopped_ = false;
	}
#endif
}



#ifdef MARSYAS_AUDIOIO

int 
AudioSinkCallback::playCallback(void *outputBuffer, void *inputBuffer, 
						unsigned int nBufferFrames, double streamTime, 
								unsigned int status, void *userData)
{
	unsigned int drain_count;
	
	mrs_real* data = (mrs_real*)outputBuffer;
	OutputData *oData = (OutputData *)userData;
	//AudioSinkCallback* mythis = (AudioSinkCallback *)oData->myself;
	realvec& ringBuffer = *(oData->ringBuffer);
	unsigned int t;
   
	for (t=0; t < nBufferFrames; t++)
	{
		if (oData->samplesInBuffer >= oData->low_watermark)
		{
			
			if (oData->inchannels == 1) 
			{
				data[2*t] = ringBuffer(0,oData->rp);
				data[2*t+1] = ringBuffer(0,oData->rp);
			}
			else 
			{
				data[2*t] = ringBuffer(0,oData->rp);
				data[2*t+1] = ringBuffer(1,oData->rp);
				
			}
			oData->rp = ++(oData->rp) % oData->ringBufferSize;
			if (oData->wp >= oData->rp) 
				oData->samplesInBuffer = oData->wp - oData->rp;
			else 
				oData->samplesInBuffer = oData->ringBufferSize - (oData->rp - oData->wp);
		}
	}
	
	while (oData->samplesInBuffer < oData->low_watermark)
	{
		// block until there are available samples 
		SLEEP(1);  // 1 millisecond 
		drain_count++;
		if (drain_count == 1000)
			return 1;
	}
	return 0;
}

#endif


void 
AudioSinkCallback::stop()
{
#ifdef MARSYAS_AUDIOIO
	if ( !stopped_) {

		audio_->stopStream();
		stopped_ = true;
	}
#endif 
}


void
AudioSinkCallback::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}


unsigned int 
AudioSinkCallback::getSpaceAvailable() 
{
	unsigned int free = (odata.rp - odata.wp -1 + odata.ringBufferSize) % odata.ringBufferSize;
	unsigned int underMark = odata.high_watermark - getSamplesAvailable();
	
	return(min(underMark, free));
}


unsigned int 
AudioSinkCallback::getSamplesAvailable() 
{
	unsigned int samplesAvailable = (odata.wp - odata.rp +odata.ringBufferSize) % odata.ringBufferSize;
	return samplesAvailable;
}


void 
AudioSinkCallback::myProcess(realvec& in, realvec& out)
{
	mrs_natural t,o;
	 
	//check MUTE
	if(ctrl_mute_->isTrue())
    {
		for (t=0; t < inSamples_; t++)
    	{
			for (o=0; o < inObservations_; o++)
			{
				out(o,t) = in(o,t);
			}
		}
    }
	
  	// copy to output and into reservoir
	for (t=0; t < inSamples_; t++)
    {
		for (o=0; o < inObservations_; o++)
		{
			out(o,t) = in(o,t);
		}
	}
	
	//check if RtAudio is initialized
	if (!isInitialized_)
		return;


	
	// write samples to reservoir 
	for (t=0; t < onSamples_; t++)		
	{
		if (odata.samplesInBuffer <= odata.high_watermark)
		{
			
			for (o=0; o < onObservations_; o++) 
				ringBuffer_(o,odata.wp) = in(o,t);
			
			odata.wp = ++ (odata.wp) % odata.ringBufferSize;
			if (odata.wp >= odata.rp) 
				odata.samplesInBuffer = odata.wp - odata.rp;
			else 
				odata.samplesInBuffer = odata.ringBufferSize - (odata.rp - odata.wp);
		}
		else 
		{
			while (odata.samplesInBuffer > odata.high_watermark)
			{
				SLEEP(1);
			}
		}
	}
	
	
	
	
	//assure that RtAudio thread is running
	//(this may be needed by if an explicit call to start()
	//is not done before ticking or calling process() )
	if ( stopped_ )
    {
		start();
    }


}

 









	
