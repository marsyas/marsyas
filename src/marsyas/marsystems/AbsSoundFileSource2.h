/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_ABSSOUNDFILESOURCE2_H
#define MARSYAS_ABSSOUNDFILESOURCE2_H

#include <marsyas/system/MarSystem.h>
#include <string>

namespace Marsyas
{
/**
	\class AbsSoundFileSource2
	\ingroup Experimental
	\brief Abstract Interface for SoundFileSource of various formats
*/


class AbsSoundFileSource2: public MarSystem
{
private:
  void addControls();

protected:
  std::string filename_;

  mrs_natural nChannels_;

  mrs_natural size_;  //size in number of samples per channel

  realvec audioRegion_;

  virtual bool getHeader();// = 0;

public:
  AbsSoundFileSource2(std::string type, std::string name);
  //AbsSoundFileSource2(const AbsSoundFileSource2& a); //[!][?]
  virtual ~AbsSoundFileSource2();

  MarSystem* clone() const;

  virtual realvec& getAudioRegion(mrs_natural startSample, mrs_natural endSample);// = 0;

  virtual void myProcess(realvec& in,realvec &out);// = 0;
};

}//namespace Marsyas

#endif    /* !MARSYAS_ABSSOUNDFILESOURCE2_H */



