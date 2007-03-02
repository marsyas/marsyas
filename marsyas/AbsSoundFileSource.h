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

/**
   \class AbsSoundFileSource
   \brief Abstract Interface for SoundFileSource of various formats
*/

#ifndef MARSYAS_ABSSOUNDFILESOURCE_H
#define MARSYAS_ABSSOUNDFILESOURCE_H

#include "MarSystem.h" 
#include <string> 

namespace Marsyas
{

class AbsSoundFileSource: public MarSystem
{
public:
  AbsSoundFileSource(std::string type, std::string name);
  virtual ~AbsSoundFileSource();
  AbsSoundFileSource(const AbsSoundFileSource& a);

  virtual void getHeader(std::string filename) = 0;
  virtual void myProcess(realvec& in,realvec &out) = 0;

  // public for efficiency [!]
  mrs_natural pos_;
  mrs_natural rewindpos_;
  bool notEmpty_;
};

}//namespace Marsyas

#endif    /* !MARSYAS_ABSSOUNDFILESOURCE_H */ 

	
	
