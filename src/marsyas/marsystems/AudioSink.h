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


#ifndef MARSYAS_AUDIOSINK_H
#define MARSYAS_AUDIOSINK_H

#include <marsyas/realtime/realvec_queue.h>
#include <marsyas/system/MarSystem.h>

#include "RtAudio.h"

#include <mutex>
#include <condition_variable>
#include <atomic>


class RtAudio;
class Thread;



namespace Marsyas
{

/**
   \class AudioSink
   \ingroup IO
   \brief Sends audio to hardware outputs.
   \author George Tzanetakis (gtzan@cs.uvic.ca)
   \author Jakob Leben (jakob.leben@gmail.com)

   Controls:
   - \b mrs_natural/bufferSize [rw] : Desired block size (in frames) for data exchange with the
   audio backend. After initialization, this may be changed according to limits imposed by the
   backend.
   - \b mrs_bool/realtime [w] (default: false) : If true, will minimize buffer to ensure
   smallest possible latency at the cost of tolerance to processing times of other parts of
   the network. After changed, requires re-initialization.
   - \b mrs_bool/initAudio [w] : When set to true, initializes audio backend. Until then, no
   initialization takes place and no data will be sent to the audio output.
   This allows you to trigger initialization only after having set other controls.
*/


class AudioSink:public MarSystem
{
private:

  struct OutputData
  {
    OutputData(): underrun(false) {}
    OutputData(const OutputData &): underrun(false) {}

    std::mutex mutex;
    std::condition_variable condition;

    realvec_queue buffer;

    std::atomic<mrs_natural> watermark;
    bool underrun;

    unsigned int channel_count;
    unsigned int sample_rate;

  } shared;

  mrs_real old_source_sample_rate_;
  mrs_natural old_dest_block_size_;

  realvec resampler_output_;
  MarSystem* resampler_;

  RtAudio*  audio_;

  bool isInitialized_;
  bool stopped_;
  bool is_resampling_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  void initRtAudio(
    unsigned int* sample_rate,
    unsigned int* block_size,
    unsigned int channel_count,
    bool realtime
  );

  void start();
  void stop();

  void localActivate(bool state);

  void configureResampler(mrs_real in_sample_rate, mrs_natural in_block_size,
                          mrs_real out_sample_rate, mrs_natural *out_block_size,
                          mrs_natural channel_count);

  void updateResamplerBlockSize(mrs_natural in_block_size,
                                mrs_natural *out_block_size,
                                mrs_natural channel_count);

  void clearBuffer();
  bool reformatBuffer(mrs_natural sourceBlockSize,
                      mrs_natural destBlockSize,
                      mrs_natural channel_count,
                      bool realtime, bool resize);


  static int playCallback(void *outputBuffer, void *inputBuffer,
                          unsigned int nBufferFrames, double streamTime, unsigned int status, void *userData);
  void playCallback_test();


public:
  AudioSink(std::string name);
  ~AudioSink();
  AudioSink(const AudioSink& a);

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
