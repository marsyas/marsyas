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

#ifndef MARSYAS_AUDIOSOURCE_H
#define MARSYAS_AUDIOSOURCE_H



#include <marsyas/realtime/realvec_queue.h>
#include <marsyas/system/MarSystem.h>

#include "RtAudio.h"

#include <mutex>
#include <condition_variable>
#include <atomic>

class RtAudio;

namespace Marsyas
{

/**
   \class AudioSource
   \ingroup IO
   \brief Receives audio from hardware inputs.
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
   initialization takes place and no data will be received from the audio input.
   This allows you to trigger initialization only after having set other controls.
   - \b mrs_bool/hasData [r] : is there any audio input left?
     (End-Of-File not yet reached)
   - \b mrs_real/gain [w] : scales input
   - \b mrs_natural/nChannels [DOCME] : DOCME
   - \b mrs_natural/nBuffers [DOCME] : DOCME
*/




class AudioSource:public MarSystem
{
private:

  struct InputData
  {
    InputData(): overrun(false) {}
    InputData(const InputData &): overrun(false) {}

    std::mutex mutex;
    std::condition_variable condition;

    realvec_queue buffer;

    std::atomic<mrs_natural> watermark;
    bool overrun;

    unsigned int sample_rate;
    unsigned int channel_count;
  } shared;

  mrs_natural old_source_block_size_;
  mrs_natural old_dest_block_size_;

  RtAudio* audio_;

  bool isInitialized_;
  bool stopped_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  void initRtAudio(
    mrs_natural sample_rate,
    mrs_natural *block_size,
    mrs_natural channel_count,
    bool realtime
  );

  void start();
  void stop();

  void localActivate(bool state);

  void clearBuffer();
  bool reformatBuffer(mrs_natural sourceBlockSize,
                      mrs_natural destBlockSize,
                      mrs_natural channel_count,
                      bool realtime, bool resize);

  static int recordCallback(void *outputBuffer, void *inputBuffer,
                            unsigned int nBufferFrames, double streamTime, unsigned int status, void *userData);

public:
  AudioSource(std::string name);
  ~AudioSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif




