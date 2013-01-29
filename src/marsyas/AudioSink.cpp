/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "common_source.h"
#include "AudioSink.h"

 
using std::ostringstream;
using std::cout;
using std::endl;
using std::min;

using namespace Marsyas;


#ifdef MARSYAS_AUDIOIO
#include "RtAudio.h"
#endif 



AudioSink::AudioSink(mrs_string name):MarSystem("AudioSink", name)
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
	rtDevice_ = 0;

	addControls();
}

AudioSink::~AudioSink()
{
#ifdef MARSYAS_AUDIOIO
	delete audio_;
#endif 

}

MarSystem* 
AudioSink::clone() const
{
	return new AudioSink(*this);
}

void 
AudioSink::addControls()
{
  
  //TODO: Why is this still here?
#ifdef MARSYAS_MACOSX
	addctrl("mrs_natural/bufferSize", 512);
#else
	addctrl("mrs_natural/bufferSize", 512);
#endif

	addctrl("mrs_bool/initAudio", false);
	setctrlState("mrs_bool/initAudio", true);
  
	addctrl("mrs_natural/device", 0);
  
}

void 
AudioSink::myUpdate(MarControlPtr sender)
{
	MRSDIAG("AudioSink::myUpdate");

	MarSystem::myUpdate(sender);
	//Set ringBuffer size
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

	if (inSamples_ < bufferSize_) 
		ringBufferSize_ = 16 * bufferSize_;
	else 
    {
		ringBufferSize_ = 16 * inSamples_;
    }
	odata_.ringBufferSize = ringBufferSize_;
	odata_.high_watermark = ringBufferSize_ / 4;
	odata_.low_watermark =  ringBufferSize_ /8;
	odata_.samplesInBuffer = 0;
	

	

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
AudioSink::initRtAudio()
{
	
	rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
	srate_ = rtSrate_;
	bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
	rtDevice_= (int)getctrl("mrs_natural/device")->to<mrs_natural>();

#ifdef MARSYAS_MACOSX
	// hack to get 22050Hz sound files to play 
	// ok on OS X that by default supports on 44100 
	// without utilizing explicit resampling 
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
	if (rtDevice_ == 0)
 	{
 		rtDevice_ = audio_->getDefaultOutputDevice();
 	}
	oParams.deviceId = rtDevice_;
	oParams.nChannels = rtChannels_;
	oParams.firstChannel = 0; 

	odata_.ringBuffer = &ringBuffer_;
	odata_.wp = 0;
	odata_.rp = 0;
	odata_.ringBufferSize = ringBufferSize_;
	odata_.inchannels = inObservations_;
	odata_.myself = this;
	odata_.srate = srate_;
	

	//marsyas represents audio data as float numbers
	RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  

	rtFormat = RTAUDIO_FLOAT64;
	
	// RtAudio::StreamOptions options;
	audio_->showWarnings(true);

	//options.flags |= RTAUDIO_HOG_DEVICE;
	//options.flags |= RTAUDIO_SCHEDULE_REALTIME;	
	//options.flags |= RTAUDIO_NONINTERLEAVED;
	

    if (rtDevice_ != audio_->getDefaultOutputDevice())
    {
        RtAudio::DeviceInfo info;
        info = audio_->getDeviceInfo(rtDevice_);
    }
  
  
   try 
   {
	   audio_->openStream(&oParams, NULL, rtFormat, rtSrate_, 
	   &bufferFrames, &playCallback, (void *)&odata_, NULL);
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
AudioSink::start()
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
AudioSink::playCallback(void *outputBuffer, void *inputBuffer, 
								unsigned int nBufferFrames, double streamTime, 
								unsigned int status, void *userData)
{
    (void) inputBuffer;
    (void) streamTime;
    (void) status;

	unsigned int drain_count	= 0;
	
	mrs_real* data = (mrs_real*)outputBuffer;
	OutputData *odata = (OutputData *)userData;
	//AudioSink* mythis = (AudioSink *)odata_->myself;
	realvec& ringBuffer = *(odata->ringBuffer);
	unsigned int t;

	if (odata->srate == 22050) 
		nBufferFrames = nBufferFrames/2;
	static int count = 0;
	count ++;

	unsigned int samplesAvailable = 0;
	
	for (t=0; t < nBufferFrames; t++)
	{
		if (odata->wp >= odata->rp)
			samplesAvailable = odata->wp - odata->rp;
		else 
			samplesAvailable = odata->ringBufferSize - (odata->rp - odata->wp);
		

		if (samplesAvailable >= odata->low_watermark)
		{
			
			const int t4 = 4 * t;
			const int t2 = 2 * t;
			
			if (odata->srate == 22050)		// hack to get 22050 working without resampling                                        // on OS X that typically only support 44k
			{	      
				
				if (odata->inchannels == 1) 
				{
					mrs_real val = ringBuffer(0, odata->rp);
					data[t4] = val;
					data[t4+1] = val;
					data[t4+2] = val;
					data[t4+3] = val;
				}
				else
				{
					for (int j=0; j < (int)odata->inchannels; j++)
					{
						data[t4+j] = ringBuffer(0+j,odata->rp);
						data[t4+1+j] = ringBuffer(0+j,odata->rp);
					}
				}
			}
			else						// default case - use actual srate
			{
				
				if (odata->inchannels == 1) 
				{
					mrs_real val = ringBuffer(0,odata->rp);
					data[t2] = val;
					data[t2+1] = val;
					
				}
				else 
				{

					
					for (int j=0; j < (int)odata->inchannels; j++) 
					{
						data[t2+j] = ringBuffer(j,odata->rp);
					}
				}
			}
			odata->rp = ++(odata->rp) % odata->ringBufferSize;


			if (odata->wp >= odata->rp)
				samplesAvailable = odata->wp - odata->rp;
			else 
				samplesAvailable = odata->ringBufferSize - (odata->rp - odata->wp);
			


		}
		
	}
	
		
		
	while (samplesAvailable < odata->low_watermark)
	{
		SLEEP(5);
		if (odata->wp >= odata->rp)
			samplesAvailable = odata->wp - odata->rp;
		else 
			samplesAvailable = odata->ringBufferSize - (odata->rp - odata->wp);
		
		drain_count++;
		if (drain_count == 200)
		{
			return 1;
		}
		
	}
	
	return 0;
}

#endif




void 
AudioSink::stop()
{
#ifdef MARSYAS_AUDIOIO
	if ( !stopped_) {

		audio_->stopStream();
		stopped_ = true;
	}
#endif 
}


void
AudioSink::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}


unsigned int 
AudioSink::getSpaceAvailable() 
{
	unsigned int free = (odata_.rp - odata_.wp -1 + odata_.ringBufferSize) % odata_.ringBufferSize;
	unsigned int underMark = odata_.high_watermark - getSamplesAvailable();
	
	return(min(underMark, free));
}


unsigned int 
AudioSink::getSamplesAvailable() 
{
	unsigned int samplesAvailable = (odata_.wp - odata_.rp +odata_.ringBufferSize) % odata_.ringBufferSize;
	return samplesAvailable;
}


void 
AudioSink::myProcess(realvec& in, realvec& out)
{
	
	static int count = 0;
	count ++;
	 

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
    } else {
		// copy to output 
		for (t=0; t < inSamples_; t++)
		{
			for (o=0; o < inObservations_; o++)
			{
				out(o,t) = in(o,t);
			}
		}
		
		
#ifdef MARSYAS_AUDIOIO
		//check if RtAudio is initialized
		if (!isInitialized_)
			return;
		
		unsigned int samplesInBuffer; 
		//unsigned int free ; 
		//unsigned int underMark; 
		

		// write samples to reservoir 
		for (t=0; t < onSamples_; t++)		
		{
			samplesInBuffer = (odata_.wp - odata_.rp +odata_.ringBufferSize) % odata_.ringBufferSize;
			
			if (odata_.wp >= odata_.rp)
				samplesInBuffer = odata_.wp - odata_.rp;
			else 
				samplesInBuffer = odata_.ringBufferSize - (odata_.rp - odata_.wp);
			
			if (samplesInBuffer < odata_.high_watermark)
			{
				
				for (o=0; o < onObservations_; o++) 
					ringBuffer_(o,odata_.wp) = in(o,t);
				odata_.wp = ++ (odata_.wp) % odata_.ringBufferSize;				
			}
			else 
			{
				t--;
				while (samplesInBuffer >= odata_.high_watermark)
				{
					SLEEP(5);
					// odata_.rp = (odata_.rp+512) % odata_.ringBufferSize;
					
					if (odata_.wp >= odata_.rp)
						samplesInBuffer = odata_.wp - odata_.rp;
					else 
						samplesInBuffer = odata_.ringBufferSize - (odata_.rp - odata_.wp);
				}
			}
		}
#endif
	}
	
	
	// assure that RtAudio thread is running
	// (this may be needed by if an explicit call to start()
	// is not done before ticking or calling process() )
	if ( stopped_ )
	{
	start();
	}
}

