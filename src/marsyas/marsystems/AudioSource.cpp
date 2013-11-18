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

#include "../common_source.h"
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

  mrs_natural source_block_size = getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
  mrs_natural dest_block_size = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural sample_rate = (mrs_natural) getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural channel_count = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
  bool realtime = getControl("mrs_bool/realtime")->to<mrs_bool>();

  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
  {
    stop();

    initRtAudio(sample_rate, &source_block_size, channel_count, realtime);

    const bool do_resize_buffer = true;
    reformatBuffer(source_block_size, dest_block_size, channel_count, realtime, do_resize_buffer);

    shared.sample_rate = sample_rate;
    shared.channel_count = channel_count;
    shared.overrun = false;

    isInitialized_ = true;

    //update bufferSize control which may have been changed
    //by RtAudio (see RtAudio documentation)
    setctrl("mrs_natural/bufferSize", (mrs_natural) source_block_size);

    setctrl("mrs_bool/initAudio", false);
  }
  else if (isInitialized_)
  {
    const bool do_not_resize_buffer = false;

    if (source_block_size != old_source_block_size_
        || sample_rate != shared.sample_rate
        || (realtime != (shared.watermark == 0))
        || !reformatBuffer(source_block_size,
                           dest_block_size,
                           channel_count,
                           realtime,
                           do_not_resize_buffer) )
    {
      MRSERR("AudioSource: Reinitialization required!");
      // stop processing until re-initialized;
      stop();
      isInitialized_ = false;
    }
  }

  old_source_block_size_ = source_block_size;
  old_dest_block_size_ = dest_block_size;
}


void
AudioSource::initRtAudio(
  mrs_natural sample_rate,
  mrs_natural *block_size,
  mrs_natural channel_count,
  bool realtime
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

  RtAudio::StreamOptions options;
  options.streamName = "Marsyas";
  options.numberOfBuffers = 0; // Use default.
  options.flags = RTAUDIO_SCHEDULE_REALTIME;
  options.priority = 70;
  if (realtime)
    options.flags |= RTAUDIO_MINIMIZE_LATENCY; // Use smallest possible 'numberOfBuffers'.

  RtAudioFormat source_format = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  // Suppress useless warnings when both an AudioSource and
  // an AudioSink are being opened using ALSA:
  audio_->showWarnings(false);

  try
  {
    unsigned int uint_block_size = *block_size;
    audio_->openStream(NULL, &source_params, source_format, sample_rate,
                       &uint_block_size, &recordCallback, (void *)&shared, &options);
    *block_size = uint_block_size;
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

bool AudioSource::reformatBuffer(mrs_natural source_block_size,
                                 mrs_natural dest_block_size,
                                 mrs_natural channel_count,
                                 bool realtime, bool resize)
{
  mrs_natural new_capacity = source_block_size + dest_block_size + 1;
  if (!realtime)
    new_capacity = std::max( new_capacity * 4, (mrs_natural) 2000 );

  if (resize)
  {
    assert(stopped_);
    mrs_natural size = new_capacity * 2;
    if (size != shared.buffer.samples() || channel_count != shared.buffer.observations())
    {
      bool do_clear_buffer = true;
      shared.buffer.resize(channel_count, size, new_capacity, do_clear_buffer);
    }
    else
    {
      shared.buffer.set_capacity(new_capacity);
    }
    shared.watermark = realtime ? 0 : new_capacity / 2;
  }
  else
  {
    if (channel_count != shared.buffer.observations()
        || new_capacity > shared.buffer.samples())
    {
      MRSERR("AudioSource: Can not set requested buffer capacity or channel count without"
             " resizing the buffer!");
      return false;
    }

    //cout << "Changing capacity: " << new_capacity << "/" << shared.buffer.samples() << endl;
    mrs_natural new_watermark = realtime ? 0 : new_capacity / 2;;
    if (new_capacity > shared.buffer.capacity())
    {
      // First increase capacity, then watermark.
      shared.buffer.set_capacity(new_capacity);
      shared.watermark = new_watermark;
    }
    else
    {
      // First decrease watermark, then capacity.
      shared.watermark = new_watermark;
      shared.buffer.set_capacity(new_capacity);
    }
  }

  return true;
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

