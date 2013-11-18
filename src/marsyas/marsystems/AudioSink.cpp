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
#include "AudioSink.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>

using std::ostringstream;
using std::cout;
using std::endl;
using std::min;

using namespace Marsyas;

AudioSink::AudioSink(mrs_string name):
  MarSystem("AudioSink", name),
  old_source_block_size_(-1),
  old_dest_block_size_(-1)
{
  audio_ = NULL;

  isInitialized_ = false;
  stopped_ = true;//lmartins

  addControls();
}

AudioSink::~AudioSink()
{
  delete audio_;
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

  addControl("mrs_string/backend", "");

  addControl("mrs_bool/realtime", false);
  setControlState("mrs_bool/realtime", true);
}

void
AudioSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AudioSink::myUpdate");

  MarSystem::myUpdate(sender);

  mrs_natural source_block_size = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural dest_block_size = getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
  mrs_natural sample_rate = (mrs_natural) getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural channel_count = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  bool realtime = getControl("mrs_bool/realtime")->to<mrs_bool>();

  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
  {
    stop();

    initRtAudio(sample_rate, &dest_block_size, channel_count, realtime);

    const bool do_resize_buffer = true;
    reformatBuffer(source_block_size, dest_block_size, channel_count, realtime, do_resize_buffer);

    shared.sample_rate = sample_rate;
    shared.channel_count = channel_count;
    shared.underrun = false;

    isInitialized_ = true;

    //update bufferSize control which may have been changed
    //by RtAudio (see RtAudio documentation)
    setctrl("mrs_natural/bufferSize", (mrs_natural) dest_block_size);

    setctrl("mrs_bool/initAudio", false);
  }
  else if (isInitialized_)
  {
    const bool do_not_resize_buffer = false;

    if (dest_block_size != old_dest_block_size_
        || sample_rate != shared.sample_rate
        || (realtime != (shared.watermark == 0))
        || !reformatBuffer(source_block_size,
                           dest_block_size,
                           channel_count,
                           realtime,
                           do_not_resize_buffer) )
    {
      MRSERR("AudioSink: Reinitialization required!");
      // stop processing until re-initialized;
      stop();
      isInitialized_ = false;
    }
  }

  old_source_block_size_ = source_block_size;
  old_dest_block_size_ = dest_block_size;
}


void
AudioSink::initRtAudio(mrs_natural sample_rate,
  mrs_natural *block_size,
  mrs_natural channel_count,
  bool realtime
)
{
  mrs_string backend = getControl("mrs_string/backend")->to<mrs_string>();

  RtAudio::Api rt_audio_api = RtAudio::UNSPECIFIED;
  if (!backend.empty()) {
    if (backend == "jack")
      rt_audio_api = RtAudio::UNIX_JACK;
    else if (backend == "alsa")
      rt_audio_api = RtAudio::LINUX_ALSA;
    else if (backend == "pulse")
      rt_audio_api = RtAudio::LINUX_PULSE;
    else if (backend == "oss")
      rt_audio_api = RtAudio::LINUX_OSS;
    else if (backend == "core-audio")
      rt_audio_api = RtAudio::MACOSX_CORE;
    else if (backend == "asio")
      rt_audio_api = RtAudio::WINDOWS_ASIO;
    else if (backend == "direct-sound")
      rt_audio_api = RtAudio::WINDOWS_DS;
    else
      MRSWARN("AudioSink: audio backend '" << backend << "' not supported.");
  }

  if (audio_ == NULL)
    audio_ = new RtAudio(rt_audio_api);
  else if (audio_->isStreamOpen())
    audio_->closeStream();

  int device_id = (int) getctrl("mrs_natural/device")->to<mrs_natural>();
  if (device_id == 0)
  {
    device_id = audio_->getDefaultOutputDevice();
  }



  // expand mono to stereo
  channel_count = std::max((mrs_natural) 2, channel_count);

  RtAudio::StreamParameters output_params;
  output_params.deviceId = device_id;
  output_params.nChannels = channel_count;
  output_params.firstChannel = 0;

  RtAudio::StreamOptions options;
  options.streamName = "Marsyas";
  options.numberOfBuffers = 0; // Use default.
  options.flags = RTAUDIO_SCHEDULE_REALTIME;
  options.priority = 70;
  if (realtime)
    options.flags |= RTAUDIO_MINIMIZE_LATENCY; // Use smallest possible 'numberOfBuffers'.

  RtAudioFormat format = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  // Suppress useless warnings when both an AudioSource and
  // an AudioSink are being opened using ALSA:
  audio_->showWarnings(false);

  try
  {
    unsigned int resulting_block_size;
    audio_->openStream(&output_params, NULL, format, sample_rate,
                       &resulting_block_size, &playCallback, (void *)&shared, &options);
    *block_size = resulting_block_size;
  }
  catch (RtError& e)
  {
    MRSERR("AudioSink: RtAudio error:");
    e.printMessage();
    exit(0);
  }

  audio_->showWarnings(true);
}

