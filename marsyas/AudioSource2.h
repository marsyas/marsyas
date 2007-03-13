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
\brief Real-time audio source 

Real-time Audio Source based on RtAudio
*/

#ifndef MARSYAS_AUDIOSOURCE2_H
#define MARSYAS_AUDIOSOURCE2_H

#include "RtAudio.h"
#include "MarSystem.h" 

namespace Marsyas
{

class AudioSource2:public MarSystem
{
	private:
		RtAudio *audio_;
		int bufferSize_;

		mrs_natural ri_;
		mrs_natural nChannels_;
		mrs_real *data_;  
		realvec reservoir_;
		mrs_natural reservoirSize_;
		mrs_natural preservoirSize_;

		mrs_real gain_;

		bool isInitialized_;
		bool stopped_;

		void addControls();
		void myUpdate(MarControlPtr sender);

		void initRtAudio();

		void start();
		void stop();

		void localActivate(bool state);

	public:
		AudioSource2(std::string name);
		~AudioSource2();
		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif




