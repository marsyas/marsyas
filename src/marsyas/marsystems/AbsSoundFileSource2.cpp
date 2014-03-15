/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "AbsSoundFileSource2.h"

using std::ostringstream;
using namespace Marsyas;

AbsSoundFileSource2::AbsSoundFileSource2(mrs_string type, mrs_string name):MarSystem(type, name)
{
  size_ = 0;

  addControls();
}

AbsSoundFileSource2::~AbsSoundFileSource2()
{
}

// AbsSoundFileSource2::AbsSoundFileSource2(const AbsSoundFileSource2& a):MarSystem(a) //[!][?]
// {
// 	filename_ = a.filename_;
//
// 	nChannels_ = a.nChannels_;
//
// 	size_ = a.size_;
// 	pos_ = a.pos_;
//
// 	loopSize_ = a.loopSize_;
// 	//loopDuration_ = a.loopDuration_;
// 	loopRepetitions_ = a.loopRepetitions_;
// 	loopStart_ = a.loopStart_;
// }

MarSystem*
AbsSoundFileSource2::clone() const
{
  return new AbsSoundFileSource2(*this);
}

void
AbsSoundFileSource2::addControls()
{
  //setctrl("mrs_string/onObsNames", "audio,");
  setctrl("mrs_string/inObsNames", "audio,");

  //common controls
  addctrl("mrs_natural/nChannels",(mrs_natural)1);
  //setctrlState("mrs_natural/nChannels", true); //[?] state or stateless? Let each derived class set it up its way...

  addctrl("mrs_natural/pos", (mrs_natural)0);

  addctrl("mrs_bool/hasData", false);

  addctrl("mrs_string/filename", "");
  setctrlState("mrs_string/filename", true);

  addctrl("mrs_string/filetype", "defaulttype");

  addctrl("mrs_natural/size", (mrs_natural)0);//size in number of samples per channel
}

bool
AbsSoundFileSource2::getHeader()
{
  //does nothing => used to create a "DummyFileSource"
  return true;
}

realvec&
AbsSoundFileSource2::getAudioRegion(mrs_natural startSample, mrs_natural endSample)
{
  (void) startSample; (void) endSample;
  //return empty realvec
  return audioRegion_;
}

void
AbsSoundFileSource2::myProcess(realvec& in,realvec &out)
{
  (void) in;
  //send silence to the output => used to create a "DummyFileSource"
  out.setval(0.0);
}
