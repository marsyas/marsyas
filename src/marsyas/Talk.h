/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_TALK_H
#define MARSYAS_TALK_H

#include <marsyas/marsystems/AbsMax.h>
#include <marsyas/marsystems/SoundFileSource.h>
#include <marsyas/marsystems/Series.h>
#include <marsyas/marsystems/Accumulator.h>
#include <marsyas/marsystems/AudioSink.h>
#include <marsyas/TimeLine.h>
#include "Communicator.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define SEPCHARS " \t\n"
#define UNSPECIFIED -1
#define WIN_SIZE 512

#ifdef WIN32
#define MARSYAS_MFDIR ""
#endif

namespace Marsyas
{
/**
    \class Talk
	\ingroup none
    \brief Message handler

    Reacts to various messages using Communicator.
Specifically used for handling the graphical user interface events.
*/


class Talk
{
private:
  Communicator* communicator_;
  void cmd_load(std::string fname, mrs_natural lineSize);
  void cmd_play(mrs_natural start, mrs_natural end, mrs_natural lineSize);

  void cmd_merge(std::string systemName, unsigned int pstart, unsigned int pend,
                 unsigned int start,  unsigned int end,
                 unsigned int winSize);
  void cmd_classify(std::string system, std::string classifier, unsigned int start, unsigned int end, unsigned int winSize);
  void cmd_extract(std::string systemName, std::string fileName, TimeLine tline);
  void cmd_segment(std::string systemName, unsigned int memSize, unsigned int numPeaks, unsigned int peakSpacing, unsigned int start, unsigned int end, unsigned int winSize);

  void cmd_fullfft(unsigned int winSize);
  void cmd_fft(unsigned int start, unsigned int winSize);
  void cmd_colorgram(std::string collection, std::string system, unsigned int start, unsigned int end, unsigned int win_size);
  SoundFileSource *src_;
  AudioSink *dest_;

  std::string fname_;

public:
  Talk();
  ~Talk();
  void init(Communicator* communicator);
  void process(char *message);
};

}//namespace Marsyas




#endif





