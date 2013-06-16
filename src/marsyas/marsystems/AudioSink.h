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


#ifndef MARSYAS_AUDIOSINK_H
#define MARSYAS_AUDIOSINK_H


#ifdef MARSYAS_AUDIOIO
#include "RtAudio.h"
#endif 
#include "MarSystem.h"

#include "Thread.h" 


class RtAudio;
class Thread;



namespace Marsyas
{
/**
   \class AudioSink
   \ingroup IO
   \brief Real-time Audio source for Linux

   Real-time Audio Sink based on RtAudio.

   Controls:
   - \b mrs_natural/bufferSize [rw] : size of audio buffer (in samples)
   - \b mrs_bool/initAudio [w] : initialize audio (this should be \em true)
*/


    class AudioSink:public MarSystem
    {
    private:
        
        struct OutputData 
        {
            mrs_realvec* ringBuffer;
            volatile long wp;
            volatile long rp;
            unsigned int samplesInBuffer;
            unsigned int ringBufferSize;
            unsigned int inchannels;
            unsigned int high_watermark;
            unsigned int low_watermark;
            int srate;
            AudioSink* myself;
                
        } odata_;
        
        

        RtAudio*  audio_;

        int bufferSize_;
        int rtSrate_;
        int rtChannels_;
        int srate_;
        unsigned int rtDevice_;
        
        bool isInitialized_;
        bool stopped_;
        
        mrs_natural nChannels_;
        mrs_real *data_;  
        realvec ringBuffer_;
        mrs_natural ringBufferSize_;
        mrs_natural pringBufferSize_;
        mrs_natural pnChannels_;


        void addControls();
        void myUpdate(MarControlPtr sender);

        void initRtAudio();

        void start();
        void stop();

        unsigned int getSpaceAvailable();
        unsigned int getSamplesAvailable();
        void localActivate(bool state);

        static int playCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, unsigned int status, void *userData);
		void playCallback_test();
		

    public:
        AudioSink(std::string name);
        ~AudioSink();
        MarSystem* clone() const;  

        void myProcess(realvec& in, realvec& out);
    };
    
}//namespace Marsyas

#endif
