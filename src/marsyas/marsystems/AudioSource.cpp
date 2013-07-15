/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include "AudioSource.h"

#include <algorithm>
#include <cassert>

using namespace std;
using namespace Marsyas;

AudioSource::AudioSource(string name):MarSystem("AudioSource", name)
{
	audio_ = NULL;

	isInitialized_ = false;
	stopped_ = true;

	addControls();
}

AudioSource::~AudioSource()
{
	delete audio_;

}


MarSystem* 
AudioSource::clone() const
{
	return new AudioSource(*this);
}

void 
AudioSource::addControls()
{
	addctrl("mrs_natural/nChannels", 1);


#ifdef MARSYAS_MACOSX
	addctrl("mrs_natural/bufferSize", 1024);
#else
	addctrl("mrs_natural/bufferSize", 256);
#endif


	addctrl("mrs_bool/initAudio", false);
	setctrlState("mrs_bool/initAudio", true);

	addctrl("mrs_bool/hasData", true);
	addctrl("mrs_real/gain", 1.0);

	addControl("mrs_bool/realtime", false);
	setControlState("mrs_bool/realtime", true);
}

void 
AudioSource::myUpdate(MarControlPtr sender)
{
	(void) sender;  //suppress warning of unused parameter(s)
	MRSDIAG("AudioSource::myUpdate");

	//set output controls
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/nChannels"));

	unsigned int source_block_size = getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
	unsigned int sample_rate = getctrl("mrs_real/israte")->to<mrs_real>();
	unsigned int channel_count = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
	unsigned int dest_block_size = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	bool realtime = getControl("mrs_bool/realtime")->to<mrs_bool>();

	if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
	{
		stop();

		initRtAudio(sample_rate, &source_block_size, channel_count);

		unsigned int ring_buffer_size;
		if (realtime)
			ring_buffer_size = source_block_size + dest_block_size;
		else
			ring_buffer_size = (source_block_size + dest_block_size) * 5;

		if (ring_buffer_size != shared.buffer_size
		|| channel_count != shared.channel_count)
		{
			reformatBuffer(channel_count, ring_buffer_size);
		}

		shared.watermark = realtime ? 0 : ring_buffer_size / 2;
		shared.buffer_size = ring_buffer_size;
		shared.source_block_size = source_block_size;
		shared.sample_rate = sample_rate;
		shared.channel_count = channel_count;
		shared.myself = this;

		isInitialized_ = true;

		//update bufferSize control which may have been changed
		//by RtAudio (see RtAudio documentation)
		setctrl("mrs_natural/bufferSize", (mrs_natural) source_block_size);

		setctrl("mrs_bool/initAudio", false);
	}
	else if (isInitialized_)
	{
		if
		( channel_count != shared.channel_count
		|| source_block_size != shared.source_block_size
		|| sample_rate != shared.sample_rate
		|| (realtime != (shared.watermark == 0)) )
		{
			// stop processing until re-initialized;
			stop();
			isInitialized_ = false;
		}
	}
}


void 
AudioSource::initRtAudio(
unsigned int sample_rate,
unsigned int *block_size,
unsigned int channel_count
)
{
	//marsyas represents audio data as float numbers
	if (audio_ == NULL)
	{
		audio_ = new RtAudio();
	}
	else if (audio_->isStreamOpen())
	{
		audio_->closeStream();
	}

	RtAudio::StreamParameters source_params;
	source_params.deviceId = audio_->getDefaultInputDevice();
	source_params.nChannels = channel_count;
	source_params.firstChannel = 0;
	
	RtAudioFormat source_format = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

	// Suppress useless warnings when both an AudioSource and
	// an AudioSink are being opened using ALSA:
	audio_->showWarnings(false);

	try 
	{
		audio_->openStream(NULL, &source_params, source_format, sample_rate,
						   block_size, &recordCallback, (void *)&shared, NULL);
	}
	catch (RtError& e)
	{
		MRSERR("AudioSource: RtAudio error:");
		e.printMessage();
		exit(0);
	}

	audio_->showWarnings(true);
}

void 
AudioSource::start()
{
	if ( stopped_ && isInitialized_ )
	{
		clearBuffer();
		audio_->startStream();
		stopped_ = false;
	}
}

void 
AudioSource::stop()
{
	if ( !stopped_ )
	{
		audio_->stopStream();
		stopped_ = true;
	}
}

void
AudioSource::localActivate(bool state)
{
	if(state)
	{
		start();
	}
	else
	{
		stop();
	}
}

void AudioSource::clearBuffer()
{
	assert(stopped_);
	shared.buffer.clear();
	shared.overrun = false;
}

void AudioSource::reformatBuffer(int rows, int columns)
{
	assert(stopped_);
	shared.buffer.resize(rows, columns);
	shared.overrun = false;
}

int
AudioSource::recordCallback(void *outputBuffer, void *inputBuffer,
							unsigned int nFrames,
							double streamTime, unsigned int status,
							void *userData)
{
	(void) outputBuffer;
    (void) streamTime;
    (void) status;

	mrs_real* data = (mrs_real*)inputBuffer;
	InputData &shared = *((InputData*) userData);
	unsigned int nChannels = shared.channel_count;

	if (shared.overrun)
		shared.overrun = shared.buffer.write_capacity() <= shared.watermark;

	if (!shared.overrun)
	{
		// Limit scope of realvec_queue_producer!
		realvec_queue_producer producer( shared.buffer, nFrames );

		if (producer.capacity() == nFrames)
		{
			for (unsigned int t=0; t < nFrames; t++)
			{
				for (unsigned int ch=0; ch < nChannels; ch++) {
					producer(ch, t) = data[nChannels*t+ch];
				}
			}
		}
		else
		{
			shared.overrun = true;
			MRSWARN("AudioSource: buffer overrun!");
		}
	}

	shared.mutex.lock();
	shared.condition.notify_all();
	shared.mutex.unlock();

	return 0;
}

void 
AudioSource::myProcess(realvec& in, realvec& out)
{
	(void) in;
	
	//check if RtAudio is initialized
	if (!isInitialized_)
	{
		return;
	}
	
	//check MUTE
	if(ctrl_mute_->isTrue())
	{
		return;
	}

	//assure that RtAudio thread is running
	//(this may be needed by if an explicit call to start()
	//is not done before ticking or calling process() )
	if ( stopped_ )
	{
		start();
	}
	
	realvec_queue_consumer consumer(shared.buffer, onSamples_);

	if ((mrs_natural) consumer.capacity() < onSamples_)
	{
		std::unique_lock<std::mutex> locker(shared.mutex);

		shared.condition.wait (
		locker,
		[&consumer, this]()
		{
			bool ok = consumer.reserve(onSamples_);
			if (shared.watermark > 0)
				ok = ok && shared.buffer.read_capacity() >= shared.watermark;
			return ok;
		}
		);

		locker.unlock();
	}

	MRSASSERT((mrs_natural) consumer.capacity() == onSamples_);

	for (mrs_natural t=0; t < onSamples_; t++)
	{
		for (mrs_natural o=0; o < onObservations_; o++)
		{
			out(o,t) = consumer(o,t);
		}
	}
}