void
AudioSink::start()
{
  if ( stopped_ && isInitialized_ )
  {
    clearBuffer();
    audio_->startStream();
    stopped_ = false;
  }
}

void
AudioSink::stop()
{
  if ( !stopped_) {

    audio_->stopStream();
    stopped_ = true;
  }
}


void
AudioSink::localActivate(bool state)
{
  if(state)
    start();
  else
    stop();
}

void AudioSink::clearBuffer()
{
  assert(stopped_);
  shared.buffer.clear();
  shared.underrun = false;
}

bool AudioSink::reformatBuffer(mrs_natural source_block_size,
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
      MRSERR("AudioSink: Can not set requested buffer capacity or channel count without"
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

void
AudioSink::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural t=0; t < inSamples_; t++)
  {
    for (mrs_natural o=0; o < inObservations_; o++)
    {
      out(o,t) = in(o,t);
    }
  }

  //check if RtAudio is initialized
  if (!isInitialized_)
    return;

  // assure that RtAudio thread is running
  // (this may be needed by if an explicit call to start()
  // is not done before ticking or calling process() )
  if ( stopped_ )
  {
    start();
  }

  //check MUTE
  if(ctrl_mute_->isTrue())
  {
    return;
  }

  realvec_queue_producer producer(shared.buffer, onSamples_);

  //cout << "Pushing " << onSamples_ << endl;
  if ((mrs_natural) producer.capacity() < onSamples_)
  {
    //        cout << "Producer waiting..." << endl;
    std::unique_lock<std::mutex> locker(shared.mutex);

    shared.condition.wait (
      locker,
      [&producer, this]()
    {
      //          cout << "Producer awake..." << endl;
      bool ok = producer.reserve(onSamples_);
      if (shared.watermark > 0)
        ok = ok && shared.buffer.write_capacity() >= shared.watermark;
      return ok;
    }
    );

    locker.unlock();
    //cout << "Producer continuing..." << endl;
  }

  for (mrs_natural t=0; t < onSamples_; t++)
  {
    for (mrs_natural o=0; o < onObservations_; o++)
      producer(o,t) = in(o,t);
  }
  //cout << "Pushed " << onSamples_ << endl;
}


int
AudioSink::playCallback(void *outputBuffer, void *inputBuffer,
                        unsigned int nFrames, double streamTime,
                        unsigned int status, void *userData)
{
  (void) inputBuffer;
  (void) streamTime;
  (void) status;


  mrs_real* data = (mrs_real*)outputBuffer;
  OutputData &shared = *((OutputData*)userData);
  unsigned int nChannels = shared.channel_count;

  //cout << "-- Consuming " << nFrames << endl;

  if (shared.underrun)
    shared.underrun = shared.buffer.read_capacity() <= shared.watermark;

  if (!shared.underrun)
  {
    // Limit scope of realvec_queue_consumer!
    realvec_queue_consumer consumer(shared.buffer, nFrames);

    if (consumer.capacity() >= (mrs_natural) nFrames)
    {
      for (unsigned int t=0; t < nFrames; t++)
      {
        unsigned int t2 = t * 2;
        if (nChannels == 1)
        {
          mrs_real val = consumer(0, t);
          data[t2] = val;
          data[t2+1] = val;
        }
        else
        {
          for (unsigned int ch=0; ch < nChannels; ch++) {
            data[nChannels*t+ch] = consumer(ch, t);
          }
        }
      }
      //cout << "-- Consumed." << endl;
    }
    else
    {
      shared.underrun = true;
      MRSWARN("AudioSink: buffer underrun!");
    }
  }

  if (shared.underrun)
  {
    // write silence:
    nChannels = std::max(nChannels, (unsigned int) 2);
    std::memset(outputBuffer, 0, nChannels * nFrames * sizeof(mrs_real));
  }

  shared.mutex.lock();
  shared.condition.notify_all();
  shared.mutex.unlock();

  return 0;
}

